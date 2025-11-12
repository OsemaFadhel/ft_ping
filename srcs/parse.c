#include "../includes/ping.h"
#include <getopt.h>

t_flags defaults = {
	.flag_v = false,
	.ttl = 1
};

void get_flags(int ac, char **av, t_flags *flags)
{
	int opt;

	optind = 1;

	while ((opt = getopt(ac, av, "vh?")) != -1)
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
			case 'h':
				print_usage();
				exit(0);
				break;
			default:
				fprintf(stderr, "ft_ping: invalid option -- '%c'\n", opt);
				exit(1);
		}
	}
}

void parse_args(int ac, char **av, t_pars *p)
{
	p->flags = defaults;
	p->target = NULL;

	get_flags(ac, av, &p->flags);

	if (optind < ac)
	{
		p->target = av[optind];

		if (optind + 1 < ac)
		{
			fprintf(stderr, "ft_ping: too many arguments\n");
			fprintf(stderr, "Try 'ft_ping -?' for more information.\n");
			exit(1);
		}
	}
	else
	{
		fprintf(stderr, "ft_ping: missing host operand\n");
		fprintf(stderr, "Try 'ft_ping -?' for more information.\n");
		exit(1);
	}
}
