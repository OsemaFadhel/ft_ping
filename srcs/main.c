#include "../includes/ping.h"

int ping_loop = 1;
int g_ping_count = 0;
int g_pckt_recvd = 0;
int g_ping_interval = 1;

void intHandler(int sig)
{
	(void)sig;
	ping_loop = 0;
}

int	main(int ac, char **av)
{
	int sockfd;
	struct sockaddr_in addr_con;
	t_pars parsed;
	//int addrlen = sizeof(addr_con);

	if (ac < 2)
	{
		fprintf(stderr, "ft_ping: usage error: Destination address required\n");
		return (1);
	}

	
	parse_args(ac--, av++, &parsed);

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0)
	{
		perror("Socket creation failed");
		return 1;
	}

	if (parsed.flags.flag_v) {
	    printf("ft_ping: sock4.fd: %d (socktype: SOCK_RAW), hints.ai_family: AF_INET\n\n", sockfd);
	}

	addr_con.sin_addr.s_addr = get_ip(parsed.target, &parsed.flags);

	/*
	reverse_hostname = reverse_dns_lookup(ip_addr);
	printf("\nTrying to connect to '%s' IP: %s\n", av[1], ip_addr);
	printf("\nReverse Lookup domain: %s\n", reverse_hostname);
	*/


	signal(SIGINT, intHandler);

	start_loop(sockfd, &addr_con);
	
	return 0;
}
