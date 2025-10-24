#include "../includes/ping.h"

uint32_t dns_lookup(char *target)
{
	(void)target;
	return 0;
}

uint32_t get_ip(char *target)
{
	struct in_addr ipv4;

	if (inet_pton(AF_INET, target, &ipv4) == 1)
		return ipv4.s_addr;

	return dns_lookup(target);
}