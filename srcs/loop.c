#include "../includes/ping.h"

void prepare_icmp_packet(t_icmp_packet *packet)
{
	/* Zero the packet buffer to ensure no garbage bytes */
	bzero(packet, sizeof(*packet));

	/* Fill ICMP header fields */
	packet->hdr.type = ICMP_ECHO;
	packet->hdr.code = 0;
	packet->hdr.un.echo.id = htons(getpid() & 0xFFFF);
	packet->hdr.un.echo.sequence = htons(g_ping_count++);

	/* Compute checksum over the full packet */
	packet->hdr.checksum = 0;
}

int recv_packet(int sockfd, struct sockaddr_in *addr_con)
{
	char buffer[1024];
	socklen_t addr_len = sizeof(*addr_con);

	int bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0,
	    (struct sockaddr*)addr_con, &addr_len);

	return bytes_received;
}

void start_loop(int sockfd, struct sockaddr_in *addr_con)
{
	t_icmp_packet packet;
	int packet_size = sizeof(packet);
	int bytes_received;

	while (ping_loop) {
		prepare_icmp_packet(&packet);

		if (sendto(sockfd, &packet, packet_size, 0, (struct sockaddr*)addr_con, sizeof(*addr_con)) <= 0) 
			perror("Packet Sending Failed");
		else 
			printf("Packet Sent to %s, size=%d bytes\n", inet_ntoa(addr_con->sin_addr), packet_size);
					
		bytes_received = recv_packet(sockfd, addr_con);
		if (bytes_received <= 0)
			printf("Packet receive failed\n");
		else 
		{
			g_pckt_recvd++;
			printf("Packet Received from %s, size=%d bytes\n",
		       inet_ntoa(addr_con->sin_addr), bytes_received);
		}

		sleep(g_ping_interval);
	}
}