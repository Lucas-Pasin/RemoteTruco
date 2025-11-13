#include "game.h"
#include "network.h"
#include <pthread.h>
#include <iostream>

network::network(){

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Erro ao criar socket");
        exit(1);
    }

    struct sockaddr_in serv_addr {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(LISTEN_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        cerr << "Endereço IP inválido: " << SERVER_IP << endl;
        exit(1);
    }

    cout << "Conectando ao servidor..." << endl;
    // Attempt non-blocking connect loop without exiting the program.
    // Set socket to non-blocking and try to connect; if it fails, keep socket open and set connected flag later.
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    int res = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (res == 0) {
        // immediate connection
        this->connected = true;
        // restore blocking mode
        fcntl(sock, F_SETFL, flags);
        cout << "Conectado ao servidor!" << endl;
    } else {
        // will be connected by the network thread later
        cout << "Conexão assíncrona iniciada; thread de rede fará novas tentativas." << endl;
    }
    
}


void network::getPlay(game &g){
    if (!connected) {
        // try to connect (blocking small period) if not connected yet
        fd_set wfds;
        struct timeval tv;
        FD_ZERO(&wfds);
        FD_SET(sock, &wfds);
        tv.tv_sec = 0;
        tv.tv_usec = 100000; // 100ms
        int sel = select(sock+1, NULL, &wfds, NULL, &tv);
        if (sel > 0 && FD_ISSET(sock, &wfds)) {
            int err = 0;
            socklen_t len = sizeof(err);
            if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len) == 0 && err == 0) {
                this->connected = true;
                // restore blocking mode
                int flags = fcntl(sock, F_GETFL, 0);
                fcntl(sock, F_SETFL, flags & ~O_NONBLOCK);
                cout << "Rede: conectado ao servidor." << endl;
            }
        }
        // nothing to receive if not connected
        return;
    }

    PacoteTurno jogada;
    ssize_t r = recv(sock, &jogada, sizeof(PacoteTurno), MSG_DONTWAIT);
    if (r == 0) {
        // connection closed
        cerr << "Servidor fechou a conexão" << endl;
        this->connected = false;
        close(sock);
        return;
    } else if (r < 0) {
        // no data available or error
        return;
    } else if (r == sizeof(PacoteTurno)) {
        g.PushPacote(jogada);
    } else {
        // partial receive: use recv_all to block until full packet available
        recv_all(sock, ((char*)&jogada) + r, sizeof(PacoteTurno) - r);
        g.PushPacote(jogada);
    }
}

void recv_all(int sock, void* buffer, size_t size) {
    size_t total = 0;
    while (total < size) {
        ssize_t bytes = recv(sock, (char*)buffer + total, size - total, 0);
        if (bytes <= 0) {
            // don't exit the whole program; mark disconnected and return
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
    if (!this->connected) return false;
    // send the current packet (non-blocking attempt)
    send_all(this->sock, &g.PacoteAtual, sizeof(PacoteTurno));
    return true;
}


struct ThreadArgs {
    network* net;
    game* g;
};

// função que a thread vai executar
void* networkThread(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;

    while (true) {
        args->net->getPlay(*args->g);
        // Pequena pausa para não travar a CPU
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

    // Se não precisar esperar o fim da thread, pode ser destacada
    pthread_detach(tid);
}