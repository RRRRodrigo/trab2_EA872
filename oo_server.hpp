#ifndef OO_SERVER_HPP
#define OO_SERVER_HPP

#include <thread>

#define NUM_PLAY 2

class Instance {
  private:
    int jogadores;
    int rodando;
    int vivo[NUM_PLAY];
    char jog_input[NUM_PLAY];
    struct sockaddr_in myself, client;
    socklen_t client_size;

  public:
    bool AllowClientProceed;
    int socket_fd, connection_fd[NUM_PLAY];
    std::thread server_thread;
    std::thread keyboard_recv[NUM_PLAY];
    Instance();
    int getsocket();
    int getRodando();
    struct sockaddr_in getmyself();
    struct sockaddr_in getclient();
    socklen_t getclientsize();
    static void esperar_thread(Instance* insta);
    static void envioParaJogadores(ListaDeCorpos* l, Instance* insta);
    static void adquirirdadosJogadores(int ID, Instance* insta);
    char getInput(int Id);
};

class Jogador {
  private:
    char jog_input;

  public:
    Jogador();
    std::thread keyboard_send, client_thread;
    bool AllowClientProceed;
    int socket_fd, connection_fd, id, rodando;
    struct sockaddr_in client, myself;
    int getAlive();
};

void enviarDados(Jogador* jog);
void receberDados(ListaDeCorpos* ldc_new, Jogador* jog);

#endif
