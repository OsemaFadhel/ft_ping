#include "ping.h"

void send_ping(int sockfd, struct sockaddr_in *addr_con, char *reverse_hostname, char *ip_addr, char *hostname)
{
	int pingloop = 1;
	int ping_count = 0;
	int ping_interval = 1;
	int ping_timeout = 1;
	int ping_size = 64;
	int ping_ttl = 64;

	char *ping_ip = ip_addr;
	char *ping_hostname = hostname;

	while (pingloop) {
		// Send ICMP echo request
		// ...
		// Handle response
		// ...
		sleep(ping_interval);
		ping_count++;
		if (ping_count >= g_ping_count) {
			pingloop = 0;
		}
	}
}
