#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_DICE 5
#define NUM_PLAYERS 2

// Function prototypes
void rollDice(int dice[], int numDice);
void printDice(int dice[], int numDice);
int countDiceFace(int dice[], int numDice, int faceValue);
int isValidBid(int currentBid[], int newBid[], int numDice);
void playTurn(int player, int dice[], int numDice);
void callBluff(int player, int opponentDice[], int numDice, int bid[], int isSpotOn);
void spotOn(int player, int opponentDice[], int numDice, int bid[]);
int allDiceDiscarded(int dice[], int numDice);

int main() {
    srand(time(0));

    int player1Dice[MAX_DICE] = {0};
    int player2Dice[MAX_DICE] = {0};
    int player1Bid[2] = {0}; // [quantity, face value]
    int player2Bid[2] = {0}; // [quantity, face value]
    
    int round = 1;
    while (1) {
        printf("Round %d\n", round);

        // Player 1 turn
        playTurn(1, player1Dice, MAX_DICE);
        printf("Player 1's dice: ");
        printDice(player1Dice, MAX_DICE);
        
        // Player 2 turn
        playTurn(2, player2Dice, MAX_DICE);
        printf("Player 2's dice: ");
        printDice(player2Dice, MAX_DICE);

        // Handle bid comparison and bluff/spot on
        printf("Player 1 makes a bid.\n");
        printf("Enter bid quantity and dice face (e.g., 3 1 for 3 ones): ");
        scanf("%d %d", &player1Bid[0], &player1Bid[1]);
        printf("Player 2, do you call bluff (1 for yes, 0 for no)? ");
        int callBluffChoice;
        scanf("%d", &callBluffChoice);
        if (callBluffChoice) {
            callBluff(2, player1Dice, MAX_DICE, player1Bid, 0);
        }
        
        printf("Player 2 makes a bid.\n");
        printf("Enter bid quantity and dice face (e.g., 3 1 for 3 ones): ");
        scanf("%d %d", &player2Bid[0], &player2Bid[1]);
        printf("Player 1, do you call bluff (1 for yes, 0 for no)? ");
        scanf("%d", &callBluffChoice);
        if (callBluffChoice) {
            callBluff(1, player2Dice, MAX_DICE, player2Bid, 0);
        }
        
        if (allDiceDiscarded(player1Dice, MAX_DICE)) {
            printf("Player 2 wins!\n");
            break;
        }
        if (allDiceDiscarded(player2Dice, MAX_DICE)) {
            printf("Player 1 wins!\n");
            break;
        }

        round++;
    }
    return 0;
}

// Rolls a set of dice
void rollDice(int dice[], int numDice) {
    for (int i = 0; i < numDice; i++) {
        dice[i] = rand() % 6 + 1;  // Random dice roll between 1 and 6
    }
}

// Prints the dice
void printDice(int dice[], int numDice) {
    for (int i = 0; i < numDice; i++) {
        printf("%d ", dice[i]);
    }
    printf("\n");
}

// Counts how many times a specific dice face value occurs
int countDiceFace(int dice[], int numDice, int faceValue) {
    int count = 0;
    for (int i = 0; i < numDice; i++) {
        if (dice[i] == faceValue) {
            count++;
        }
    }
    return count;
}

// Validates whether a new bid is higher in quantity or face value
int isValidBid(int currentBid[], int newBid[], int numDice) {
    if (newBid[0] > currentBid[0]) {
        return 1;
    } else if (newBid[0] == currentBid[0] && newBid[1] > currentBid[1]) {
        return 1;
    }
    return 0;
}

// Plays a turn for a player
void playTurn(int player, int dice[], int numDice) {
    printf("Player %d is rolling their dice...\n", player);
    rollDice(dice, numDice);
    printDice(dice, numDice);
}

// Handles the bluff call
void callBluff(int player, int opponentDice[], int numDice, int bid[], int isSpotOn) {
    int actualCount = countDiceFace(opponentDice, numDice, bid[1]);
    if (actualCount >= bid[0]) {
        printf("Player %d's bid was correct!\n", player);
    } else {
        printf("Player %d's bid was incorrect.\n", player);
    }
}

// Handles the spot on condition
void spotOn(int player, int opponentDice[], int numDice, int bid[]) {
    int actualCount = countDiceFace(opponentDice, numDice, bid[1]);
    if (actualCount == bid[0]) {
        printf("Spot on! Player %d gets a penalty.\n", player);
    } else {
        printf("Incorrect! Player %d gets a penalty.\n", player);
    }
}

// Checks if all dice are discarded
int allDiceDiscarded(int dice[], int numDice) {
    for (int i = 0; i < numDice; i++) {
        if (dice[i] != 0) {
            return 0;
        }
    }
    return 1;
}
