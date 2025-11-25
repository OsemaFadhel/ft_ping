# ft_ping

ft_ping is a simple ICMP echo request/reply utility, similar to the standard `ping` command. It is written in C and designed to test network connectivity between hosts using raw sockets.

The project implements the core functionality of the ping utility, including sending ICMP ECHO_REQUEST packets, handling ICMP ECHO_REPLY packets, and calculating round-trip time statistics.

## Features

*   **ICMP Protocol**: Implements ICMP ECHO_REQUEST and ECHO_REPLY handling.
*   **DNS Resolution**: Supports hostname resolution and reverse DNS lookups.
*   **Statistics**: Reports packet loss and RTT (min/avg/max/mdev) metrics.
*   **Error Handling**: Detects timeouts, destination unreachable, and TTL exceeded errors.
*   **Verbose Mode**: Optional detailed output for debugging.

## Usage

Build the project using Make:

```bash
make
```

Run the utility:

```bash
./ft_ping [options] <destination>
```

### Options

*   `-v`: Enable verbose output.
*   `-h`: Display help message.
*   `-?`: Display help message.

## Examples

Basic usage:

```bash
./ft_ping google.com
```

Verbose output:

```bash
./ft_ping -v 8.8.8.8
```

## Output Format

The program displays per-packet results and summary statistics upon termination (SIGINT):

```
64 bytes from google.com (142.250.184.14): icmp_seq=1 ttl=116 time=14.2 ms
...
--- google.com ft_ping statistics ---
2 packets transmitted, 2 received, 0% packet loss, time 2003ms
rtt min/avg/max/mdev = 13.800/14.000/14.200/0.200 ms
```