#include "game.h"
#include "network.h"
#include <pthread.h>
#include <iostream>

network::network(){}

bool network::isConnected(){
    return connected;
}

void network::checkConnectionStatus(){
    if (!connected) {
        fd_set wfds;
        struct timeval tv;
        FD_ZERO(&wfds);
        FD_SET(sock, &wfds);
        tv.tv_sec = 0;
        tv.tv_usec = 100000;
        int sel = select(sock+1, NULL, &wfds, NULL, &tv);
        if (sel > 0 && FD_ISSET(sock, &wfds)) {
            int err = 0;
            socklen_t len = sizeof(err);
            if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len) == 0 && err == 0) {
                this->connected = true;
                int flags = fcntl(sock, F_GETFL, 0);
                fcntl(sock, F_SETFL, flags & ~O_NONBLOCK);
                cout << "Conectado ao servidor com sucesso!" << endl;
            }
        }
    }
}

void network::connectToServer(string ip, int port) {
    if (!connected) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("Erro ao criar socket");
            return;
        }

        struct sockaddr_in serv_addr {};
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
            cerr << "Endereço IP inválido: " << ip << endl;
            return;
        }

        cout << "Conectando ao servidor..." << endl;
        int flags = fcntl(sock, F_GETFL, 0);
        fcntl(sock, F_SETFL, flags | O_NONBLOCK);

        int res = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        if (res == 0) {
            this->connected = true;
            fcntl(sock, F_SETFL, flags);
            cout << "Conectado ao servidor!" << endl;
        } else {
            cout << "Conexão assíncrona iniciada; thread de rede fará novas tentativas." << endl;
        }
    }
}

void network::getPlay(game &g){
    if (!connected) {
        fd_set wfds;
        struct timeval tv;
        FD_ZERO(&wfds);
        FD_SET(sock, &wfds);
        tv.tv_sec = 0;
        tv.tv_usec = 100000;
        int sel = select(sock+1, NULL, &wfds, NULL, &tv);
        if (sel > 0 && FD_ISSET(sock, &wfds)) {
            int err = 0;
            socklen_t len = sizeof(err);
            if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len) == 0 && err == 0) {
                this->connected = true;
                int flags = fcntl(sock, F_GETFL, 0);
                fcntl(sock, F_SETFL, flags & ~O_NONBLOCK);
                cout << "Rede: conectado ao servidor." << endl;
            }
        }
        return;
    }

    PacoteTurno jogada;
    ssize_t r = recv(sock, &jogada, sizeof(PacoteTurno), MSG_DONTWAIT);
    if (r == 0) {
        cerr << "Servidor fechou a conexão" << endl;
        this->connected = false;
        close(sock);
        return;
    } else if (r < 0) {
        return;
    } else if (r == sizeof(PacoteTurno)) {
        g.PushPacote(jogada);
    } else {
        recv_all(sock, ((char*)&jogada) + r, sizeof(PacoteTurno) - r);
        g.PushPacote(jogada);
    }
}

void recv_all(int sock, void* buffer, size_t size) {
    size_t total = 0;
    while (total < size) {
        ssize_t bytes = recv(sock, (char*)buffer + total, size - total, 0);
        if (bytes <= 0) {
            perror("Erro ou desconexão ao receber dados");
            return;
        }
        total += bytes;
    }
}

void send_all(int sock, void* buffer, size_t size) {
    size_t total = 0;
    while (total < size) {
        ssize_t bytes = send(sock, (char*)buffer + total, size - total, 0);
        if (bytes <= 0) {
            perror("Erro ao enviar dados");
            return;
        }
        total += bytes;
    }
}

struct SendArg{
    int sock;
    PacoteTurno packet;
    size_t size;
};

void network::ThreadSend(void *arg){
}

bool network::sendPlay(game &g) {
    if (!this->connected) {
        printf("Não conectado - não pode enviar jogada\n");
        return false;
    }
    
    // CORREÇÃO CRÍTICA: envia pendingPacket em vez de PacoteAtual
    // PacoteAtual contém o estado ANTIGO do servidor
    // pendingPacket contém o estado NOVO após a jogada local
    // printf("Enviando pacote de jogada ao servidor...\n");
    send_all(this->sock, &g.pendingPacket, sizeof(PacoteTurno));
    return true;
}

struct ThreadArgs {
    network* net;
    game* g;
};

void* networkThread(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;

    while (true) {
        args->net->getPlay(*args->g);
        usleep(16000); // ~60 fps
    }

    return nullptr;
}

void network::startThread(game& g) {
    pthread_t tid;
    ThreadArgs* args = new ThreadArgs{this, &g};

    if (pthread_create(&tid, nullptr, networkThread, args) != 0) {
        perror("Erro ao criar thread de rede");
        exit(1);
    }

    pthread_detach(tid);
}