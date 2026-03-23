#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

void read_greeting_file(const char *filename, char *buffer, int size) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Khong mo duoc file cau chao");
        buffer[0] = '\0';
        return;
    }

    int n = fread(buffer, 1, size - 1, fp);
    buffer[n] = '\0';
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Cach dung: %s <cong> <tep tin chua cau chao> <tep tin luu noi dung client gui den>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    char *greeting_file = argv[2];
    char *output_file = argv[3];

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

        printf("Client ket noi tu %s:%d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        char greeting[BUFFER_SIZE];
        read_greeting_file(greeting_file, greeting, BUFFER_SIZE);

        if (strlen(greeting) > 0) {
            send(client_sock, greeting, strlen(greeting), 0);
        }

        FILE *fp = fopen(output_file, "a");
        if (fp == NULL) {
            perror("Khong mo duoc file luu du lieu");
            close(client_sock);
            continue;
        }

        char buffer[BUFFER_SIZE];
        int bytes_received;

        while ((bytes_received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
            buffer[bytes_received] = '\0';
            fprintf(fp, "%s", buffer);
        }

        fprintf(fp, "\n");
        fclose(fp);

        printf("Da ghi du lieu client vao file %s\n", output_file);
        close(client_sock);
    }

    close(server_sock);
    return 0;
}
