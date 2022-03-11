#include <zmq.h>
#include <iostream>
 
int main()
{
	int major = 0;
	int minor = 0;
	int patch = 0;
	zmq_version( &major, &minor, &patch );
	std::wcout << "Current 0MQ version is " << major << '.' << minor << '.' << patch << '\n';
}