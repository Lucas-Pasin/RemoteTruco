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
    
    while (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << ".";
        exit(1);
        usleep(500000); // espera 0.5s antes de tentar novamente
    }
    cout<<endl;
    

    cout << "\nConectado ao servidor!" << endl;
    
}


void network::getPlay(game &g){
    PacoteTurno jogada;
    recv_all(sock, &jogada, sizeof(PacoteTurno));
    g.PacoteAtual = jogada;
}

void recv_all(int sock, void* buffer, size_t size) {
    size_t total = 0;
    while (total < size) {
        ssize_t bytes = recv(sock, (char*)buffer + total, size - total, 0);
        if (bytes <= 0) {
            perror("Erro ou desconexão ao receber dados");
            exit(1);
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
            exit(1);
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

bool sendPlay(game &g){
    pthread_t tid;
    if (pthread_create(&tid, nullptr, g.net.startThread, &g) != 0) {
        perror("Erro ao criar thread de rede");
        exit(1);
    }
    pthread_join(tid,NULL);
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