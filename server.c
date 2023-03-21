#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#define BUF_SIZE 1024
void error_handling(char *message);
int main(int argc, char *argv[]) {
    int serv_sock;
    int clnt_sock;
    char buf[BUF_SIZE];
    int read_cnt;
    int file_size;
    int total_read_cnt = 0;
    FILE *fp;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;
    pid_t pid;
    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    // 创建套接字
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        error_handling("socket() error");
    }
    // 初始化服务器地址信息
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    // 绑定套接字
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }
    // 监听套接字
    if (listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }
    while (1) { // 循环等待客户端连接
        clnt_addr_size = sizeof(clnt_addr);
        // 接收客户端连接
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1) {
            continue; // 如果连接失败，继续等待下一个连接
        }
        pid = fork(); // 创建子进程处理客户端请求
        if (pid == -1) {
            close(clnt_sock); // 创建子进程失败，关闭客户端套接字
            continue; // 继续等待下一个连接
        } else if (pid == 0) { // 子进程代码，处理客户端请求
            close(serv_sock); // 关闭服务器套接字
            // 接收文件名
            read_cnt = read(clnt_sock, buf, BUF_SIZE);
            if (read_cnt == -1) {
                error_handling("read() error");
            }
            buf[read_cnt] = '\0';
            char *token = strtok(buf, " ");
            fp = fopen(token, "wb");
            if (fp == NULL) {
                error_handling("fopen() error");
            }
            token = strtok(NULL, " ");
            file_size = atoi(token);
            // 发送继续信号
            write(clnt_sock, "go", BUF_SIZE);
            // 接收文件数据
            while ((read_cnt = read(clnt_sock, buf, BUF_SIZE)) != 0) {
                fwrite(buf, 1, read_cnt, fp);
                // 发送继续信号
                write(clnt_sock, "go", BUF_SIZE);
                total_read_cnt += read_cnt;
                if (total_read_cnt >= file_size) {
                    break;
                }
            }
            fclose(fp);
            close(clnt_sock);
            return 0;
        } else { // 父进程代码，关闭客户端套接字，继续等待下一个连接
            close(clnt_sock);
            continue;
        }
    }
    close(serv_sock);
    return 0;
}
void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}