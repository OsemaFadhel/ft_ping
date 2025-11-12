#include "../includes/ping.h"

int ping_loop = 1;
int g_ping_count = 0;
int g_pckt_recvd = 0;
int g_ping_interval = 1;
double rtt_min = 0.0;
double rtt_max = 0.0;
double rtt_sum = 0.0;
double rtt_sum_squares = 0.0;
int rtt_count = 0;
struct timeval start_time;

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

	if (ac < 2)
	{
		fprintf(stderr, "ft_ping: usage error: Destination address required\n");
		return (1);
	}


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

	gettimeofday(&start_time, NULL);

	start_loop(sockfd, &addr_con, &parsed.flags, &parsed);

	return 0;
}
