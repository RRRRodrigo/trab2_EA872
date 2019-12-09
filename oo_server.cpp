#include <vector>
#include <chrono>
#include <thread>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "oo_model.hpp"
#include "oo_server.hpp"
#include "json.hpp"
#include <ncurses.h>
#include <cstdlib>
#include <signal.h>

int Instance::getsocket(){
  return this->socket_fd;
}

struct sockaddr_in Instance::getmyself(){
  return this->myself;
}

struct sockaddr_in Instance::getclient(){
  return this->client;
}

socklen_t Instance::getclientsize(){
  return this->client_size;
}

Instance::Instance(){
  this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  this->client_size = (socklen_t)sizeof(client);
  (this->myself).sin_family = AF_INET;
  (this->myself).sin_port = htons(3001);
  inet_aton("127.0.0.1", &((this->myself).sin_addr));
  //possivelmente uma funcao scanf para adquirir o IP
  if(bind(this->socket_fd,(struct sockaddr*)&(this->myself),sizeof(this->myself))){
    return;
  }
  listen(this->socket_fd, 2);
  this->rodando++;
  this->jogadores = 0;
  this->AllowClientProceed = false;
  std::cout << "Servidor iniciado" << "\n";
}

void Instance::esperar_thread(Instance* insta){
  int connection_here, i = 0;
  while(insta->jogadores < NUM_PLAY){
    connection_here = accept(insta->socket_fd, (struct sockaddr*)&(insta->client), &(insta->client_size));
    if(connection_here == -1)
      continue;
    else{
      std::cout << "Adicionando cliente" << "\n";
      insta->connection_fd[i] = connection_here;
      insta->vivo[i] = 1;
      insta->jogadores++;
      std::cout << "Cliente adicionado" << "\n";
      i++;
    }
  }
  insta->AllowClientProceed = true;
  std::cout << "Todos os jogadores foram adicionados" << "\n";
  return;
}

void Instance::envioParaJogadores(ListaDeCorpos* l, Instance* insta){
  int i, check_going = 0;
  char envio_stringc[300];
  std::string envio;
  char* envio_empty = (char*)calloc(300, sizeof(char));
  Corpo* kill_body;
  while(insta->rodando){
    for(i=0;i<NUM_PLAY;i++){
      envio = serial(l, i, insta->AllowClientProceed);
      if(insta->jogadores == 0){
        std::cout << "Jogo terminou\n";
        insta->rodando = 0;
      }
      if(insta->vivo[i]){
        strcpy(envio_stringc, envio.c_str());
        if(send(insta->connection_fd[i], &(envio_stringc), 300, MSG_NOSIGNAL) == -1){
          std::cout << "Jogador " << i << " morreu\n";
          insta->jogadores--;
          insta->vivo[i] = 0;
          insta->connection_fd[i] = 0;
        }
      }else{
        //mata o jogador
        kill_body = (*l->get_corpos())[i];
        kill_body->kill();
        insta->connection_fd[i] = -1;
      }
    }
    for(i=0;i<NUM_PLAY;i++){
      if(!insta->vivo[i])
        check_going++;
    }
    if(check_going == NUM_PLAY){
      insta->rodando = 0;
      std::cout << "Nenhum jogador esta vivo\n";
    }else{
      check_going = 0;
    }
    std::this_thread::sleep_for (std::chrono::milliseconds(100));
  }
  std::cout << "Parando de enviar info\n";
}

void Instance::adquirirdadosJogadores(int ID, Instance* insta){
  char keybuffer;
  while(insta->rodando){
    if(insta->vivo[ID]){
      if(recv(insta->connection_fd[ID],&keybuffer,1,0) != -1){
        insta->jog_input[ID] = keybuffer;
        if(keybuffer == -3){
          insta->jog_input[ID] = 0;
          insta->vivo[ID] = 0;
        }
      }
    }
    else{
      continue;
    }
    std::this_thread::sleep_for (std::chrono::milliseconds(50));
  }
  std::cout << "Saindo da thread de ouvir clientes\n";
}

char Instance::getInput(int Id){
  char retorno_char = this->jog_input[Id];
  this->jog_input[Id] = 0;
  return retorno_char;
}

Jogador::Jogador(){
  this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in target;
  target.sin_family = AF_INET;
  target.sin_port = htons(3001);
  char ipstring[50];
  scanf("%s", ipstring);
  inet_aton(ipstring, &(target.sin_addr));
  if(connect( this->socket_fd, (struct sockaddr*)&(target), sizeof(target) )){
    std::cout << "Problemas na conexao\n";
    exit(0);
  }
  this->rodando = 1;
  this->AllowClientProceed = false;
}

void receberDados(ListaDeCorpos* ldc_new, Jogador* jog){
  char reply_c[300];
  char envio_kill = -3;
  std::vector<Corpo*>* esteaqui_lista = ldc_new->get_corpos();
  Corpo* esteaqui = (*esteaqui_lista)[jog->id];

  while(jog->rodando){
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    if(recv(jog->socket_fd, reply_c, 300, MSG_DONTWAIT) != -1){
      std::string reply(reply_c);
      unserialize(reply, ldc_new->get_corpos(), &(jog->id), &(jog->AllowClientProceed));
    }
    if(esteaqui->check_alive()){
      continue;
    }
    else{
      jog->rodando = 0;
    }
  }

  send(jog->socket_fd, &(envio_kill), 1, 0);
  close(jog->socket_fd);
}

void enviarDados(Jogador* jog){ //falta essa funcao
  char c;
  while(jog->rodando){
    c = getch();
    send(jog->socket_fd, &c, 1, 0);
    std::this_thread::sleep_for (std::chrono::milliseconds(20));
 }
}

int Instance::getRodando(){
  return this->rodando;
}

int Jogador::getAlive(){
  return this->rodando;
}
