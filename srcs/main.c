#include "../includes/ping.h"

/*
void intHandler(int sig)
{
	(void)sig;
	ping_loop = 0;
}*/

int	main(int ac, char **av)
{
	//int sockfd;
	struct sockaddr_in addr_con;
	t_pars parsed;
	//int addrlen = sizeof(addr_con);

	if (ac < 2)
	{
		fprintf(stderr, "ft_ping: usage error: Destination address required\n");
		return (1);
	}

	
	parse_args(ac--, av++, &parsed);

	//addr_con.sin_addr.s_addr = get_ip(parsed.target);

	(void)addr_con;

	/*if (ip_addr == NULL) {
		write(2,"\nDNS lookup failed! Could not resolve hostname!\n", 48);
		return 0;
	}

	reverse_hostname = reverse_dns_lookup(ip_addr);
	printf("\nTrying to connect to '%s' IP: %s\n", av[1], ip_addr);
	printf("\nReverse Lookup domain: %s\n", reverse_hostname);

	signal(SIGINT, intHandler);

	send_ping(sockfd, &addr_con, reverse_hostname, ip_addr, av[1]);
	*/

	return 0;
}
