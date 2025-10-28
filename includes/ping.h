#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
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

/*
int ping_loop = 1;
int g_ping_count = 0;
int g_ping_interval = 1;
int g_ping_timeout = 1;
int g_ping_size = 64;
int g_ping_ttl = 64;*/

typedef struct s_flags
{
	bool flag_f;
	bool flag_n;
	bool flag_r;
	bool flag_T;
	bool flag_v;
	bool flag_help;

	// flags with arguments
	int  preload;       // -l
	int  deadline;      // -w
	int  timeout;       // -W
	int  size;          // -s
	int  ttl;           // --ttl
	int  ip_timestamp;  // --ip-timestamp
	char *pattern;      // -p
}   t_flags;

typedef struct s_pars
{
	char *target; //destination host
	t_flags flags; //flags
}	t_pars;


void parse_args(int ac, char **av, t_pars *p);

uint32_t get_ip(char *target);
uint32_t dns_lookup(char *target);

void send_ping(int sockfd, struct sockaddr_in *addrcon, char *reverse_hostname, char *ip_addr, char *hostname);


//flags

void print_usage();