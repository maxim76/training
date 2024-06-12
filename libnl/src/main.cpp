#include <stdio.h>
#include <netlink/socket.h>
#include <libnl3/netlink/route/link.h>
#include <libnl3/netlink/route/addr.h>

#define ADDR_STR_BUF_SIZE 80

void print_link_info(struct rtnl_link * addr_ptr);
void print_addr_info(struct rtnl_addr * addr_ptr);

const char *get_family_descr(int family) {
	switch (family) {
		case AF_INET: return "IPv4"; break;
		case AF_INET6: return "IPv6"; break;
		case AF_LLC: return "LLC"; break;
		case AF_UNSPEC: return "UNSPEC"; break;
		default: return "Error";
	}
}


void link_cache_callback(struct nl_object *obj_ptr, void *data) {
	// data param is not used by this callback
	print_link_info((rtnl_link *)obj_ptr);
}


void addr_cache_callback(struct nl_object *obj_ptr, void *data) {
	// data param is not used by this callback
	print_addr_info((rtnl_addr *)obj_ptr);
}


void print_link_info(struct rtnl_link * link_ptr) {
	const int if_index = rtnl_link_get_ifindex(link_ptr);
	if (if_index == 0) {
		printf("print_link_info: rtnl_link_get_ifindex() error\n");
		return;
	}

	const char * if_name = rtnl_link_get_name(link_ptr);

	struct nl_addr * l2_nl_addr_ptr = rtnl_link_get_addr(link_ptr);
	char addr_str[ADDR_STR_BUF_SIZE];
	const char *addr_s = nl_addr2str(l2_nl_addr_ptr, addr_str, sizeof(addr_str));
	if (!addr_s) {
		printf("print_link_info: nl_addr2str() failed\n");
		return;
	}

	const int family = rtnl_link_get_family(link_ptr);  // Another way to get family is nl_addr_guess_family(struct nl_addr *addr)

	printf("Index: %2d, family: %s, addr: %s name: %s\n", if_index, get_family_descr(family), addr_s, if_name);
}


void print_addr_info(struct rtnl_addr * addr_ptr) {
	const int if_index = rtnl_addr_get_ifindex(addr_ptr);
	if (if_index == 0) {
		printf("print_addr_info: rtnl_link_get_ifindex() error\n");
		return;
	}

	const struct nl_addr *nl_addr_ptr = rtnl_addr_get_local(addr_ptr);
	if (!nl_addr_ptr) {
		printf("print_addr_info: rtnl_addr_get_local() failed\n");
		return;
	}

	char addr_str[ADDR_STR_BUF_SIZE];
	const char *addr_s = nl_addr2str(nl_addr_ptr, addr_str, sizeof(addr_str));
	if (!addr_s) {
		printf("print_addr_info: nl_addr2str() failed\n");
		return;
	}

	const int family = rtnl_addr_get_family(addr_ptr);  // Another way to get family is nl_addr_guess_family(struct nl_addr *addr)

	printf("Index: %2d, family: %s, addr: %s\n", if_index, get_family_descr(family), addr_s);
}


int main() {
	int err;
	
	// Allocate new netlink socket
	// https://www.infradead.org/~tgr/libnl/doc/api/group__socket.html#gaf9458e6b88324962065f93a13d53cf2c
	struct nl_sock *sock_ptr = nl_socket_alloc();
	if (sock_ptr == nullptr) {
		printf("nl_socket_alloc() failed\n");
		exit(EXIT_FAILURE);
	}

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


	// Allocate address cache and fill in all configured addresses.
	// http://charette.no-ip.com:81/programming/doxygen/netfilter/group__rtaddr.html
	// https://www.infradead.org/~tgr/libnl/doc/api/group__rtaddr.html, https://www.infradead.org/~tgr/libnl/doc/core.html#_allocation_of_caches
    struct nl_cache *addr_cache;
	err = rtnl_addr_alloc_cache(sock_ptr, &addr_cache);
	if (err) {
		printf("rtnl_addr_alloc_cache error: %s\n", nl_geterror(err));
		exit(err);
	}

	// Print link cache
	printf("Link cache content:\n");
	nl_cache_foreach(link_cache, link_cache_callback, nullptr);

	// Another (manual) way of iterating over cache
	/*
	const int link_count = nl_cache_nitems(link_cache);
	obj_ptr = nl_cache_get_first(link_cache); 
	for(int i=0; i < link_count; ++i) {
		if (!obj_ptr) {
			printf("Error during link cache iteration\n");
			break;
		}
		struct rtnl_link * link_ptr = (rtnl_link *)obj_ptr;

		const int if_index = rtnl_link_get_ifindex(link_ptr);
		if (if_index == 0) {
			printf("rtnl_link_get_ifindex() error\n");
			break;
		}

		const char * if_name = rtnl_link_get_name(link_ptr);

		struct nl_addr * l2_nl_addr_ptr = rtnl_link_get_addr(link_ptr);
		const char *addr_s = nl_addr2str(l2_nl_addr_ptr, addr_str, sizeof(addr_str));
		if (!addr_s) {
			printf("nl_addr2str() failed\n");
			continue;
		}

		const int family = rtnl_link_get_family(link_ptr);  // Another way to get family is nl_addr_guess_family(struct nl_addr *addr)

		printf("Index %02d, family: %s, addr %s name %s\n", if_index, get_family_descr(family), addr_s, if_name);

		obj_ptr = nl_cache_get_next(obj_ptr);
	}
	*/

	// Print addr cache
	printf("\nAddr cache content:\n");
	nl_cache_foreach(addr_cache, addr_cache_callback, nullptr);

	nl_socket_free(sock_ptr);

}
