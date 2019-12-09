#include <iostream>
#include <thread>
#include <vector>
#include "oo_model.hpp"

int main(){
  FILE* point;
  point = fopen("assets/gamemap.txt", "w");

  Projetil *projetil = new Projetil();
  projetil->gerar(0.98);

  for(int i=0;i<MAX_Y;i++){
    for (int j=0;j<MAX_X;j++){
      fprintf(point,"%c",projetil->mapa[i][j]);
    }
  }
  for(int i=0;i<MAX_Y;i++){
    for (int j=0;j<MAX_X;j++){
      fprintf(point,"%c",projetil->mapa[i][j]);
    }
  }
  fclose(point);
}
