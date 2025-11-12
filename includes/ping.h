#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/time.h>
#include <math.h>

extern int ping_loop;


typedef struct s_global_vars
{
	int g_ping_count;
	int g_ping_interval;
	int g_pckt_recvd;
	double rtt_min;
	double rtt_max;
	double rtt_sum;
	double rtt_sum_squares;
	int rtt_count;
	int error_count;
	struct timeval start_time;
} t_global_vars;

typedef struct s_icmp_packet
{
	struct icmphdr hdr;
	char msg[64 - sizeof(struct icmphdr)];
}	t_icmp_packet;

typedef struct s_flags
{
	bool flag_v;
	int ttl;
}   t_flags;

typedef struct s_pars
{
	char *target; //destination host
	t_flags flags; //flags
}	t_pars;


void parse_args(int ac, char **av, t_pars *p);

uint32_t get_ip(char *target, t_flags *flags);
uint32_t dns_lookup(char *target, t_flags *flags);
char *reverse_dns_lookup(uint32_t ip_addr);

void start_loop(int sockfd, struct sockaddr_in *addr_con, t_flags *flags, t_pars *parsed, t_global_vars *globals);

void print_usage();
