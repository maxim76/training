#include <stdio.h>
#include <netlink/socket.h>
#include <libnl3/netlink/route/link.h>
#include <libnl3/netlink/route/addr.h>

#define ADDR_STR_BUF_SIZE 80

int main() {

	printf("Hello\n");

	int err;

	// Allocate new netlink socket
	// https://www.infradead.org/~tgr/libnl/doc/api/group__socket.html#gaf9458e6b88324962065f93a13d53cf2c
	struct nl_sock *sock_ptr = nl_socket_alloc();
	if (sock_ptr == nullptr) {
		printf("nl_socket_alloc() failed\n");
		exit(EXIT_FAILURE);
	}
	printf("Allocated socket: %p\n", sock_ptr);

	//printf("Current SEQ: %u\n", nl_socket_use_seq(sk));

	// Bind socket to protocol
	// https://www.infradead.org/~tgr/libnl/doc/api/group__send__recv.html#ga86da21b5037e3b883f307661c3edd22f
	if ((err = nl_connect(sock_ptr, NETLINK_ROUTE)) != 0) {
		printf("nl_connect() error %s\n", nl_geterror(err));
		exit(err);
	}
	
	// Allocate link cache and fill in all configured links.
	// http://charette.no-ip.com:81/programming/doxygen/netfilter/group__link.html
	// https://www.infradead.org/~tgr/libnl/doc/api/group__link.html#ga8fdeaee0a6f872d19e72cd77c141ce73
    struct nl_cache *link_cache;
	err = rtnl_link_alloc_cache(sock_ptr, AF_UNSPEC, &link_cache);
	if (err) {
		printf("rtnl_link_alloc_cache error: %s\n", nl_geterror(err));
		exit(err);
	}

	// Cache iterating: http://charette.no-ip.com:81/programming/doxygen/netfilter/group__cache.html
	const int link_count = nl_cache_nitems(link_cache);
	printf("Received link cache, %d items\n", link_count);
	/*
	struct nl_object *item = nl_cache_get_first(link_cache);
	while(item != nullptr) {
		item = nl_cache_get_next(item);
	}
	*/
	for(int i = 0; i < nl_cache_nitems(link_cache); ++i) {
		const rtnl_link *link = rtnl_link_get(link_cache, i);
		printf("Link %d\n", i);
		if (link == NULL) {
			printf("Error during link cache iteration\n");
			break;
		}
	}

	// Allocate address cache and fill in all configured addresses.
	// http://charette.no-ip.com:81/programming/doxygen/netfilter/group__rtaddr.html
	// https://www.infradead.org/~tgr/libnl/doc/api/group__rtaddr.html, https://www.infradead.org/~tgr/libnl/doc/core.html#_allocation_of_caches
    struct nl_cache *addr_cache;
	err = rtnl_addr_alloc_cache(sock_ptr, &addr_cache);
	if (err) {
		printf("rtnl_addr_alloc_cache error: %s\n", nl_geterror(err));
		exit(err);
	}

	const int addr_count = nl_cache_nitems(addr_cache);
	printf("Received addr cache, %d items\n", addr_count);
    nl_object *obj_ptr = nl_cache_get_first(addr_cache); 
	for(int i=0; i<addr_count; ++i) {
		if (!obj_ptr) {
			printf("Error during addr cache iteration\n");
		}
		struct rtnl_addr * addr_ptr = (rtnl_addr *)obj_ptr;
    	const struct nl_addr *p_nl_addr_local = rtnl_addr_get_local(addr_ptr);
		if (!p_nl_addr_local) {
			printf("rtnl_addr_get failed\n");
			return(EXIT_FAILURE);
		}

		int cur_ifindex = rtnl_addr_get_ifindex(addr_ptr);
		printf("This is index %d\n",cur_ifindex);

	    char addr_str[ADDR_STR_BUF_SIZE];
		const char *addr_s = nl_addr2str(p_nl_addr_local, addr_str, sizeof(addr_str));
		if (!addr_s) {
			printf("nl_addr2str failed\n");
			return(EXIT_FAILURE);
		}
		printf("\naddr is: %s\n", addr_s);
		
		obj_ptr = nl_cache_get_next(obj_ptr);
	}
	


	nl_socket_free(sock_ptr);

}
