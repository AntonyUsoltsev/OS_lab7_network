#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#define CLIENTS_COUNT 3

int main() {
    int srv_sock;
    int clt_sock;
    struct sockaddr_un srv_sockaddr, clt_socksaddr;
    socklen_t len = 0;
    int err;

    signal(SIGPIPE, SIG_IGN);

    srv_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (srv_sock == -1) {
        perror("failed to create socket\n");
        exit(EXIT_FAILURE);
    }

    memset(&srv_sockaddr, 0, sizeof(struct sockaddr_un));
    srv_sockaddr.sun_family = AF_UNIX;
    strcpy(srv_sockaddr.sun_path, "dsock_file");
    unlink("dsock_file");

    err = bind(srv_sock, (struct sockaddr *) &srv_sockaddr, sizeof(srv_sockaddr));
    if (err == -1) {
        perror("failed to bind\n");
        close(srv_sock);
        exit(EXIT_FAILURE);
    }

    err = listen(srv_sock, CLIENTS_COUNT);
    if (err == -1) {
        perror("failed to listen\n");
        close(srv_sock);
        exit(EXIT_FAILURE);
    }
    while (1) {
        printf("waiting clients\n");
        memset(&clt_socksaddr, 0, sizeof(struct sockaddr_un));

        clt_sock = accept(srv_sock, (struct sockaddr *) &clt_socksaddr, &len);
        if (clt_sock == -1) {
            perror("failed to accept\n");
            close(srv_sock);
            exit(EXIT_FAILURE);
        }

        len = sizeof(clt_socksaddr);
        err = getpeername(clt_sock, (struct sockaddr *) &clt_socksaddr, &len);
        if (err == -1) {
            perror("failed to get peer name\n");
            close(srv_sock);
            close(clt_sock);
            exit(EXIT_FAILURE);;
        }
        pid_t pid = fork();
        if (pid == 0) {
            break;
        }
        close(clt_sock);


    }
    close(srv_sock);
    while (1) {
        size_t ret;
        char buff[128];
        ret = read(clt_sock, buff, sizeof(buff));
        if (ret == -1 || ret == 0) {
            printf("stop reading\n");
            break;
        }
        ret = write(clt_sock, buff, ret);
        if(ret == -1 || ret == 0){
            printf("stop writing\n");
            break;
        }

    }
    return 0;
}

