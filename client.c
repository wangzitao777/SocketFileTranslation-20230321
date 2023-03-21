#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#define BUF_SIZE 1024
void error_handling(char *message);
int main(int argc, char *argv[]) {
    int sock;
    char buf[BUF_SIZE];
    FILE *fp;
    int read_cnt;
    int total_read_cnt = 0;
    int file_size;
    struct sockaddr_in serv_addr;
    if (argc != 4) {
        printf("Usage: %s <ip> <port> <filename>\n", argv[0]);
        exit(1);
    }
    // 创建套接字
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        error_handling("socket() error");
    }
    // 初始化服务器地址信息
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    // 连接服务器
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("connect() error");
    }
    // 发送文件名
    //printf("Input file name: ");
    char *filename = argv[3];
    strcpy(buf, filename);
    //scanf("%s", buf);
    fp = fopen(buf, "rb");
    if (fp == NULL) {
        error_handling("fopen() error");
    }
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    sprintf(buf, "%s %d", buf, file_size);
    write(sock, buf, BUF_SIZE);
    // 接收服务端的继续信号
    read(sock, buf, BUF_SIZE);
    // 发送文件数据
    while (total_read_cnt < file_size) {
        read_cnt = fread(buf, 1, BUF_SIZE, fp);
        write(sock, buf, read_cnt);
        total_read_cnt += read_cnt;
        // 接收服务端的继续信号
        read(sock, buf, BUF_SIZE);
    }
    fclose(fp);
    close(sock);
    return 0;
}
void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}