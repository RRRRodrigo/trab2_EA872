
#ifndef OO_MODEL_HPP
#define OO_MODEL_HPP

#include <thread>

#define MAX_Y 20
#define MAX_X 2000

class Corpo {
  private:
    float posicao;
    float velocidade;
    float massa;
    bool alive;

  public:
    Corpo(float massa, float velocidade, float posicao);
    void att_corpo(float massa, float velocidade, float posicao, bool alive);
    void update(float nova_velocidade, float nova_posicao);
    bool check_alive();
    void kill();
    float get_posicao();
    float get_velocidade();
    float get_massa();
    bool check_gameover();
};

class ListaDeCorpos {
 private:
    std::vector<Corpo*> *corpos;

  public:
    ListaDeCorpos();
    void hard_copy(ListaDeCorpos *ldc);
    void add_corpo(Corpo *c);
    std::vector<Corpo*> *get_corpos();
};

class Projetil {
    public:
    char** mapa; //linha e coluna
    void gerar(float chance);
    void mapreader();
    bool update(std::vector<Corpo*>* corpos, int NUM_PLAYER);
    Projetil();
    ~Projetil();
};

std::string serial(ListaDeCorpos* l, int i, bool AllowClientProceed);
void unserialize(std::string server_bodies, std::vector<Corpo*>* corpos, int* id, bool* AllowClientProceed);

class Fisica {
  private:
    ListaDeCorpos *lista;

  public:
    Fisica(ListaDeCorpos *ldc);
    void add_corpo(Corpo *c);
    void salto(float direcao);
    void step(float direcao, int i);
    void update(float deltaT);
};

class Tela {
  private:
    ListaDeCorpos *lista, *lista_anterior;
    int maxI, maxJ;
    float maxX, maxY;

  public:
    Tela(ListaDeCorpos *ldc, int maxI, int maxJ, float maxX, float maxY);
    ~Tela();
    ListaDeCorpos* get_lista();
    void stop();
    void init();
    void update(char** mapa);
};

class Teclado {
  private:
    char ultima_captura;
    int rodando;

    std::thread kb_thread;

  public:
    Teclado();
    ~Teclado();
    void stop();
    void init(int* init_game, char** mapa, int NUM_PLAYER, int this_player, ListaDeCorpos *l);
    char getchar();

};

#endif
