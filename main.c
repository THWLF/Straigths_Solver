#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "vec.h"
#include <stdbool.h>

uint8_t WIDTH = 0;
uint8_t HEIGHT = 0;
const uint8_t MaxNumber = 9;

typedef struct{
	bool type;
	uint8_t number;
} Field;


vec_int_t v;


Field **Level = NULL;
Field **Gamefield = NULL;


bool UniqueInRow(uint8_t row, uint8_t number){
	for(int y=0; y < HEIGHT;y++){
		if (Gamefield[row][y].number == number) return false;
	}
	return true;
}

bool UniqueInColumn(uint8_t column, uint8_t number){
	for(int x=0; x < WIDTH;x++){
		if(Gamefield[x][column].number == number) return false;
	}
	return true;
}


int comp (const void * elem1, const void * elem2)
{
    int f = *((int*)elem1);
    int s = *((int*)elem2);
    if (f > s) return  1;
    if (f < s) return -1;
    return 0;
}

//Checks if a number reached its maximum allowed
//if so it deletes it and backtraces till valid increase is found
void AlterVector(){
  bool cleaned = false;
  int toClean;
  while((cleaned == false) && (v.length >= 1)){
    toClean = vec_pop(&v);
    if(toClean < MaxNumber){
          vec_push(&v, toClean+1);
          return;
    }
  }
}

bool CheckVector(vec_int_t vecToCheck){
  int counter = 0;
  vec_sort(&vecToCheck, comp);
          for(int k=0 ; k < vecToCheck.length - 1; k++ ){
            if(vecToCheck.data[k] != vecToCheck.data[k+1] -1) {
              vec_deinit(&vecToCheck);
              return false;
            }
            counter++;
          }
          vec_clear(&vecToCheck);
          return true;
}

bool CheckNumber(int y, int x, int number){
  if(UniqueInRow(y, number) == false) return false;
  if(UniqueInColumn(x, number) == false) return false;
return true;
}


bool CheckStreets(){
  vec_int_t vecToCheck;
  vec_init(&vecToCheck);
  bool scanningStreet = false;
  //Horizontal Check
  for(int y=0;y < HEIGHT ;y++){
    vec_clear(&vecToCheck);
    for(int x=0;x < WIDTH ;x++){
      if(Gamefield[y][x].type == false) scanningStreet = true;
      while(scanningStreet == true){
        vec_push(&vecToCheck, Gamefield[y][x].number);
        x++;
        if(x >= WIDTH || Gamefield[y][x].type == true){
          scanningStreet = false;
          if(CheckVector(vecToCheck) == false) return false; else continue;
        }
        if(Gamefield[y][x].type == false && Gamefield[y][x].number == 0){
          return false;
        }
        if((x+1)*(y+1) == HEIGHT*WIDTH) scanningStreet = false;
      }
    }
  }

  //Vertical Check
  for(int x=0; x < WIDTH ;x++){
    vec_clear(&vecToCheck);
    for(int y=0; y < HEIGHT ;y++){
      if(Gamefield[y][x].type == false) scanningStreet = true;
      while(scanningStreet == true){
        vec_push(&vecToCheck, Gamefield[y][x].number);
        y++;
        if(y >= HEIGHT || Gamefield[y][x].type == true){
          scanningStreet = false;
          if(CheckVector(vecToCheck) == false) return false; else continue;
        }
        if(Gamefield[y][x].type == false && Gamefield[y][x].number == 0){
          return false;
        }
        if((x+1)*(y+1) == HEIGHT*WIDTH) scanningStreet = false;
      }
    }
  }
  vec_deinit(&vecToCheck);
  return true;
}


void ResetGamefield(){
  for(int y=0;y < HEIGHT ;y++){
    for(int x=0;x < WIDTH ;x++){
      Gamefield[y][x].number = 0;
      Gamefield[y][x].type = false;
    }
  }
}

void CleanVector(int toDelete){
  for(int i=1; i < toDelete; i++){
    vec_pop(&v);
  }
}

void FillGamefield(){
  ResetGamefield();
  // Basedata
  for(int y=0;y < HEIGHT ;y++){
    for(int x=0;x < WIDTH ;x++){
      Gamefield[y][x].number = Level[y][x].number;
      Gamefield[y][x].type = Level[y][x].type;
    }
  }
  // Alreadyfound from Vector
  int vecLength = v.length;
  int vecCount = 0;
  for(int y=0;y < HEIGHT ;y++){
    for(int x=0;x < WIDTH ;x++){
      if(Gamefield[y][x].number == 0 && Gamefield[y][x].type == false){
        if(vecCount == vecLength) return;
        if(CheckNumber(y, x, v.data[vecCount]) == false) {
        CleanVector(v.length - vecCount);
        return;
        }
          Gamefield[y][x].number = v.data[vecCount];
          vecCount++;
        }
      }
    }
    return;
}


void PrintGamefield(){
  printf("\e[1;1H\e[2J");
  printf("\t");
  for(int y=0; y < HEIGHT ;y++){
		for(int x= 0; x < WIDTH; x++){
			if(Gamefield[y][x].type == true){
			printf("\033[40;37m%d\033[0m", Gamefield[y][x].number);
			}
			else{
			printf("\033[30;47m%d\033[0m", Gamefield[y][x].number);
			}
		}
		printf("\n\t");
	}
}

void SearchNumbers(){
  for(int y=0;y < HEIGHT ;y++){
     for(int x=0; x < WIDTH ; x++){
       if(Gamefield[y][x].number == 0 && Gamefield[y][x].type == false){
         for(int number=1 ; number <= MaxNumber ; number++){
           if(CheckNumber(y, x, number) == true){
             vec_push(&v, number);
             FillGamefield();
             break;
           }
         }
         continue;
        }

       if((x+1)*(y+1) == HEIGHT*WIDTH){
           if(CheckStreets() == false) {
           AlterVector();
           FillGamefield();
           x=0;
           y=0;
           PrintGamefield();
           continue;
         }
         else
           return;
         }
      }
    }
}

void LoadLevelToSolve(char* Path){
char line[17];
FILE *file;
file = fopen(Path, "r");
if (file) {
    while (fgets(line, sizeof(line), file)) {
        WIDTH = (strlen(line)-1)/2;
        HEIGHT++;
     }
    Level = calloc(HEIGHT, sizeof(Field*));             // Size of the Pointer to the "Field"
    rewind(file);                                       // Set Readposition back to 0
    Gamefield = calloc(HEIGHT, sizeof(Field*)/2);
    for (int y = 0; fgets(line, sizeof(line), file); y++) {
       Level[y] = calloc(WIDTH, sizeof(Field));         // Size of the "Field"
       Gamefield[y] = calloc(WIDTH, sizeof(Field));
            for (int i = 0; i < strlen(line); i++){
                Level[y][i].type = line[2*i] == 'B';
                Level[y][i].number = line[2*i+1] - '0';
            }

    }
    fclose(file);
}
}



int main(int argc, char* argv[]){

if(argc > 1){
 LoadLevelToSolve("./4x4");
 FillGamefield();
 vec_int_t v;
 vec_init(&v);

 // Solve Straights
 SearchNumbers();
	//Deinitilze Vector
	vec_deinit(&v);
	// Headline
	printf("Straigths - Solution\n");
	printf("--------------------\n");
	// Print Gamefield
	PrintGamefield();
}else
  printf("Please give Path to a Str8ts Riddle");
}

