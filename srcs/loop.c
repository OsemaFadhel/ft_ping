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
	packet->hdr.un.echo.sequence = htons(g_ping_count++);
	packet->hdr.checksum = 0; // 0 before calculating checksum
	memset(packet->msg, 0x42, sizeof(packet->msg)); // Fill message
	packet->hdr.checksum = checksum(packet, sizeof(*packet));
}

int recv_packet(int sockfd, struct sockaddr_in *addr_con, struct timeval *send_time)
{
	char buffer[1024];
	socklen_t addr_len = sizeof(*addr_con);
	struct timeval recv_time;

	int bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0,
		(struct sockaddr*)addr_con, &addr_len);

	gettimeofday(&recv_time, NULL);  // Set recv_time RIGHT AFTER receiving

	double time_ms = (recv_time.tv_sec - send_time->tv_sec) * 1000.0 +
		(recv_time.tv_usec - send_time->tv_usec) / 1000.0;

	// validating recvd packet
	if (bytes_received > 0) {
		struct iphdr *ip_header = (struct iphdr *)buffer;
		struct icmphdr *icmp_header = (struct icmphdr *)(buffer + (ip_header->ihl * 4));

		// Check if it's an ICMP Echo Reply
		if (icmp_header->type != ICMP_ECHOREPLY) {
			//printf("Received packet is not an ICMP Echo Reply\n");
			return -1;
		}

		// matching ID
		if (icmp_header->un.echo.id != htons(getpid() & 0xFFFF)) {
			//printf("Received packet ID does not match\n");
			return -1;
		}

		// matching sequence number
		if (icmp_header->un.echo.sequence != htons(g_ping_count - 1)) {
			//printf("Received packet sequence number does not match\n");
			return -1;
		}

		g_pckt_recvd++;
		printf(
			"%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%.1f ms\n",
			bytes_received,
			reverse_dns_lookup(addr_con->sin_addr.s_addr),
			inet_ntoa(addr_con->sin_addr),
			ntohs(icmp_header->un.echo.sequence),
			ip_header->ttl,
			time_ms
		);
	}
	return bytes_received;
}

void start_loop(int sockfd, struct sockaddr_in *addr_con)
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

		bytes_received = recv_packet(sockfd, addr_con, &send_time);

		if (bytes_received <= 0)
			printf("Request timeout\n");

		sleep(g_ping_interval);
	}
}
