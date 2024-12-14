#include "PmodKYPD.h"
#include "sleep.h"
#include "xil_cache.h"
#include "xparameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define Max_Dice 5
#define Num_Player 2
#define seed 24

//----Struct for player specific information (dice values and number of dice)----
typedef struct player {
    int dice[Max_Dice];         // array to hold dice values
    int dice_num;               // number of dice
} player;

//----Function Prototypes----
void create_players(player* p);
void display_player(player* p);
void roll_dice(player* p);
void action_bid(int* bid);
void tally_dice(int* bid, player* players, int caller, int action);
//---------------------------

//----Initialize a player - Iterate through loop to set all players in game----
void create_players(player* player) {
    player->dice_num = Max_Dice;         // Set player dice number to max dice
    for (int i = 0; i < Max_Dice; i++) {
        player->dice[i] = 0;             // Set player dice values to 0 Ready to roll for round 1
    }
}

//----Display player information (dice values and number of dice)----
void display_player(player* player) {
    printf("Dice num: %d\n  Dice values: ", player->dice_num);       // Print Number of player dice
    for (int i = 0; i < player->dice_num; i++) {
        printf("%d ", player->dice[i]);                              // Print dice values
    }
    printf("\n");
}

//----Roll player dice from 1-6----
void roll_dice(player* player) {
    for (int i = 0; i < player->dice_num; i++) {
        player->dice[i] = rand() % 6 + 1;       // Roll player dice from 1-6 
    }
}

void action_bid(int* bid) {
    while (1) {
        int new_face = 0;
        int new_quantity = 0;

        xil_printf("Enter bid quantity and dice face (e.g., 3 1 for 3 ones):\r\n");

        // Get the first number (quantity)
        while (1) {
            u8 key;
            XStatus status = KYPD_getKeyPressed(&myDevice, keystate, &key);
            if (status == KYPD_SINGLE_KEY && key != 'x') {   // ensure a valid key was pressed
                new_quantity = key - '0';   // Convert the character to an integer
                xil_printf("You entered quantity: %d\r\n", new_quantity);
                break;
            }
        }

        // Get the second number (dice face)
        while (1) {
            u8 key;
            XStatus status = KYPD_getKeyPressed(&myDevice, keystate, &key);
            if (status == KYPD_SINGLE_KEY && key != 'x') {
                new_face = key - '0';   // Convert the character to an integer
                xil_printf("You entered face value: %d\r\n", new_face);
                break;
            }
        }

        if ((new_quantity <= bid[0] && new_face <= bid[1]) || (new_face > 6 || new_face < 1)) { 
            xil_printf("Invalid bid. Please try again.\r\n");
            continue;
        }

        bid[0] = new_quantity;          // Update the bid array
        bid[1] = new_face;
        xil_printf("New bid: %d %d\r\n", bid[0], bid[1]);
        break;
    }
}

void tally_dice(int* bid, player* players, int caller, int action) { // Tally dice for spot on and call bluff
    int tally = 0;

    for (int i = 0; i < Num_Player; i++) {
        for (int j = 0; j < players[i].dice_num; j++) {
            if (players[i].dice[j] == bid[1]) {
                tally++;
            }
        }
    }

    if (action == 0) {
        if (tally == bid[0]) {
            xil_printf("Player %d was spot on! Everyone else loses a dice!\r\n", caller + 1);
            for (int i = 0; i < Num_Player; i++) {
                if (i == caller) {
                    continue;
                }
                players[i].dice_num--;
                xil_printf("Player %d is now at %d dice\r\n", i + 1, players[i].dice_num);
            }
        } else {
            xil_printf("Player %d was wrong! The bid was not spot on\r\n", caller + 1);
            players[caller].dice_num--;
            xil_printf("Player %d is now at %d dice\r\n", caller + 1, players[caller].dice_num);
        }
    } else {
        if (tally < bid[0]) {
            xil_printf("Player %d was right! Everyone else was bluffing!\r\n", caller + 1);
            for (int i = 0; i < Num_Player; i++) {
                if (i == caller) {
                    continue;
                }
                players[i].dice_num--;
                xil_printf("Player %d is now at %d dice\r\n", i + 1, players[i].dice_num);
            }
        } else {
            xil_printf("Player %d was wrong! The bid was not a bluff!\r\n", caller + 1);
            players[caller].dice_num--;
            xil_printf("Player %d is now at %d dice\r\n", caller + 1, players[caller].dice_num);
        }
    }
}

int main() {
    srand(seed);

    player players[Num_Player];
    for (int i = 0; i < Num_Player; i++) {
        create_players(&players[i]);
    }

    int round = 1;
    int bid[2];  // The bid for each round made by the last player - {quantity, face value}
    int action = 0;
    int end_game_flag = 0;
    int end_round_flag = 0;
    int start;

    while (1) {
        for (int i = 0; i < Num_Player; i++) {
            if (players[i].dice_num == 0) {
                xil_printf("Player %d has run out of dice! They lose!\r\n", i + 1);
                end_game_flag = 1;
            }
        }

        if (end_game_flag) {
            break;
        }

        xil_printf("Round: %d\r\n", round);
        for (int i = 0; i < Num_Player; i++) {
            roll_dice(&players[i]);
        }

        memset(bid, 0, sizeof(bid));  // Clear the bid array

        while (1) {
            for (int i = 0; i < Num_Player; i++) {
                while (1) {
                    xil_printf("Player %d's turn: Ready to start turn? Press 1 to start.\r\n", i + 1);
                    u8 key;
                    XStatus status = KYPD_getKeyPressed(&myDevice, keystate, &key);
                    if (status == KYPD_SINGLE_KEY && key == '1') {
                        break;
                    }
                }

                display_player(&players[i]);

                if (bid[1] == 0) {
                    action_bid(bid);
                } else {
                    while (1) {
                        xil_printf("Actions: New bid (1) | Spot On (2) | Call Bluff (3)\r\n");
                        u8 key;
                        XStatus status = KYPD_getKeyPressed(&myDevice, keystate, &key);
                        if (status == KYPD_SINGLE_KEY && (key >= '1' && key <= '3')) {
                            action = key - '0';
                            break;
                        }
                    }

                    switch (action) {
                        case 1:
                            action_bid(bid);
                            break;
                        case 2:
                            tally_dice(bid, players, i, 0);
                            end_round_flag = 1;
                            break;
                        case 3:
                            tally_dice(bid, players, i, 1);
                            end_round_flag = 1;
                            break;
                    }
                }

                if (end_round_flag) {
                    break;
                }
            }

            if (end_round_flag) {
                round++;
                end_round_flag = 0;
                break;
            }
        }
    }

    return 0;
}
