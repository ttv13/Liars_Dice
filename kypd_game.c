#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


#include <stdint.h>
#include "PmodKYPD.h"
#include "sleep.h"
#include "xparameters.h"
#include "xtime_l.h"


#define Max_Dice 5
#define Num_Player 2
#define seed 24


PmodKYPD kypd;
#define DEFAULT_KEYTABLE "0FED789C456B123A"
#define debounce_time 300000
XTime last_time = 0;
char last_key = 0;




void kypdIni() {

    KYPD_begin(&kypd, XPAR_PMODKYPD_0_AXI_LITE_GPIO_BASEADDR);
    KYPD_loadKeyTable(&kypd, (u8 *)DEFAULT_KEYTABLE);
}


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
void tally_dice (int* bid, player* players, int caller, int action);
void kypdIni();
char kypd_press();

//----Initialize a player - Iterate through loop to set all players in game----

void create_players (player* player)  {

    player->dice_num = Max_Dice;         //Set player dice number to max dice
    for (int i = 0; i < Max_Dice; i++){
        player->dice[i] = 0;             //Set player dice values to 0 Ready to roll for round 1
    }

}

//----Display player information (dice values and number of dice)----

void display_player (player* player) {
    
    xil_printf("Dice num: %d\r\n  Dice values: ", player->dice_num);       //Print Number of player dice
    for (int i = 0; i < player->dice_num; i++){
        xil_printf("%d ", player->dice[i]);                              //Print dice values
    }
    xil_printf("\r\n");

}

//----Roll player dice from 1-6----

void roll_dice (player* player) {
    
    for(int i = 0; i < player->dice_num; i++){
        player->dice[i] = rand() % 6 + 1;       //Roll player dice from 1-6 
    }

}

void action_bid (int* bid) {

    xil_printf("Enter bid quantity and dice face (e.g., 3 1 for 3 ones): \r\n");
    int key_num = 0;
    int new_face = 0;
    int new_quantity = 0;
    while (1){

        char bid_key = kypd_press();

        if (bid_key != 0) {
            XTime bid_time;
            XTime_GetTime(&bid_time);
            u64 between_bid_time = bid_time - last_time;
            
            if(between_bid_time > ((XTime) debounce_time * COUNTS_PER_SECOND / 1000000)&& bid_key != last_key) {

                xil_printf("You pressed : %c\r\n ", bid_key);
                last_key = bid_key;
                last_time = bid_time;

                xil_printf("key num is %d \r\n", key_num);

                if(key_num == 0){
                    xil_printf("You have entered new quantity %c\r\n ", bid_key);
                    new_quantity = bid_key - '0';
                    key_num++;
                    xil_printf("new quanity value is %d \r\n ", new_quantity);
                    continue;
                } else if(key_num == 1){
                    xil_printf("You have entered new face %c\r\n ", bid_key);

                    new_face = bid_key - '0';
                    key_num = 0;
                    xil_printf("new face value is %d \r\n ", new_face);

                }

                if ((new_quantity <= bid[0] && new_face <= bid[1]) || (new_face > 6 || new_face < 1))
                { // Check If the new bid is valid (if the new bid is smaller than old bid and if the new face is less than 6 or 1)

                    xil_printf("Invalid bid. Please try again. last bid was %d %d \r\n ", bid[0], bid[1]);
                    new_quantity = 0;
                    new_face = 0;
                    continue;
                }else {
                    bid[0] = new_quantity; // Update the bid array
                    bid[1] = new_face;
                    xil_printf("\r\n\r\n\r\nNew bid: %d %d \r\n", bid[0], bid[1]);
                    break;
                }
            }
        }else {
            last_key = 0;
        }

        usleep(1000);

    }
}

void tally_dice (int* bid, player* players, int caller, int action) {             //Tally dice for spot on and call bluff (action = 0 for spot on and 1 for call bluff)

    int tally = 0;


    for (int i = 0; i < Num_Player; i++){           // iterate through all of the players and cound number of a specific face value

        for (int j = 0; j < players[i].dice_num; j++){
            
            if (players[i].dice[j] == bid[1]){

                tally++;

            }
        }
        
    }

    if (action == 0){               //if action was spot on 

        if(tally == bid[0]){        //If caller was right

            xil_printf("Player %d was spot on ! Everyone else loses a dice ! \r\n", caller + 1);

            for(int i = 0; i < Num_Player; i++){            //Iterate through all of the players to discard dice except for caller

                if(i == caller){
                    continue;
                }
                players[i].dice_num = players[i].dice_num - 1;
                xil_printf("Player %d is now at %d dice \r\n ", i + 1, players[i].dice_num);

            }
        }else {                 //Caller is wrong
            xil_printf("Player %d was wrong! The bid was not spot on \r\n ", caller + 1);
            players[caller].dice_num = players[caller].dice_num - 1;            //Discard wrong caller dice
            xil_printf("Player %d is now at %d dice \r\n ", caller + 1, players[caller].dice_num);

        }


    } else {                    // if action was call bluff 

        if(tally < bid[0]){        //if caller was right

            xil_printf("Player %d was right ! Everyone else was bluffing ! \r\n ", caller + 1);

            for (int i = 0; i < Num_Player; i++){           //Iterate through all of the players to discard dice except for caller
                if (i == caller)
                {
                    continue;
                }
                players[i].dice_num = players[i].dice_num - 1;
                xil_printf("Player %d is now at %d dice \r\n", i + 1, players[i].dice_num);


            }
        } else {        //if caller was wrong
            xil_printf("Player %d was wrong! The bid was not a bluff! \r\n", caller + 1);
            players[caller].dice_num = players[caller].dice_num - 1;            //Discard wrong caller dice
            xil_printf("Player %d is now at %d dice \r\n", caller + 1, players[caller].dice_num);

        }

    }
}

char kypd_press(){

    u16 keystate;
    XStatus status;
    u8 key;
    keystate = KYPD_getKeyStates(&kypd);
    status = KYPD_getKeyPressed(&kypd, keystate, &key);

    if (status == KYPD_SINGLE_KEY) {
        return key;  // Return the raw key value
    }else {
        return 0;
    }
    
}

int main ()
{
    kypdIni();
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

    int end_game_flag = 0;
    int end_round_flag = 0;
    while (1){          //Preround Loop
    
        for(int i = 0; i < Num_Player; i++){
            if(players[i].dice_num == 0){
                xil_printf("player %d has ran out of dice ! They lose! \r\n", i+1);
                end_game_flag = 1;
            }
        }

        if(end_game_flag){
            break;
        }


        xil_printf("Round: %d \r\n", round);
        for (int i = 0; i < Num_Player; i++){
            roll_dice(&players[i]);               //Roll every player's dice
        }

        memset(bid, 0, sizeof(bid));                  //Clear the bid array
        

        while (1){          //Round Loop

            for (int i = 0; i < Num_Player; i++){           //Iterate through players

                xil_printf("Player %d's turn: Press one to start turn \r\n", i + 1);

                while(1){//start turn loop

                    char start = kypd_press();
                    
                    if (start != 0){
                        XTime starttime;
                        XTime_GetTime(&starttime);
                        u64 between_key_time = starttime - last_time;
                        if(between_key_time > ((XTime)debounce_time * COUNTS_PER_SECOND / 1000000) && start != last_key){
                            xil_printf("You pressed : %c\r\n ", start);
                            last_key = start;
                            last_time = starttime;
                            if(start == '1'){
                                xil_printf("Starting turn \r\n");
                                break;
                            }else {
                                xil_printf("Not starting turn \r\n");
                            }
                        }
                    }else {
                        last_key = 0;
                    }

                    usleep(10000);
                }

                display_player(&players[i]);

                if (bid[1] == 0){           //If this is the first bid only action is to bid
                
                    action_bid(bid);
                } else {

                    xil_printf("Actions : New bid | Spot On | Call Bluff - 1 , 2 ,3 \r\n");

                    // Action Selection
                    while (1)
                    {

                        char action = kypd_press();

                        if (action != 0){
                            XTime actiontime;
                            XTime_GetTime(&actiontime);
                            u64 between_key_time_action = actiontime - last_time;
                            if(between_key_time_action > ((XTime)debounce_time * COUNTS_PER_SECOND / 1000000) && action != last_key){
                                xil_printf("You pressed : %c\r\n ", action);
                                last_key = action;
                                last_time = actiontime;

                                switch (action)
                                {

                                case '1':
                                    action_bid(bid);
                                    break;

                                case '2':
                                    tally_dice(bid, players, i, 0);
                                    end_round_flag = 1;
                                    break;

                                case '3':
                                    tally_dice(bid, players, i, 1);
                                    end_round_flag = 1;
                                    break;
                                default:
                                    xil_printf("Invalid action. Please try again. \r\n");
                                    continue;
                                }

                                break; // Break out of action selection once an action is selected
                            }
                        }else {
                            last_key = 0;
                        }

                        usleep(10000);

                    }//action selection while
                }

                if (end_round_flag){
                    break;
                }

            }//for player loop

            if (end_round_flag){
                round++;
                end_round_flag = 0;
                break;
            }

        }//round loop

    }//preround loop
    
    

    return 0;
}//end main 