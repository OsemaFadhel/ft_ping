#include "../includes/ping.h"

uint32_t dns_lookup(char *target)
{
	struct addrinfo hints, *result;
	struct sockaddr_in *addr;
	uint32_t ip;


	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;      // IPv4, for both ipv4/ipv6 use af_unspec
	hints.ai_socktype = SOCK_RAW;   //raw for icmo
	hints.ai_protocol = IPPROTO_ICMP;

	int status = getaddrinfo(target, NULL, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "ft_ping: -: Name or service not known\n");
        return 0;
    }

	addr = (struct sockaddr_in *)result->ai_addr;
	ip = addr->sin_addr.s_addr;

	printf("ip address resolved: %u", ip);
	freeaddrinfo(result);
	
	return ip;
}

uint32_t get_ip(char *target)
{
	struct in_addr ipv4;

	if (inet_pton(AF_INET, target, &ipv4) == 1)
		return ipv4.s_addr;

	return dns_lookup(target);
}