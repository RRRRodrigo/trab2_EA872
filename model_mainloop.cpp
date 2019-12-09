#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "oo_model.hpp"
#include "oo_server.hpp"

// JSON library
#include "json.hpp"

using namespace std::chrono;

uint64_t get_now_ms() {
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int main (){
  int i = 0;
  ListaDeCorpos *l = new ListaDeCorpos(); //adicionar
  Corpo* player[NUM_PLAY];

  for(i=0;i<NUM_PLAY;i++){
    player[i] = new Corpo(10, 0, 10);
    l->add_corpo(player[i]); //cria corpo para o player
  }

  Fisica *f = new Fisica(l);

  Projetil *projetil = new Projetil();
  projetil->mapreader();

  uint64_t t0;
  uint64_t t1;
  uint64_t deltaT;
  uint64_t T;
  int quit = 0;
  char c;
  i = 0;

  T = get_now_ms();
  t1 = T;

  /*Teclado* teclados[NUM_PLAY];
  for(i=0;i<NUM_PLAY;i++){
    teclados[i]->init(&prosseguir_jogo,projetil->mapa,NUM_PLAY,i+1,l);
    //criar uma classe para cada thread do jogador
    //refazer isso, transformar o teclado em uma funcao (uma unica thread) para receber info de todos os jogadores
  }*/

  //thread de conexao de usuarios
  Instance* mainServer = new Instance();
  std::thread criar_servidor(mainServer->esperar_thread, mainServer);
  (mainServer->server_thread).swap(criar_servidor);

  //espera todos conectarem pra prosseguir
  (mainServer->server_thread).join();

  //thread de enviar info para os jogadores
  std::thread threadFuncionalJogo(mainServer->envioParaJogadores, l, mainServer);
  (mainServer->server_thread).swap(threadFuncionalJogo);

  //thread para receber comando dos jogadores
  for(i=0;i<NUM_PLAY;i++){
    std::thread threadReceberComando(mainServer->adquirirdadosJogadores, i, mainServer);
    (mainServer->keyboard_recv[i]).swap(threadReceberComando);
  }

  while (1) {
    // Atualiza modelo
    f->update(deltaT);

    //Atualiza mapa, se houver colisão, o jogador perde
    std::vector<Corpo*>* atualizar = l->get_corpos();

    //Le os teclados
    for(i=0;i<NUM_PLAY;i++){
      c = mainServer->getInput(i);
      switch(c){
        case 'w':
          f->step(1, i);
        break;
        case 's':
          f->step(-1, i);
        break;
        case 'q':
          quit = !quit;
        break;
      }
      if(quit){
        (*atualizar)[i]->kill();
        quit = 0;
      }
    }

    if(!mainServer->getRodando()){
      break;
    }

    if ( (t1-T) > 1000000 ) break;
    // O framerate do jogo e de 30 fps, e a velocidade do jogo esta relacionada ao framerate. O jogo pode ficar mais rapido (e por consequencia, mais dificil) manipulando este framerate
    std::this_thread::sleep_for (std::chrono::milliseconds(100));
  }

  //Imprime a pontuacao do jogo. A pontuacao e baseada no periodo de tempo em que o jogador ficou vivo.
  //Agora vai ter que enviar a pontuação pra cada um quando perder
  return 0;
}
