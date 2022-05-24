#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "Buffer.h"

using namespace std;

void* request_handler(void* arg);
void error_handling(const string& buf);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <port>" << endl;
        exit(1);
    }

    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr)) == -1) {
        error_handling("bind() error!");
    }
    if (listen(serv_sock, 5)) {
        error_handling("listen() error!");
    }

    sockaddr_in clnt_addr;
    socklen_t clnt_addr_sz = sizeof(clnt_addr);
    while (1) {
        int clnt_sock = accept(serv_sock, reinterpret_cast<sockaddr*>(&clnt_addr), &clnt_addr_sz);
        cout << "Connection Request: " << inet_ntoa(clnt_addr.sin_addr) << ":" << ntohs(clnt_addr.sin_port) << endl;
        pthread_t t_id;
        pthread_create(&t_id, NULL, request_handler, &clnt_sock);
        pthread_detach(t_id);
    }

    close(serv_sock);
    return 0;
}

void* request_handler(void* arg) {
    Buffer buff;
    int clnt_sock = *(reinterpret_cast<int*>(arg));
    int error;
    while (1) {
        buff.read_fd(clnt_sock, &error);
        buff.write_fd(clnt_sock, &error);
    }
    return NULL;
}

void error_handling(const string& buf) {
    cerr << buf << endl;
    exit(1);
}