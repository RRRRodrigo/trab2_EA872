#include <vector>
#include <chrono>
#include <thread>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ncurses.h>
#include <cstdlib>
#include "oo_model.hpp"
#include "oo_server.hpp"

// JSON library
#include "json.hpp"

//servidor
using namespace std::chrono;

uint64_t get_now_ms() {
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

using json = nlohmann::json;

int main() {
  int rodar = 1;
  Projetil *projetil = new Projetil();
  projetil->mapreader();

  ListaDeCorpos *l = new ListaDeCorpos();
  Corpo* player[NUM_PLAY];

  for(int i=0;i<NUM_PLAY;i++){
    player[i] = new Corpo(10, 0, i+10);
    l->add_corpo(player[i]); //cria corpo para o player
  }

  Tela *tela = new Tela(l, 400, 400, 400, 400);
  tela->init(); //Tela do cliente

  ListaDeCorpos* ldc = tela->get_lista();
  std::vector<Corpo*>* mandar = ldc->get_corpos();

  Jogador* este_jogador = new Jogador();

  //thread de enviar dados
  std::thread cliente_envio(receberDados, ldc, este_jogador);
  (este_jogador->client_thread).swap(cliente_envio);

  //thread para receberDados
  std::thread cliente_recebe(enviarDados, este_jogador);
  (este_jogador->keyboard_send).swap(cliente_recebe);

  uint64_t t1;
  uint64_t T;

  T = get_now_ms();
  t1 = T;

  while(!este_jogador->AllowClientProceed){}

  while (este_jogador->getAlive()) {

    tela->update(projetil->mapa);

    if(projetil->update(l->get_corpos(), este_jogador->id)){
      este_jogador->rodando = 0; //todos morreram
      break;
    }
    /*if( !(*(l->get_corpos()))[este_jogador->id]->check_alive() ){
    //adquire lista de corpos, adquire o corpo desse programa ,checa o estado do corpo com o id desse programa
      este_jogador->rodando = 0; //esse morreu
      break;
    }*/
    std::this_thread::sleep_for (std::chrono::milliseconds(100));
  }

  tela->stop();
  //imprimir pontuação
  T  = get_now_ms();
  endwin();
  std::cout << (T-t1)/50 << "\n";
  return 0;
}
