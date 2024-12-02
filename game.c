#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define Max_Dice 5
#define Num_Player 2


//player Struct 
typedef struct player
{
    int dice[Max_Dice]; //array to hold dice values
    int dice_num; // number of dice
}player;

void Create_players (player* p)  {
    p->dice_num = Max_Dice;
    for (int i = 0; i < Max_Dice; i++){
        p->dice[i] = 0;
    }
}

void Display_players (player* p) {
    
    printf("Dice num: %d\n  Dice values: ", p->dice_num);
    for (int i = 0; i < p->dice_num; i++){
        printf("%d ", p->dice[i]);
    }
    printf("\n");
    
}

int main ()
{

    //Player information and initialization
    player players [Num_Player];
    for (int i = 0; i < Num_Player; i++){
        Create_players(&players[i]);
    }

    
    

    return 0;
}