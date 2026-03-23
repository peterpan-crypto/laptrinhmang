#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Cach dung: %s <dia chi IP> <cong>\n", argv[0]);
        return 1;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);

    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) {
        perror("Loi tao socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        printf("Dia chi IP khong hop le\n");
        close(client_sock);
        return 1;
    }

    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Loi ket noi");
        close(client_sock);
        return 1;
    }

    char mssv[50];
    char hoten[100];
    char ngaysinh[50];
    float diemtb;

    printf("Nhap MSSV: ");
    fgets(mssv, sizeof(mssv), stdin);
    mssv[strcspn(mssv, "\n")] = '\0';

    printf("Nhap ho ten: ");
    fgets(hoten, sizeof(hoten), stdin);
    hoten[strcspn(hoten, "\n")] = '\0';

    printf("Nhap ngay sinh (YYYY-MM-DD): ");
    fgets(ngaysinh, sizeof(ngaysinh), stdin);
    ngaysinh[strcspn(ngaysinh, "\n")] = '\0';

    printf("Nhap diem trung binh: ");
    scanf("%f", &diemtb);

    char data[BUFFER_SIZE];
    snprintf(data, sizeof(data), "%s %s %s %.2f", mssv, hoten, ngaysinh, diemtb);

    if (send(client_sock, data, strlen(data), 0) < 0) {
        perror("Loi gui du lieu");
        close(client_sock);
        return 1;
    }

    printf("Da gui du lieu: %s\n", data);

    close(client_sock);
    return 0;
}
