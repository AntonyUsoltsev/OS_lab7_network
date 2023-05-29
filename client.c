#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

int main() {
    int clt_sock;
    struct sockaddr_un srv_sockaddr;
    int err;

    signal(SIGPIPE, SIG_IGN);

    clt_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (clt_sock == -1) {
        perror("failed to create socket\n");
        exit(EXIT_FAILURE);
    }

    memset(&srv_sockaddr, 0, sizeof(struct sockaddr_un));
    srv_sockaddr.sun_family = AF_UNIX;
    strcpy(srv_sockaddr.sun_path, "dsock_file");

    err = connect(clt_sock, (struct sockaddr *) &srv_sockaddr, sizeof(srv_sockaddr));
    if (err == -1) {
        perror("failed to connect\n");
        close(clt_sock);
        exit(EXIT_FAILURE);
    }
    printf("Successful connect to server. Write message:\n");
    char buff[128];
    size_t ret = 0;
    while (1) {

        memset(buff, 0, sizeof(buff));
        fgets(buff, sizeof(buff), stdin);

        ret = write(clt_sock, buff, strlen(buff));
        if (ret == -1 || ret == 0) {
            perror("failed to write\n");
            close(clt_sock);
            exit(EXIT_FAILURE);
        }
        ret = read(clt_sock, &buff, sizeof(buff));
        if (ret == -1) {
            perror("failed to read\n");
            sleep(1);
            continue;
        }
        if (ret == 0) {
            perror("connection closed\n");
            close(clt_sock);
            exit(EXIT_FAILURE);
        }

        printf("Server answer: %s\n", buff);

    }
    close(clt_sock);
}