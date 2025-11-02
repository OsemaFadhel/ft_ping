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
extern int g_ping_count;
extern int g_ping_interval;
extern int g_pckt_recvd;
extern double rtt_min;
extern double rtt_max;
extern double rtt_sum;
extern double rtt_sum_squares;
extern int rtt_count;
extern struct timeval start_time;

typedef struct s_icmp_packet
{
	struct icmphdr hdr;
	char msg[64 - sizeof(struct icmphdr)];
}	t_icmp_packet;

typedef struct s_flags
{
	/*bool flag_f;
	bool flag_n;
	bool flag_r;
	bool flag_T;*/
	bool flag_v;

	/*
	// flags with arguments
	int  preload;       // -l
	int  deadline;      // -w
	int  timeout;       // -W
	int  size;          // -s
	int  ttl;           // --ttl
	int  ip_timestamp;  // --ip-timestamp
	char *pattern;      // -p*/
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

void start_loop(int sockfd, struct sockaddr_in *addr_con, t_flags *flags, t_pars *parsed);


//flags

void print_usage();
