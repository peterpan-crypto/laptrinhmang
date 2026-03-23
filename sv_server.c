#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Cach dung: %s <cong> <ten file log>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    char *log_file = argv[2];

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Loi tao socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Loi bind");
        close(server_sock);
        return 1;
    }

    if (listen(server_sock, 5) < 0) {
        perror("Loi listen");
        close(server_sock);
        return 1;
    }

    printf("Server dang lang nghe o cong %d...\n", port);

    while (1) {
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Loi accept");
            continue;
        }

        char buffer[BUFFER_SIZE];
        int n = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);

        if (n > 0) {
            buffer[n] = '\0';

            // Lấy thời gian hiện tại
            time_t now = time(NULL);
            struct tm *tm_info = localtime(&now);

            char time_str[64];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

            char *client_ip = inet_ntoa(client_addr.sin_addr);

            // In ra màn hình
            printf("%s %s %s\n", client_ip, time_str, buffer);

            // Ghi vào file
            FILE *fp = fopen(log_file, "a");
            if (fp == NULL) {
                perror("Loi mo file log");
            } else {
                fprintf(fp, "%s %s %s\n", client_ip, time_str, buffer);
                fclose(fp);
            }
        }

        close(client_sock);
    }

    close(server_sock);
    return 0;
}
