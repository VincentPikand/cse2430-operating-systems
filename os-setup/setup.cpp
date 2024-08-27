#include <netinet/ip.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void) {
    int send_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (send_fd == -1) {
        fprintf(stderr, "Unable to open socket for sending: %s\n", strerror(errno));
        return 1;
    }
    int recv_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (recv_fd == -1) {
        fprintf(stderr, "Unable to open socket for receiving: %s\n", strerror(errno));
        return 1;
    }
    int enable_broadcast = 1;
    int sso_bc_status = setsockopt(send_fd, SOL_SOCKET, SO_BROADCAST, &enable_broadcast, sizeof(enable_broadcast));
    if (sso_bc_status) {
        fprintf(stderr, "Unable to enable broadcast mode on socket: %s\n", strerror(errno));
        return 1;
    }
    struct sockaddr_in in_recv_addr = {0};
    in_recv_addr.sin_family = AF_INET;
    in_recv_addr.sin_port = htons(2431);
    in_recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int bind_status = bind(recv_fd, (struct sockaddr *) &in_recv_addr, sizeof(in_recv_addr));
    if (bind_status == -1) {
        fprintf(stderr, "Unable to bind address: %s\n", strerror(errno));
        return 1;
    }
    struct sockaddr_in in_send_addr = {0};
    in_send_addr.sin_family = AF_INET;
    in_send_addr.sin_port = htons(2430);
    in_send_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    ssize_t nbytes_sent = sendto(send_fd, "cse2430", 8, 0, (struct sockaddr *) &in_send_addr, sizeof(in_send_addr));
    if (nbytes_sent == -1) {
        fprintf(stderr, "Unable to write outgoing message: %s\n", strerror(errno));
        return 1;
    }
    for (;;) {
        struct sockaddr_storage peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);
        char buf[16];
        ssize_t nbytes = recvfrom(recv_fd, buf, sizeof(buf), 0, (struct sockaddr *) &peer_addr, &peer_addr_len);
        if (nbytes == -1) {
            fprintf(stderr, "Unable to read incoming message: %s\n", strerror(errno));
            continue;
        }
        if (nbytes != 2) continue;
        if (memcmp(buf, "pi", 2)) continue;
        if (peer_addr.ss_family == AF_INET) {
            const struct sockaddr_in *peer_addr_in4 = (struct sockaddr_in *) &peer_addr;
            const unsigned char *addr = (unsigned char *) &peer_addr_in4->sin_addr.s_addr;
            printf("Found a Pi at %u.%u.%u.%u\n", addr[0], addr[1], addr[2], addr[3]);
        } else if (peer_addr.ss_family == AF_INET6) {
            const struct sockaddr_in6 *peer_addr_in6 = (struct sockaddr_in6 *) &peer_addr;
            const unsigned short *addr = (unsigned short *) peer_addr_in6->sin6_addr.s6_addr;
            printf("Found a Pi at %hx:%hx:%hx:%hx:%hx:%hx:%hx:%hx\n",
                addr[0], addr[1], addr[2], addr[3],
                addr[4], addr[5], addr[6], addr[7]
            );
        } else {
            printf("Found a Pi at an address of an unrecognized family\n");
        }
    }
}