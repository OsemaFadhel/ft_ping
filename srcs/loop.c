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

void prepare_icmp_packet(t_icmp_packet *packet)
{
	/* Zero the packet buffer to ensure no garbage bytes */
	memset(packet, 0, sizeof(*packet));

	/* Fill ICMP header fields */
	packet->hdr.type = ICMP_ECHO;
	packet->hdr.code = 0;
	packet->hdr.un.echo.id = htons(getpid() & 0xFFFF);
	packet->hdr.un.echo.sequence = htons(g_ping_count);
	packet->hdr.checksum = 0; // 0 before calculating checksum
	memset(packet->msg, 0x42, sizeof(packet->msg)); // Fill message
	packet->hdr.checksum = checksum(packet, sizeof(*packet));
    g_ping_count++;
}

int recv_packet(int sockfd, struct sockaddr_in *addr_con, struct timeval *send_time, 
                t_flags *flags)
{
    char buffer[1024];
    socklen_t addr_len = sizeof(*addr_con);
    struct timeval recv_time;
    struct timeval timeout;
    fd_set readfds;
    int bytes_received;

    // Set timeout to 1 second (or your desired timeout)
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    // Loop until we get the right packet or timeout
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        // Wait for data with timeout
        int ready = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
        
        if (ready <= 0) {
            // Timeout or error
            return -1;
        }

        bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0,
            (struct sockaddr*)addr_con, &addr_len);

        if (bytes_received <= 0) {
            continue;  // Try again
        }

        gettimeofday(&recv_time, NULL);

        struct iphdr *ip_header = (struct iphdr *)buffer;
        struct icmphdr *icmp_header = (struct icmphdr *)(buffer + (ip_header->ihl * 4));

        // Check if it's an ICMP Echo Reply
        if (icmp_header->type != ICMP_ECHOREPLY) {
            continue;
        }

        // matching ID
        if (icmp_header->un.echo.id != htons(getpid() & 0xFFFF)) {
            // Not our packet, keep waiting
            continue;
        }

        // matching sequence number
        if (ntohs(icmp_header->un.echo.sequence) != g_ping_count - 1) {
            // Wrong sequence (could be delayed reply from previous iteration)
            // For strict behavior, you might want to ignore this
            continue;
        }

        // Valid reply! Calculate RTT and print
        double rtt = (recv_time.tv_sec - send_time->tv_sec) * 1000.0 +
            (recv_time.tv_usec - send_time->tv_usec) / 1000.0;

        g_pckt_recvd++;

        if (rtt_count == 0 || rtt < rtt_min) {
            rtt_min = rtt;
        }
        if (rtt_count == 0 || rtt > rtt_max) {
            rtt_max = rtt;
        }
        rtt_sum += rtt;
        rtt_sum_squares += (rtt * rtt);
        rtt_count++;

        if (flags->flag_v) {
            printf(
                "%d bytes from %s (%s): icmp_seq=%d ident=%d ttl=%d time=%.1f ms\n",
                bytes_received - (ip_header->ihl * 4),
                reverse_dns_lookup(addr_con->sin_addr.s_addr),
                inet_ntoa(addr_con->sin_addr),
                ntohs(icmp_header->un.echo.sequence),
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
                ntohs(icmp_header->un.echo.sequence),  // ← FIX: use received seq
                ip_header->ttl,
                rtt
            );
        }

        return bytes_received;
    }
}

void print_stats(t_pars *parsed)
{
	struct timeval end_time;
	gettimeofday(&end_time, NULL);

	// Calculate total time in milliseconds
	double total_time = (end_time.tv_sec - start_time.tv_sec) * 1000.0 +
					(end_time.tv_usec - start_time.tv_usec) / 1000.0;

	// Calculate packet loss percentage
	double packet_loss = 0.0;
	if (g_ping_count > 0) {
		packet_loss = ((g_ping_count - g_pckt_recvd) / (double)g_ping_count) * 100.0;
	}

	printf("\n--- %s ft_ping statistics ---\n", parsed->target);
	printf("%d packets transmitted, %d received, %d%% packet loss, time %.0fms\n",
		g_ping_count, g_pckt_recvd, (int)packet_loss, total_time);

	// Only print RTT stats if we received packets
	if (rtt_count > 0) {
		double rtt_avg = rtt_sum / rtt_count;

		// Calculate standard deviation (mdev) - same as real ping
		if (rtt_count > 1) {
			double variance = (rtt_sum_squares - (rtt_sum * rtt_sum / rtt_count)) / (rtt_count - 1);
			double rtt_mdev = (variance > 0) ? sqrt(variance) : 0.0;
			
			printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
				rtt_min, rtt_avg, rtt_max, rtt_mdev);
		} else {
			// Only one sample, no meaningful standard deviation
			printf("rtt min/avg/max = %.3f/%.3f/%.3f ms\n",
				rtt_min, rtt_avg, rtt_max);
		}
	}
}

void start_loop(int sockfd, struct sockaddr_in *addr_con, t_flags *flags, t_pars *parsed)
{
	t_icmp_packet packet;
	int packet_size = sizeof(packet);
	int bytes_received;
	struct timeval send_time;

	while (ping_loop) {
		prepare_icmp_packet(&packet);

		gettimeofday(&send_time, NULL);
		if (sendto(sockfd, &packet, packet_size, 0, (struct sockaddr*)addr_con, sizeof(*addr_con)) <= 0)
		{
			//perror("Packet Sending Failed");
			return;
		}

		bytes_received = recv_packet(sockfd, addr_con, &send_time, flags);

		if (bytes_received <= 0)
			printf("Request timeout\n");

		sleep(g_ping_interval);
	}

	print_stats(parsed);
}
