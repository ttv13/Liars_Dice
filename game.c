#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define Max_Dice 5
#define Num_Player 2
#define seed 24

//----Struct for player specific information (dice values and number of dice)----

typedef struct player
{
    int dice[Max_Dice];         //array to hold dice values
    int dice_num;               // number of dice

}player;

//----Function Prototypes----
void Create_players (player* p);
void Display_player (player* p);
void roll_dice (player* p);

//---------------------------

//----Initialize a player - Iterate through loop to set all players in game----

void Create_players (player* player)  {

    player->dice_num = Max_Dice;         //Set player dice number to max dice
    for (int i = 0; i < Max_Dice; i++){
        player->dice[i] = 0;             //Set player dice values to 0 Ready to roll for round 1
    }

}

//----Display player information (dice values and number of dice)----

void Display_player (player* player) {
    
    printf("Dice num: %d\n  Dice values: ", player->dice_num);       //Print Number of player dice
    for (int i = 0; i < player->dice_num; i++){
        printf("%d ", player->dice[i]);                              //Print dice values
    }
    printf("\n");

}

//----Roll player dice from 1-6----

void roll_dice (player* player) {
    
    for(int i = 0; i < player->dice_num; i++){
        player->dice[i] = rand() % 6 + 1;       //Roll player dice from 1-6 
    }

}

int main ()
{

    //Seed the rand so that each run is random
    srand(seed);
    // srand(time(0));

    //Player initialization
    player players [Num_Player];
    for (int i = 0; i < Num_Player; i++){
        Create_players(&players[i]);
    }

    int round = 1;
    int bid [1];  // the bid for each round made by the last player - {quantity, face value}
    while (1){                  
        
        //Round setup
        printf("Round: %d\n", round);

        for (int i = 0; i < Num_Player; i++){
            roll_dice(&players[i]);               //Roll every player's dice
        }

        memset(bid, 0, sizeof(bid));                  //Clear the bid array
        

        //Round play
        for (int i = 0; i < Num_Player; i++){

            printf("Player %d's turn\n ", i+1);
            Display_player(&players[i]);


            // printf("Enter bid quantity and dice face (e.g., 3 1 for 3 ones): ");
            // scanf("%d %d", &bid[0], &bid[1]);
            // printf("\n");
        }

        break;
    }
    
    

    return 0;
}