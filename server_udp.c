#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080

int main() {
    int srv_sock;
    int err;
    struct sockaddr_in srv_sockaddr, clt_sockaddr;
    socklen_t client_addr_len;

    srv_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (srv_sock  == -1) {
        perror("failed to create socket\n");
        exit(EXIT_FAILURE);
    }

    memset(&srv_sockaddr, 0, sizeof(srv_sockaddr));
    srv_sockaddr.sin_family = AF_INET;
    srv_sockaddr.sin_port = htons(SERVER_PORT);
    srv_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    err = bind(srv_sock, (struct sockaddr *) &srv_sockaddr, sizeof(srv_sockaddr));
    if (err == -1) {
        perror("failed to bind\n");
        close(srv_sock);
        exit(EXIT_FAILURE);
    }

    printf("UDP echo server listening on port %d\n", SERVER_PORT);

    char buffer[1024];
    while (1) {
        client_addr_len = sizeof(clt_sockaddr);
        size_t received_bytes = recvfrom(srv_sock, buffer, 1024, 0, (struct sockaddr *) &clt_sockaddr, &client_addr_len);
        if (received_bytes == -1) {
            perror("failed to receive\n");
            close(srv_sock);
            exit(EXIT_FAILURE);
        }
        buffer[received_bytes] = '\0';
        printf("Received message from %s:%d: %s\n", inet_ntoa(clt_sockaddr.sin_addr), ntohs(clt_sockaddr.sin_port), buffer);
        size_t sent_bytes = sendto(srv_sock, buffer, received_bytes, 0, (struct sockaddr *) &clt_sockaddr, client_addr_len);
        if (sent_bytes == -1) {
            perror("failed to send\n");
            close(srv_sock);
            exit(EXIT_FAILURE);
        }
    }

    close(srv_sock);
    return 0;
}