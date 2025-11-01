#include "../includes/ping.h"
#include <getopt.h>

t_flags defaults = {
	/*.flag_f = false,
	.flag_n = false,
	.flag_r = false,
	.flag_T = false,*/
	.flag_v = false,
	/*.preload = 0,
	.deadline = 0,
	.timeout = 1,
	.size = 56,
	.ttl = 64,
	.ip_timestamp = 0,
	.pattern = NULL*/
};

void get_flags(int ac, char **av, t_flags *flags)
{
	int opt;

	// Reset getopt
	optind = 1;

	while ((opt = getopt(ac, av, "v?")) != -1)
	{
		switch (opt)
		{
			case 'v':
				flags->flag_v = true;
				break;
			case '?':
				print_usage();
				exit(0);
				break;
			default:
				fprintf(stderr, "ft_ping: invalid option -- '%c'\n", opt);
				exit(1);
		}
	}
}

/*
* parse_args: Parses command line arguments for ft_ping
*
* NOTE: Real ping behavior with multiple hostnames:
*   - Real ping accepts multiple hostnames and pings the LAST one
*   - Example: `ping google.com yahoo.com` will ping yahoo.com but stays stuck in loop with packet loss
*
* Here:
*   - Only accept ONE hostname/IP address
*   - Multiple targets will result in an error
*/
void parse_args(int ac, char **av, t_pars *p)
{
	// Initialize
	p->flags = defaults;
	p->target = NULL;

	// Parse flags (handles -v and -?)
	get_flags(ac, av, &p->flags);

	// Extract target (the remaining non-option argument)
	if (optind < ac)
	{
		p->target = av[optind];

		// Check for multiple hostnames (strict mode)
		if (optind + 1 < ac)
		{
			fprintf(stderr, "ft_ping: too many arguments\n");
			fprintf(stderr, "Try 'ft_ping -?' for more information.\n");
			exit(1);
		}

		//printf("Target set to: %s\n", p->target);
	}
	else
	{
		fprintf(stderr, "ft_ping: missing host operand\n");
		fprintf(stderr, "Try 'ft_ping -?' for more information.\n");
		exit(1);
	}

	/*
	static struct option long_options[] = {
		{"ttl",          required_argument, 0, 0 },
		{"ip-timestamp", required_argument, 0, 0 },
		{0,              0,                 0, 0 }
	};

	int c;
	int option_index = 0;

	while ((c = getopt_long(ac, av, "fnrv?l:T:w:W:s:p:",
							long_options, &option_index)) != -1)
	{
		switch (c) {
			case 0:
				// Handle long options
				if (strcmp(long_options[option_index].name, "ttl") == 0)
					p->flags.ttl = atoi(optarg);
				else if (strcmp(long_options[option_index].name, "ip-timestamp") == 0)
					p->flags.ip_timestamp = atoi(optarg);
				break;
			case 'f':
				p->flags.flag_f = true;
				break;
			case 'n':
				p->flags.flag_n = true;
				break;
			case 'r':
				p->flags.flag_r = true;
				break;
			case 'T':
				p->flags.flag_T = true;
				break;
			case 'l':
				p->flags.preload = atoi(optarg);
				break;
			case 'w':
				p->flags.deadline = atoi(optarg);
				break;
			case 'W':
				p->flags.timeout = atoi(optarg);
				break;
			case 's':
				p->flags.size = atoi(optarg);
				break;
			case 'p':
				p->flags.pattern = optarg;
				break;
		}
	}
	*/
}
