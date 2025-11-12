#include "../includes/ping.h"

int ping_loop = 1;

void intHandler(int sig)
{
	(void)sig;
	ping_loop = 0;
}

void init_globals(t_global_vars *globals)
{
	globals->g_ping_count = 0;
	globals->g_ping_interval = 1;
	globals->g_pckt_recvd = 0;
	globals->rtt_min = 0.0;
	globals->rtt_max = 0.0;
	globals->rtt_sum = 0.0;
	globals->rtt_sum_squares = 0.0;
	globals->rtt_count = 0;
	globals->error_count = 0;
}

int	main(int ac, char **av)
{
	int sockfd;
	struct sockaddr_in addr_con;
	t_global_vars globals;
	t_pars parsed;

	if (ac < 2)
	{
		fprintf(stderr, "ft_ping: usage error: Destination address required\n");
		return (1);
	}

	init_globals(&globals);
	parse_args(ac--, av++, &parsed);

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0)
	{
		perror("Need to run as root user, because it uses raw sockets");
		return 1;
	}

	if (parsed.flags.flag_v) {
		printf("ft_ping: sock4.fd: %d (socktype: SOCK_RAW), hints.ai_family: AF_INET\n\n", sockfd);
	}

	if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &parsed.flags.ttl, sizeof(parsed.flags.ttl)) != 0) {
		perror("setsockopt IP_TTL failed");
		close(sockfd);
		exit(1);
	}

	memset(&addr_con, 0, sizeof(addr_con));
	addr_con.sin_family = AF_INET;
	addr_con.sin_addr.s_addr = get_ip(parsed.target, &parsed.flags);
	if (addr_con.sin_addr.s_addr == 0)
		return 1;

	printf("FT_PING %s (%s) %d(%d) bytes of data.\n",
	       parsed.target,
	       inet_ntoa(addr_con.sin_addr),
	       56,  // payload size: sizeof(packet.msg)
	       84); // total: 20 (IP) + 8 (ICMP) + 56 (data)

	signal(SIGINT, intHandler);

	gettimeofday(&globals.start_time, NULL);

	start_loop(sockfd, &addr_con, &parsed.flags, &parsed, &globals);

	return 0;
}
