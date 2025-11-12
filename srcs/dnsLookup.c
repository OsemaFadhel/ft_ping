#include "../includes/ping.h"


char *reverse_dns_lookup(uint32_t ip_addr)
{
	struct in_addr addr;
	struct hostent *host;

	addr.s_addr = ip_addr;
	host = gethostbyaddr((const void *)&addr, sizeof(addr), AF_INET);
	if (host == NULL) {
		return inet_ntoa(addr);
	}
	return host->h_name;
}

uint32_t dns_lookup(char *target, t_flags *flags)
{
	struct addrinfo hints, *result;
	struct sockaddr_in *addr;
	uint32_t ip;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;    

	hints.ai_socktype = SOCK_RAW;    

	hints.ai_protocol = IPPROTO_ICMP; 

	
	int status = getaddrinfo(target, NULL, &hints, &result);
	if (status != 0) {
		fprintf(stderr, "ft_ping: %s: %s\n", target, gai_strerror(status));
		return 0;
	}

	if (flags->flag_v) {
		printf("ai->ai_family: AF_INET, ai->ai_canonname: '%s'\n",
			result->ai_canonname ? result->ai_canonname : target);
	}

	addr = (struct sockaddr_in *)result->ai_addr;

	ip = addr->sin_addr.s_addr;

	freeaddrinfo(result);

	return ip;
}

uint32_t get_ip(char *target, t_flags *flags)
{
	return dns_lookup(target, flags);
}
