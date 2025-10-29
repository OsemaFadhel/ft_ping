#include "../includes/ping.h"

/*
* dns_lookup: Resolves a hostname to an IPv4 address using DNS
* 
* PARAMETERS:
*   - target: hostname to resolve (e.g., "google.com", "example.org")
* 
* RETURN:
*   - IPv4 address in network byte order (uint32_t)
*   - 0 on failure
* 
* HOW IT WORKS:
*   1. Sets up hints to specify what kind of address we want
*   2. Calls getaddrinfo() to perform DNS lookup
*   3. Extracts IP address from the result
*   4. Frees allocated memory
*/
uint32_t dns_lookup(char *target, t_flags *flags)
{
	struct addrinfo hints, *result;
	struct sockaddr_in *addr;
	uint32_t ip;

	/*
	 * struct addrinfo - Used to provide hints to getaddrinfo() and receive results
	 * 
	 * Key fields:
	 *   - ai_family: Address family (AF_INET = IPv4, AF_INET6 = IPv6, AF_UNSPEC = any)
	 *   - ai_socktype: Socket type (SOCK_STREAM = TCP, SOCK_DGRAM = UDP, SOCK_RAW = raw)
	 *   - ai_protocol: Protocol (IPPROTO_TCP, IPPROTO_UDP, IPPROTO_ICMP, etc.)
	 *   - ai_addr: Pointer to socket address structure (contains the actual IP)
	 *   - ai_next: Pointer to next result (getaddrinfo returns a linked list)
	 */

	// Zero out hints structure to avoid garbage values
	memset(&hints, 0, sizeof(hints));
	
	hints.ai_family = AF_INET;        // AF_INET = IPv4 only
	                                  // AF_INET6 = IPv6 only
	                                  // AF_UNSPEC = accept both IPv4 and IPv6
	
	hints.ai_socktype = SOCK_RAW;     // SOCK_RAW = raw socket (needed for ICMP)
	                                  // Allows us to construct our own packets
	
	hints.ai_protocol = IPPROTO_ICMP; // IPPROTO_ICMP = Internet Control Message Protocol
	                                  // The protocol used by ping

	/*
	 * getaddrinfo() - Modern DNS resolution function (thread-safe, IPv4/IPv6)
	 * 
	 * PROTOTYPE:
	 *   int getaddrinfo(const char *node,           // hostname or IP string
	 *                   const char *service,        // service name or port (NULL here)
	 *                   const struct addrinfo *hints, // input specifications
	 *                   struct addrinfo **res);     // output: linked list of results
	 * 
	 * RETURN VALUES:
	 *   - 0 on success
	 *   - Non-zero error code on failure (use gai_strerror() to get error string)
	 * 
	 * EXAMPLES:
	 *   Input: "google.com"  → Returns list with IP like 142.250.185.46
	 *   Input: "8.8.8.8"     → Returns same IP (validates and converts)
	 *   Input: "localhost"   → Returns 127.0.0.1
	 *   Input: "asdfgh"      → Returns error (EAI_NONAME = -2)
	 *   Input: "-"           → Returns error (EAI_NONAME = -2)
	 * 
	 * RESULT STRUCTURE:
	 *   getaddrinfo() allocates memory and returns a linked list of addrinfo structs.
	 *   Each node contains one possible address for the hostname.
	 *   For "google.com", you might get multiple IPs (load balancing).
	 *   We take the first result (result->ai_addr).
	 */
	int status = getaddrinfo(target, NULL, &hints, &result);
	if (status != 0) {
		// gai_strerror() converts error code to human-readable string
		// Examples: "Name or service not known", "Temporary failure"
		fprintf(stderr, "ft_ping: %s: %s\n", target, gai_strerror(status));
		return 0;
	}

	if (flags->flag_v) {
	    printf("ai->ai_family: AF_INET, ai->ai_canonname: '%s'\n",
	           result->ai_canonname ? result->ai_canonname : target);
	}

	/*
	 * Extract IP address from result
	 * 
	 * result->ai_addr is a generic socket address (struct sockaddr*)
	 * We cast it to sockaddr_in (IPv4 specific structure) to access the IP
	 * 
	 * struct sockaddr_in {
	 *     sa_family_t    sin_family;  // Address family (AF_INET)
	 *     in_port_t      sin_port;    // Port number (not used here)
	 *     struct in_addr sin_addr;    // IPv4 address (THIS IS WHAT WE WANT)
	 * };
	 * 
	 * struct in_addr {
	 *     uint32_t s_addr;  // 32-bit IPv4 address in network byte order
	 * };
	 * 
	 * EXAMPLE:
	 *   For google.com → 142.250.185.46
	 *   sin_addr.s_addr = 0x2EB9FA8E (in hex, network byte order)
	 *                   = 780641934 (in decimal)
	 */
	addr = (struct sockaddr_in *)result->ai_addr;
	
	/*
	 * Extract the actual IP address
	 * sin_addr.s_addr is in NETWORK BYTE ORDER (big-endian)
	 * 
	 * Network byte order: Most significant byte first
	 * Example: 8.8.8.8 → stored as 0x08080808
	 * 
	 * When you print this directly as %u, you get a large number.
	 * To display as human-readable (8.8.8.8), use inet_ntoa() or inet_ntop()
	 */
	ip = addr->sin_addr.s_addr;

	// Debug: Print resolved IP (raw format - not human readable)
	//printf("IP address resolved (raw): %s\n", inet_ntoa(addr->sin_addr));
	
	/*
	 * freeaddrinfo() - CRITICAL: Free memory allocated by getaddrinfo()
	 * 
	 * getaddrinfo() uses malloc internally to create the result linked list.
	 * Always call freeaddrinfo() when done, or you'll have a memory leak.
	 */
	freeaddrinfo(result);
	
	return ip;
}

/*
 * get_ip: Determines if input is an IP address or hostname, then resolves it
 * 
 * PARAMETERS:
 *   - target: either an IPv4 address string ("8.8.8.8") or hostname ("google.com")
 * 
 * RETURN:
 *   - IPv4 address in network byte order (uint32_t)
 *   - 0 on failure
 * 
 * LOGIC:
 *   1. First, try to parse target as an IP address using inet_pton()
 *   2. If that works, return the IP directly (no DNS lookup needed)
 *   3. If that fails, assume it's a hostname and call dns_lookup()
 * 
 * WHY THIS APPROACH:
 *   - Saves time: If user provides "8.8.8.8", no need for DNS query
 *   - Handles both cases: Works for both IPs and hostnames
 */
uint32_t get_ip(char *target, t_flags *flags)
{
	//struct in_addr ipv4;

	/*
	 * inet_pton() - Convert IP address string to binary format
	 * 
	 * PROTOTYPE:
	 *   int inet_pton(int af,           // Address family (AF_INET or AF_INET6)
	 *                 const char *src,  // String IP address (e.g., "8.8.8.8")
	 *                 void *dst);       // Output buffer (struct in_addr)
	 * 
	 * RETURN VALUES:
	 *   1  = Success (valid IP address converted)
	 *   0  = Invalid IP format (not a valid IP string)
	 *   -1 = Error (af is not valid address family)
	 * 
	 * EXAMPLES:
	 *   Input: "8.8.8.8"         → Returns 1, ipv4.s_addr = 0x08080808
	 *   Input: "192.168.1.1"     → Returns 1, ipv4.s_addr = 0xC0A80101
	 *   Input: "google.com"      → Returns 0 (not an IP address)
	 *   Input: "999.999.999.999" → Returns 0 (invalid IP)
	 * 
	 * struct in_addr:
	 *   Contains a single field: s_addr (32-bit IPv4 address)
	 *   Stored in NETWORK BYTE ORDER (big-endian)
	 * 
	 * WHY USE inet_pton() INSTEAD OF inet_addr()?
	 *   - inet_addr() is obsolete
	 *   - inet_pton() supports both IPv4 and IPv6
	 *   - inet_pton() has better error handling
	 *   - inet_pton() is thread-safe
	 *
	if (inet_pton(AF_INET, target, &ipv4) == 1)
	{
		// Target is a valid IP address string
		// Return it directly (already in network byte order)
		return ipv4.s_addr;
	}*/

	// Target is not a valid IP, assume it's a hostname
	// Call dns_lookup() to resolve it via DNS
	// Examples: "google.com", "localhost", "example.org"
	return dns_lookup(target, flags);
}