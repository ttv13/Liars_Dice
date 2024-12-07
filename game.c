#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define Max_Dice 5
#define Num_Player 2


//----player Struct for player specific information----

typedef struct player
{
    int dice[Max_Dice];         //array to hold dice values
    int dice_num;               // number of dice

}player;


//----Initialize a player - Iterate through loop to set all players in game----

void Create_players (player* p)  {

    p->dice_num = Max_Dice;         //Set player dice number to max dice
    for (int i = 0; i < Max_Dice; i++){
        p->dice[i] = 0;             //Set player dice values to 0 Ready to roll for round 1
    }

}

//----Display player information----

void Display_players (player* p) {
    
    printf("Dice num: %d\n  Dice values: ", p->dice_num);       //Print Number of player dice
    for (int i = 0; i < p->dice_num; i++){
        printf("%d ", p->dice[i]);                              //Print dice values
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