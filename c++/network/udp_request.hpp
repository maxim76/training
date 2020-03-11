constexpr size_t MAX_DATAGRAM_SIZE = 1500;
constexpr size_t DEFAULT_SEND_ATTEMPTS = 2;
constexpr size_t DEFAULT_SEND_TIMEOUT = 2;
// Temporary solution: requests are stored in array instead of inordered map. Array size should be restricted
constexpr size_t MAX_REQUEST_COUNT = 1000;

class UDPRequest
{
	struct Request {
		//int req_id;
		int isActive;	// when active Requests be moved to unordered_map this field will not be required
		char data[MAX_DATAGRAM_SIZE];
		size_t len;
		size_t attemptNum;
		time_t deadline;
	};

public:
	UDPRequest( const char *host, int port, int localPort = 0);
	~UDPRequest();
	bool send( unsigned int  req_id, char *data, size_t len);

	void update();
private:
	bool recv( unsigned int  *req_id, char *data, size_t *len);

	//processRequest( unsigned int  req_id );
	int sockFD;
	int sendAttempts;
	int sendTimeout;
	Request *requests = nullptr;
};