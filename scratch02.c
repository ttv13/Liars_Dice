#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_DICE 5
#define NUM_PLAYERS 2

// Define a structure for Player
typedef struct {
    int dice[MAX_DICE];        // Array to hold the player's dice
    int bid[2];                // Array for the player's bid [quantity, face value]
    int numDice;               // Number of dice the player has
} Player;

// Function prototypes
void rollDice(Player *player);
void printDice(Player *player);
int countDiceFace(Player *player, int faceValue);
int isValidBid(Player *currentPlayer, Player *opponentPlayer);
void playTurn(Player *player);
void callBluff(Player *caller, Player *opponent, int isSpotOn);
void spotOn(Player *caller, Player *opponent);
int allDiceDiscarded(Player *player);

int main() {
    srand(time(0));

    // Initialize players
    Player player1 = {{0}, {0, 0}, MAX_DICE};
    Player player2 = {{0}, {0, 0}, MAX_DICE};

    int round = 1;
    while (1) {
        printf("Round %d\n", round);

        // Player 1 turn
        playTurn(&player1);
        printf("Player 1's dice: ");
        printDice(&player1);

        // Player 2 turn
        playTurn(&player2);
        printf("Player 2's dice: ");
        printDice(&player2);

        // Handle bid comparison and bluff/spot on
        printf("Player 1 makes a bid.\n");
        printf("Enter bid quantity and dice face (e.g., 3 1 for 3 ones): ");
        scanf("%d %d", &player1.bid[0], &player1.bid[1]);
        printf("Player 2, do you call bluff (1 for yes, 0 for no)? ");
        int callBluffChoice;
        scanf("%d", &callBluffChoice);
        if (callBluffChoice) {
            callBluff(&player2, &player1, 0);  // Not Spot On, just call bluff
        }

        printf("Player 2 makes a bid.\n");
        printf("Enter bid quantity and dice face (e.g., 3 1 for 3 ones): ");
        scanf("%d %d", &player2.bid[0], &player2.bid[1]);
        printf("Player 1, do you call bluff (1 for yes, 0 for no)? ");
        scanf("%d", &callBluffChoice);
        if (callBluffChoice) {
            callBluff(&player1, &player2, 0);  // Not Spot On, just call bluff
        }

        if (allDiceDiscarded(&player1)) {
            printf("Player 2 wins!\n");
            break;
        }
        if (allDiceDiscarded(&player2)) {
            printf("Player 1 wins!\n");
            break;
        }

        round++;
    }
    return 0;
}

// Rolls a player's dice
void rollDice(Player *player) {
    for (int i = 0; i < player->numDice; i++) {
        player->dice[i] = rand() % 6 + 1;  // Random dice roll between 1 and 6
    }
}

// Prints the player's dice
void printDice(Player *player) {
    for (int i = 0; i < player->numDice; i++) {
        printf("%d ", player->dice[i]);
    }
    printf("\n");
}

// Counts how many times a specific dice face value occurs for a player
int countDiceFace(Player *player, int faceValue) {
    int count = 0;
    for (int i = 0; i < player->numDice; i++) {
        if (player->dice[i] == faceValue) {
            count++;
        }
    }
    return count;
}

// Validates whether a new bid is higher in quantity or face value compared to the last bid
int isValidBid(Player *currentPlayer, Player *opponentPlayer) {
    if (currentPlayer->bid[0] > opponentPlayer->bid[0]) {
        return 1;
    } else if (currentPlayer->bid[0] == opponentPlayer->bid[0] && currentPlayer->bid[1] > opponentPlayer->bid[1]) {
        return 1;
    }
    return 0;
}

// Plays a turn for a player (rolls dice and prints them)
void playTurn(Player *player) {
    printf("Player is rolling their dice...\n");
    rollDice(player);
    printDice(player);
}

// Handles the bluff call
void callBluff(Player *caller, Player *opponent, int isSpotOn) {
    int actualCount = countDiceFace(opponent, opponent->bid[1]);
    if (actualCount >= opponent->bid[0]) {
        printf("Player's bid was correct!\n");
    } else {
        printf("Player's bid was incorrect.\n");
    }
}

// Handles the spot on condition
void spotOn(Player *caller, Player *opponent) {
    int actualCount = countDiceFace(opponent, opponent->bid[1]);
    if (actualCount == opponent->bid[0]) {
        printf("Spot on! Opponent gets a penalty.\n");
    } else {
        printf("Incorrect! You get a penalty.\n");
    }
}

// Checks if all dice are discarded (i.e., a player has no dice left)
int allDiceDiscarded(Player *player) {
    for (int i = 0; i < player->numDice; i++) {
        if (player->dice[i] != 0) {
            return 0;
        }
    }
    return 1;
}
