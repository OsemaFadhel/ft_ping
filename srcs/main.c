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
	
	signal(SIGINT, intHandler);

	start_loop(sockfd, &addr_con);
	
	return 0;
}
