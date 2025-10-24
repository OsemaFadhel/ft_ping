#include "../includes/ping.h"
#include <getopt.h>

t_flags defaults = {
	.flag_f = false,
	.flag_n = false,
	.flag_r = false,
	.flag_T = false,
	.flag_v = false,
	.flag_help = false,
	.preload = 0,
	.deadline = 0,
	.timeout = 1,
	.size = 56,
	.ttl = 64,
	.ip_timestamp = 0,
	.pattern = NULL
};

void get_flags(char *arg, t_flags *flags)
{

}

void parse_args(int ac, char **av, t_pars *p)
{
	//TODO: GET TARGET

	//TODO: GET FLAGS

	/*
	int c;
	int digit_optind = 0;
	p->flags = defaults;
	p->target = NULL;


	while (1) {
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		static struct option long_options[] = {
			{"ttl",     required_argument, 0,  0 },
			{"ip-timestamp",     required_argument, 0,  0 },
			{"echo",     required_argument, 0,  0 },
			{"usage",     required_argument, 0,  0 },
			{0,         0,                 0,  0 }
		};

		c = getopt_long(argc, argv, "fnrv?l:T:w:W:s:p:",
						long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
			case 0:
				printf("option %s", long_options[option_index].name);
				if (optarg)
					printf(" with arg %s", optarg);
				printf("\n");
				break;

			case 'f':
				break;
			case 'n':
				break;
			case 'r':
				break;
			case 'v':
				printf("option a\n");
				break;

			case '?':
				printf("option b\n");
				break;

			case 'l':
				printf("option c with value '%s'\n", optarg);
				break;

			case 'T':
				printf("option d with value '%s'\n", optarg);
				break;

			case 'w':
				break;
			
			case 'W':
				break;

			case 's':
				break;
			
			case 'p':
				break;

			default:
				printf("?? getopt returned character code 0%o ??\n", c);
		}
	}
	*/
}