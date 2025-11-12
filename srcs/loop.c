#include "../includes/ping.h"

//standard icmp checksum algo
u_int16_t checksum(void *b, int len)
{
	uint16_t *buf = b;
	uint32_t sum = 0;
	uint16_t result;

	// Add all 16-bit words
	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;

	// Add left-over byte if odd length
	if (len == 1)
		sum += *(unsigned char *)buf;

	// Fold 32-bit sum to 16 bits
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);

	// Take one's complement
	result = ~sum;

	return result;
}

void prepare_icmp_packet(t_icmp_packet *packet, t_global_vars *globals)
{
	memset(packet, 0, sizeof(*packet));

	packet->hdr.type = ICMP_ECHO;
	packet->hdr.code = 0;
	packet->hdr.un.echo.id = htons(getpid() & 0xFFFF);
	packet->hdr.un.echo.sequence = htons(globals->g_ping_count);
	packet->hdr.checksum = 0; 
	memset(packet->msg, 0x42, sizeof(packet->msg));
	packet->hdr.checksum = checksum(packet, sizeof(*packet));
    globals->g_ping_count++;
}

int recv_packet(int sockfd, struct sockaddr_in *addr_con, struct timeval *send_time, 
                t_flags *flags, t_global_vars *globals)
{
    char buffer[1024];
    socklen_t addr_len = sizeof(*addr_con);
    struct timeval recv_time;
    struct timeval timeout;
    fd_set readfds;
    int bytes_received;

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        int ready = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
        
        // Timeout or error
        if (ready <= 0) {
			if (flags->flag_v) {
				printf("Request timeout\n");
			}
            return -1;
        }

        bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0,
            (struct sockaddr*)addr_con, &addr_len);

        if (bytes_received <= 0) {
            continue;
        }

        gettimeofday(&recv_time, NULL);

        struct iphdr *ip_header = (struct iphdr *)buffer;
        struct icmphdr *icmp_header = (struct icmphdr *)(buffer + (ip_header->ihl * 4));

		// Check if it's an ICMP Echo Reply
        if (icmp_header->type == ICMP_ECHOREPLY) {     
			// matching ID
			if (icmp_header->un.echo.id != htons(getpid() & 0xFFFF)) {
				continue;
			}

			// matching sequence number
			if (ntohs(icmp_header->un.echo.sequence) != globals->g_ping_count - 1) {
				continue;
			}

			//round trip time
			double rtt = (recv_time.tv_sec - send_time->tv_sec) * 1000.0 +
				(recv_time.tv_usec - send_time->tv_usec) / 1000.0;

			globals->g_pckt_recvd++;

			if (globals->rtt_count == 0 || rtt < globals->rtt_min) {
				globals->rtt_min = rtt;
			}
			if (globals->rtt_count == 0 || rtt > globals->rtt_max) {
				globals->rtt_max = rtt;
			}
			globals->rtt_sum += rtt;
			globals->rtt_sum_squares += (rtt * rtt);
			globals->rtt_count++;

			if (flags->flag_v) {
				printf(
					"%d bytes from %s (%s): icmp_seq=%d ident=%d ttl=%d time=%.1f ms\n",
					bytes_received - (ip_header->ihl * 4),
					reverse_dns_lookup(addr_con->sin_addr.s_addr),
					inet_ntoa(addr_con->sin_addr),
					ntohs(icmp_header->un.echo.sequence) + 1,
					ntohs(icmp_header->un.echo.id),
					ip_header->ttl,
					rtt
				);
			}
			else {
				printf(
					"%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%.1f ms\n",
					bytes_received - (ip_header->ihl * 4),
					reverse_dns_lookup(addr_con->sin_addr.s_addr),
					inet_ntoa(addr_con->sin_addr),
					ntohs(icmp_header->un.echo.sequence) + 1, 
					ip_header->ttl,
					rtt
				);
			}
			return bytes_received;
		}

		else if (icmp_header->type == ICMP_TIME_EXCEEDED) {		
			struct iphdr *orig_ip = (struct iphdr *)((char *)icmp_header + 8);
			struct icmphdr *orig_icmp = (struct icmphdr *)((char *)orig_ip + (orig_ip->ihl * 4));
			
			if (orig_icmp->un.echo.id == htons(getpid() & 0xFFFF)) {
				printf("From %s (%s): icmp_seq=%d Time to live exceeded\n",
					reverse_dns_lookup(addr_con->sin_addr.s_addr),
					inet_ntoa(addr_con->sin_addr),
					ntohs(orig_icmp->un.echo.sequence) + 1
					);
				}		
			globals->error_count++;
			return -1; 
		}

		else if (icmp_header->type == ICMP_DEST_UNREACH) {
			struct iphdr *orig_ip = (struct iphdr *)((char *)icmp_header + 8);
			struct icmphdr *orig_icmp = (struct icmphdr *)((char *)orig_ip + (orig_ip->ihl * 4));

			if (orig_icmp->un.echo.id == htons(getpid() & 0xFFFF)) {
				printf("From %s (%s): icmp_seq=%d Destination Unreachable\n",
					reverse_dns_lookup(addr_con->sin_addr.s_addr),
					inet_ntoa(addr_con->sin_addr),
					ntohs(orig_icmp->un.echo.sequence) + 1
					);
			}
			globals->error_count++;
			return -1;
		}

        continue;
    }
}

void print_stats(t_pars *parsed, t_global_vars *globals)
{
	struct timeval end_time;
	gettimeofday(&end_time, NULL);

	// Calculate total time in milliseconds
	double total_time = (end_time.tv_sec - globals->start_time.tv_sec) * 1000.0 +
					(end_time.tv_usec - globals->start_time.tv_usec) / 1000.0;

	// Calculate packet loss percentage
	double packet_loss = 0.0;
	if (globals->g_ping_count > 0) {
		packet_loss = ((globals->g_ping_count - globals->g_pckt_recvd) / (double)globals->g_ping_count) * 100.0;
	}

	printf("\n--- %s ft_ping statistics ---\n", parsed->target);
	printf("%d packets transmitted, %d received, ",
		globals->g_ping_count, globals->g_pckt_recvd);
	
	if (globals->error_count > 0) {
		printf("+%d errors, ", globals->error_count); 
	}

	printf("%d%% packet loss, time %.0fms\n", (int)packet_loss, total_time);

	if (globals->rtt_count > 0) {
		double rtt_avg = globals->rtt_sum / globals->rtt_count;

		// Calculate standard deviation (mdev)
		if (globals->rtt_count > 1) {
			double variance = (globals->rtt_sum_squares - (globals->rtt_sum * globals->rtt_sum / globals->rtt_count)) / (globals->rtt_count - 1);
			double rtt_mdev = (variance > 0) ? sqrt(variance) : 0.0;
			
			printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
				globals->rtt_min, rtt_avg, globals->rtt_max, rtt_mdev);
		} else {
			printf("rtt min/avg/max = %.3f/%.3f/%.3f ms\n",
				globals->rtt_min, rtt_avg, globals->rtt_max);
		}
	}
}

void start_loop(int sockfd, struct sockaddr_in *addr_con, t_flags *flags, t_pars *parsed, t_global_vars *globals)
{
	t_icmp_packet packet;
	int packet_size = sizeof(packet);
	int bytes_received;
	struct timeval send_time;
	struct sockaddr_in dest_addr = *addr_con;
	struct sockaddr_in reply_addr; 

	while (ping_loop) {
		prepare_icmp_packet(&packet, globals);

		gettimeofday(&send_time, NULL);
		if (sendto(sockfd, &packet, packet_size, 0, (struct sockaddr*)addr_con, sizeof(*addr_con)) <= 0)
		{
			perror("sendto");
			return;
		}

		reply_addr = dest_addr;
		bytes_received = recv_packet(sockfd, &reply_addr, &send_time, flags, globals);

		if (bytes_received <= 0 && flags->flag_v)
			printf("No reply received for icmp_seq=%d\n", globals->g_ping_count - 1);

		sleep(globals->g_ping_interval);
	}

	print_stats(parsed, globals);
}
