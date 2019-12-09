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

using namespace std::chrono;
using json = nlohmann::json;

Corpo::Corpo(float massa, float velocidade, float posicao) {
  this->massa = massa;
  this->velocidade = velocidade;
  this->posicao = posicao;
  this->alive = true;
}

void Corpo::update(float nova_velocidade, float nova_posicao) {
  this->velocidade = nova_velocidade;
  this->posicao = nova_posicao;
}

float Corpo::get_massa() {
  return this->massa;
}

float Corpo::get_velocidade() {
  return this->velocidade;
}

float Corpo::get_posicao() {
  return this->posicao;
}

bool Corpo::check_alive() {
  return this->alive;
}

void Corpo::kill() {
  this->alive = false;
}

void Corpo::att_corpo(float massa, float velocidade, float posicao, bool alive){
  this->velocidade = velocidade;
  this->massa = massa;
  this->posicao = posicao;
  this->alive = alive;
}

ListaDeCorpos::ListaDeCorpos() {
  this->corpos = new std::vector<Corpo *>(0);
}

void ListaDeCorpos::hard_copy(ListaDeCorpos *ldc) {
  std::vector<Corpo *> *corpos = ldc->get_corpos();
  for (int k=0; k<corpos->size(); k++) {
    Corpo *c = new Corpo( (*corpos)[k]->get_massa(),\
                          (*corpos)[k]->get_posicao(),\
                          (*corpos)[k]->get_velocidade()
                        );
    this->add_corpo(c);
  }
}

void ListaDeCorpos::add_corpo(Corpo *c) {
  (this->corpos)->push_back(c);
}

std::vector<Corpo*> *ListaDeCorpos::get_corpos() {
  return (this->corpos);
}

ListaDeCorpos* Tela::get_lista(){
  return this->lista;
}

Projetil::Projetil(){ //Cria o mapa de projeteis
  this->mapa = (char**)malloc(MAX_Y * sizeof(char*));
  for(int i=0;i<MAX_Y;i++){
    this->mapa[i] = (char*)malloc(MAX_X * sizeof(char));
  }
}

void Projetil::mapreader(){
  FILE* point;
  point = fopen("assets/gamemap.txt", "r");

  for(int i=0;i<MAX_Y;i++){
    for (int j=0;j<MAX_X;j++){
      fscanf(point,"%c",&(this->mapa[i][j]));
    }
  }

  fclose(point);
}

void Projetil::gerar(float chance){ //O mapa e gerado colocando obstaculos aleatorios nele. Um numero aleatorio e adquirido para cada posicao do mapa, com uma chance de 1% de colocar um obstaculo naquela posicao do mapa.
    // RAND_MAX
    int i = 0, j = 0;
    int alea;

    for(i=0;i<MAX_Y;i++){
        for(j=0;j<MAX_X;j++){
            alea = rand();
            if(alea >= (RAND_MAX * chance))
                this->mapa[i][j] = '#';
            else
                this->mapa[i][j] = ' ';
        }
    }
}

bool Projetil::update(std::vector<Corpo*>* corpos, int NUM_PLAYER){ //A funcao de update atualiza as posicoes do mapa e checa se os jogadores cometeram uma colisao ou nao. Caso o jogador tenha atingido um obstaculo, a funcao retorna 1 e o jogo e terminado na main.
  //mover tudo pra esquerda
  for(int i=0;i<MAX_Y;i++){
    if(mapa[i][0] == '#');
	   mapa[i][0] = ' ';
  }
  for(int i=0;i<MAX_Y;i++){
     for(int j=0;j<MAX_X-1;j++){
        if(mapa[i][j+1] == '#')
          this->mapa[i][j] = '#';
          this->mapa[i][j+1] = 'X';
        if(mapa[i][j] == 'X')
          mapa[i][j] = ' ';
      }
  }
  int dead = 0;
  Corpo* bruh;
  bruh = (*corpos)[NUM_PLAYER];
  int pos_player = bruh->get_posicao();
  if(mapa[pos_player][0] == '#'){
    bruh->kill();
    dead++;
  }
  if(dead)
    return 1;

  return 0;
}

Fisica::Fisica(ListaDeCorpos *ldc) {
  this->lista = ldc;
}

void Fisica::update(float deltaT) {
  // Atualiza parametros dos corpos!
  std::vector<Corpo *> *c = this->lista->get_corpos();

  for (int i = 0; i < (*c).size(); i++) {
    float new_vel = (*c)[i]->get_velocidade() - ((*c)[i]->get_velocidade()/3);
    float new_pos = (*c)[i]->get_posicao() + (float)deltaT * new_vel/1000;
    (*c)[i]->update(new_vel, new_pos);
  }
}

void Fisica::salto(float direcao) { //funcao nao implementada no projeto final
  // Atualiza parametros dos corpos!
  std::vector<Corpo *> *c = this->lista->get_corpos();

  for (int i = 0; i < (*c).size(); i++) {
    float new_vel = (*c)[i]->get_velocidade() + (160 * direcao);
    float new_pos = (*c)[i]->get_posicao();
      if(new_pos > 19){
        new_pos = 19;
        new_vel = 0;
      }
      if(new_pos < 0){
        new_pos = 0;
        new_vel = 0;
      }
    (*c)[i]->update(new_vel, new_pos);
  }
}

void Fisica::step(float direcao, int i) { //Move o jogador, a direcao negativa move o jogador para baixo e vice-versa
  std::vector<Corpo *> *c = this->lista->get_corpos();

  float new_pos = (*c)[i]->get_posicao() - 1*(direcao);
  if(new_pos > 19){
      new_pos = 19;
    }
  if(new_pos < 0)
    new_pos = 0;
  (*c)[i]->update(0, new_pos);
}

Tela::Tela(ListaDeCorpos *ldc, int maxI, int maxJ, float maxX, float maxY) {
  this->lista = ldc;
  this->lista_anterior = new ListaDeCorpos();
  this->lista_anterior->hard_copy(this->lista);
  this->maxI = maxI;
  this->maxJ = maxJ;
  this->maxX = maxX;
  this->maxY = maxY;
}

void Tela::init() {
  initscr();			       /* Start curses mode 		*/
	raw();				         /* Line buffering disabled	*/
  keypad(stdscr, TRUE);	 /* We get F1, F2 etc..		*/
  noecho();			         /* Don't echo() while we do getch */
  curs_set(0);           /* Do not display cursor */
}

void Tela::update(char** mapa){
  int i[NUM_PLAY], k;
  std::vector<Corpo *> *corpos_old = this->lista_anterior->get_corpos();
  // Apaga corpos na tela

  for (k=0; k<corpos_old->size(); k++){
    i[k] = (int) ((*corpos_old)[k]->get_posicao());
    move(i[k]+20, k);   /* Move cursor to position */
  	addch(' ');  /* Prints character, advances a position */
    move(0, 0);
  }

  // Desenha corpos na tela
  std::vector<Corpo *> *corpos = this->lista->get_corpos();
  char current_player;
  for (k=0; k<corpos->size(); k++){
    i[k] = (int) ((*corpos)[k]->get_posicao()) * \
        (this->maxI / this->maxX);
        //posicao atual do jogador
    if(i[k]+20 < this->maxY){
    	move(i[k]+20, 0);   /* Move cursor to position */
    }

    // Atualiza corpos antigos
    (*corpos_old)[k]->update(  (*corpos)[k]->get_velocidade(),\
                               (*corpos)[k]->get_posicao());
  }

  //Escreve o mapa no terminal do ncurses
  for (int j=0; j<20; j++) {
    for (int k=0;k<20;k++){
       move(j+20, k);
       if(mapa[j][k])
        addch(mapa[j][k]);
       else
        addch(' ');
    }
  }

  for (int j=0; j<corpos->size(); j++) {
    if( (*corpos)[j]->check_alive() ){
      move(i[j]+20, 0); //reescreve o ponto no terminal do ncurses
      current_player = (char) j;
      addch(current_player+48);
    }
  }

  refresh();
  return;
  // Atualiza tela
}

void Tela::stop() {
  endwin();
}

Tela::~Tela() {
  this->stop();
}

std::string serial(ListaDeCorpos* l, int i, bool AllowClientProceed){
  json j;
  std::vector<Corpo*>* corpos = l->get_corpos();
  Corpo* poise;
  j[0] = i;
  int x;
  for(x=1;x<corpos->size()+1;x++){
    poise = (*corpos)[x-1];
    j[x]["posicao"] = poise->get_posicao();
    j[x]["velocidade"] = poise->get_velocidade();
    j[x]["massa"] = poise->get_massa();
    j[x]["alive"] = poise->check_alive();
  }
  j[x] = AllowClientProceed;
  return j.dump();
}

void unserialize(std::string server_bodies, std::vector<Corpo*>* corpos, int* id, bool* AllowClientProceed){
  json j;
  j = json::parse(server_bodies);
  int a, b, c;
  int i;
  bool d;
  (*id) = j.at(0);
  for(i=1;i<corpos->size()+1;i++){
    a = j.at(i).at("massa");
    b = j.at(i).at("velocidade");
    c = j.at(i).at("posicao");
    d = j.at(i).at("alive");
    (*corpos)[i-1]->att_corpo(a, b, c, d);
  }
  (*AllowClientProceed) = j.at(i);
}

Teclado::Teclado() {
}

Teclado::~Teclado() {
}

void Teclado::init(int* init_game, char** mapa, int NUM_PLAYER, int this_player, ListaDeCorpos *l) {

}

void Teclado::stop() {
  this->rodando = 0;
  (this->kb_thread).join();
}

char Teclado::getchar() {
  char c = this->ultima_captura;
  return c;
}
