#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define Max_Dice 5
#define Num_Player 2

//----Function Prototypes----
void Create_players (player* p);
void Display_players (player* p);
void roll_dice (player* p);

//----player Struct for player specific information----

typedef struct player
{
    int dice[Max_Dice];         //array to hold dice values
    int dice_num;               // number of dice

}player;

//----Initialize a player - Iterate through loop to set all players in game----

void Create_players (player* player)  {

    player->dice_num = Max_Dice;         //Set player dice number to max dice
    for (int i = 0; i < Max_Dice; i++){
        player->dice[i] = 0;             //Set player dice values to 0 Ready to roll for round 1
    }

}

//----Display player information----

void Display_players (player* player) {
    
    printf("Dice num: %d\n  Dice values: ", player->dice_num);       //Print Number of player dice
    for (int i = 0; i < player->dice_num; i++){
        printf("%d ", player->dice[i]);                              //Print dice values
    }
    printf("\n");

}

//----Roll player dice----

void roll_dice (player* player) {
    
    for(int i = 0; i < player->dice_num; i++){
        player->dice[i] = rand() % 6 + 1;       //Roll player dice from 1-6 
    }

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