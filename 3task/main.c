#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 80
#define BUFFER_SIZE 4096

void* handle_connection(void* client_socket);
int connect_to_target(char* host, int port);

int main(int argc, char* argv[]) {
    int server, client_socket;
    struct sockaddr_in address;
    int addr_len = sizeof(address);
    int opt = 1;


    if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        printf("socket failed");
        return -1;
    }

    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        printf("setsockopt");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);


    if (bind(server, (struct sockaddr*)&address, sizeof(address)) < 0) { //связыввание с адресом и портом
        printf("bind failed");
        return -1;
    }

    // установки сокета в режим прослушивания входящих соединений
    if (listen(server, 10) < 0) {
        printf("listen");
        return -1;
    }

    printf("HTTP Proxy Server Listening on port %d\n", PORT);

    while (1) {
        // Принятие соединения от клиента - создание нового клиентского сокета
        if ((client_socket = accept(server, (struct sockaddr*)&address, (socklen_t *)&addr_len)) < 0) {
            printf("accept");
            return -1;
        }

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_connection, (void*)&client_socket) != 0) {
            printf("pthread_create");
            continue;
        }
        pthread_detach(thread);
    }

    return 0;
}

void* handle_connection(void* client_socket) {
    int client_sock = *(int*)client_socket;

    char buffer[BUFFER_SIZE];

    int target_sock, read_bytes;
    int data_size = 0;
    char* data = NULL;

    // Чтение запроса от клиента
    while (1) {
        read_bytes = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (read_bytes <= 0) {
            break;
        }

        data = realloc(data, data_size + read_bytes);
        memcpy(data + data_size, buffer, read_bytes);
        data_size += read_bytes;

        // Проверка на наличие CRLF
        if (strstr(data, "\r\n\r\n") != NULL) {
            break;
        }
    }


    char host[_SC_HOST_NAME_MAX + 1] = {0};
    char* host_line_start = strstr(data, "Host: ");
    if (!host_line_start) {
        host_line_start = strstr(data, "host: ");
    }

    if (host_line_start) {
        char* host_line_end = strstr(host_line_start, "\r\n");
        if (!host_line_end) {
            host_line_end = strstr(host_line_start, "\n");
        }

        if (host_line_end) {
            int host_length = host_line_end - (host_line_start + 6);
            strncpy(host, host_line_start + 6, host_length);
            host[host_length] = '\0';
        } else {
            printf("Host header line end not found.\n");
        }
    } else {
        printf("Host header not found in request.\n");
        close(client_sock);
        return NULL;
    }

    printf("Connecting to host: %s\n", host);


    target_sock = connect_to_target(host, PORT);
    if (target_sock < 0) {
        close(client_sock);
        return NULL;
    }


    send(target_sock, data, data_size, 0);
    free(data);


    while ((read_bytes = recv(target_sock, buffer, BUFFER_SIZE, 0)) > 0) {
        send(client_sock, buffer, read_bytes, 0);
    }


    close(target_sock);
    close(client_sock);
    return NULL;
}

int connect_to_target(char* host, int port) {
    struct hostent *he;
    struct sockaddr_in server_addr;
    int sock;

    // Получение информации о хосте
    if ((he = gethostbyname(host)) == NULL) {
        printf("gethostbyname failed");
        return -1;
    }


    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error");
        return -1;
    }


    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
    memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    // Подключение к серверу
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    return sock;
}
