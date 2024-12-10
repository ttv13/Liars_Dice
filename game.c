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
void create_players (player* p);
void display_player (player* p);
void roll_dice (player* p);
void action_bid (int* bid);
//---------------------------

//----Initialize a player - Iterate through loop to set all players in game----

void create_players (player* player)  {

    player->dice_num = Max_Dice;         //Set player dice number to max dice
    for (int i = 0; i < Max_Dice; i++){
        player->dice[i] = 0;             //Set player dice values to 0 Ready to roll for round 1
    }

}

//----Display player information (dice values and number of dice)----

void display_player (player* player) {
    
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

void action_bid (int* bid) {
    while (1){

        int new_face = 0;
        int new_quantity = 0;

        printf("Enter bid quantity and dice face (e.g., 3 1 for 3 ones): \n");
        scanf("%d %d", &new_quantity, &new_face);


        if (new_quantity < bid[0] && new_face < bid[1]){                //Check If the new bid is valid

            printf("Invalid bid. Please try again.\n");
            continue;
        }

        bid[0] = new_quantity;          //Update the bid array
        bid[1] = new_face;
        printf("New bid: %d %d\n", bid[0], bid[1]);
        break;
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
        create_players(&players[i]);
    }

    int round = 1;
    int bid [2];  // the bid for each round made by the last player - {quantity, face value}

    while (1){          //Preround Loop
    
        printf("Round: %d\n", round);
        for (int i = 0; i < Num_Player; i++){
            roll_dice(&players[i]);               //Roll every player's dice
        }

        memset(bid, 0, sizeof(bid));                  //Clear the bid array
        

        while (1){          //Round Loop

            for (int i = 0; i < Num_Player; i++){           //Iterate through players

                printf("Player %d's turn: \n", i + 1);
                display_player(&players[i]);

                if (bid[1] == 0){           //If this is the first bid only action is to bid
                
                    action_bid(bid);
                }
            }



            break;
        }

        break;
    }
    
    

    return 0;
}