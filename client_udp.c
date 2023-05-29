#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int main() {
    int clt_sock;
    int err;
    struct sockaddr_in srv_sockaddr;
    socklen_t server_addr_len;

    clt_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (clt_sock == -1) {
        perror("failed to create socket\n");
        exit(EXIT_FAILURE);
    }

    memset(&srv_sockaddr, 0, sizeof(srv_sockaddr));
    srv_sockaddr.sin_family = AF_INET;
    srv_sockaddr.sin_port = htons(SERVER_PORT);
   + err = inet_pton(AF_INET, SERVER_IP, &(srv_sockaddr.sin_addr));
    if (err <= 0) {
        perror("failed to pton\n");
        close(clt_sock);
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    size_t ret;
    while (1) {

        printf("Enter a message: ");
        fgets(buffer, 1024, stdin);
        ret = sendto(clt_sock, buffer, strlen(buffer), 0, (struct sockaddr *) &srv_sockaddr, sizeof(srv_sockaddr));
        if (ret == -1) {
            perror("failed to send\n");
            close(clt_sock);
            exit(EXIT_FAILURE);
        }
        if(ret == 0){
            perror("connection closed\n");
            close(clt_sock);
            exit(EXIT_FAILURE);
        }

        server_addr_len = sizeof(srv_sockaddr);
        memset(buffer, 0, sizeof(buffer));
        ret = recvfrom(clt_sock, buffer, 1024, 0, (struct sockaddr *) &srv_sockaddr, &server_addr_len);
        if ( ret == -1) {
            perror("failed to recv\n");
            close(clt_sock);
            exit(EXIT_FAILURE);
        }

        printf("Server response: %s\n", buffer);
    }

    close(clt_sock);

    return 0;
}
