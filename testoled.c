#include "xparameters.h"
#include "xil_types.h"
#include "sleep.h"
#include "PmodOLED.h"
#include "PmodKYPD.h"
#include <stdio.h>
#include <stdlib.h>
#include "xtime_l.h"
#include <sys/time.h>

PmodOLED oled;
PmodKYPD kypd;

// Game variables
#define SEQUENCE_LENGTH 3
char sequence[SEQUENCE_LENGTH];
char userInput[SEQUENCE_LENGTH];
int currentInputIndex = 0;
int gameState = 0; // 0: Start, 1: Display sequence, 2: Wait for input, 3: Show result

// Scoring variables
int score = 0; // Score for correct sequences

// Debounce variables
#define DEBOUNCE_DELAY 300000 // 300ms in microseconds
XTime lastKeyPressTime = 0;
char lastKeyPressed = 0;

#define DEFAULT_KEYTABLE "0FED789C456B123A"

// Function Prototypes
void InitializeOLED();
void InitializeKeypad();
void HandleKeyPress(char key);
void UpdateDisplay();
void GenerateSequence();
void DisplaySequence();
void CheckUserInput();
void ResetGame();
char ScanKeypad();

void InitializeOLED() {
    OLED_Begin(&oled, XPAR_PMODOLED_0_AXI_LITE_GPIO_BASEADDR,
               XPAR_PMODOLED_0_AXI_LITE_SPI_BASEADDR, 0x0, 0x0);
    OLED_ClearBuffer(&oled);
    OLED_Update(&oled);
}

void InitializeKeypad() {
    KYPD_begin(&kypd, XPAR_PMODKYPD_0_AXI_LITE_GPIO_BASEADDR);
    KYPD_loadKeyTable(&kypd, (u8*) DEFAULT_KEYTABLE);
}

int main() {
    InitializeOLED();
    InitializeKeypad();

    XTime seed;
    XTime_GetTime(&seed);
    srand((unsigned int)seed);

    // Initialize game and display start message
    OLED_ClearBuffer(&oled);
    OLED_SetCursor(&oled, 0, 0);
    OLED_PutString(&oled, "Memory Game Initialized");
    OLED_Update(&oled);
    usleep(1000000); // Display the start message for 1 second

    ResetGame();

    while (1) {
        char key = ScanKeypad();
        if (key != 0) {
            XTime currentTime;
            XTime_GetTime(&currentTime);
            if ((currentTime - lastKeyPressTime) > ((XTime)DEBOUNCE_DELAY * COUNTS_PER_SECOND / 1000000) && key != lastKeyPressed) {
                HandleKeyPress(key);
                lastKeyPressTime = currentTime;
                lastKeyPressed = key;
            }
        } else {
            lastKeyPressed = 0; // Reset last key pressed when no key is pressed
        }

        UpdateDisplay();
        usleep(10000); // 10ms delay
    }

    return 0;
}

char ScanKeypad() {
    u16 keystate;
    XStatus status;
    u8 key;

    keystate = KYPD_getKeyStates(&kypd);
    status = KYPD_getKeyPressed(&kypd, keystate, &key);

    if (status == KYPD_SINGLE_KEY) {
        return key;  // Return the raw key value
    }
    return 0;
}

void HandleKeyPress(char key) {
    if (key == 'C') {
        ResetGame();
        return;
    }

    if (gameState == 2 && currentInputIndex < SEQUENCE_LENGTH) {
        // Only accept inputs from '0'-'9' and 'A'-'F'
        if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'F')) {
            userInput[currentInputIndex] = key; // Store the character directly
            currentInputIndex++;
            UpdateDisplay(); // Update display immediately after input

            if (currentInputIndex == SEQUENCE_LENGTH) {
                CheckUserInput(); // Check the user's input after they finish entering it
            }
        }
    }
}

void UpdateDisplay() {
    OLED_ClearBuffer(&oled);
    char buffer[16];

    switch (gameState) {
        case 0:
            OLED_SetCursor(&oled, 0, 0);
            OLED_PutString(&oled, "Memory Game");
            OLED_SetCursor(&oled, 0, 2);
            OLED_PutString(&oled, "Press C to start");
            break;
        case 1:
            OLED_SetCursor(&oled, 0, 0);
            OLED_PutString(&oled, "Watch sequence");
            break;
        case 2:
            OLED_SetCursor(&oled, 0, 0);
            OLED_PutString(&oled, "Enter sequence:");
            OLED_SetCursor(&oled, 0, 2);
            for (int i = 0; i < SEQUENCE_LENGTH; i++) {
                if (i < currentInputIndex) {
                    sprintf(buffer, "%c", userInput[i]);
                } else {
                    sprintf(buffer, "_");
                }
                OLED_PutString(&oled, buffer);
                OLED_PutString(&oled, " ");
            }
            break;
        case 3:
            OLED_SetCursor(&oled, 0, 0);
            OLED_PutString(&oled, "Game Over");
            OLED_SetCursor(&oled, 1, 2); // Move to next line for score display
            sprintf(buffer, "Score: %d", score); // Show score on display
            OLED_PutString(&oled, buffer);
            OLED_SetCursor(&oled, 2, 2); // Move to next line for restart instruction
            OLED_PutString(&oled, "Press C to restart");
            break;
    }

    OLED_Update(&oled);
}

void GenerateSequence() {
	// Only include numbers and letters A-F in valid characters
	const char validChars[] = "0123456789ABCDEF";
	for (int i = 0; i < SEQUENCE_LENGTH; i++) {
		sequence[i] = validChars[rand() % sizeof(validChars) - 1];
	}
}

void DisplaySequence() {
	for (int i = 0; i < SEQUENCE_LENGTH; i++) {
		OLED_ClearBuffer(&oled);
		OLED_SetCursor(&oled, 0, 1);
		char buffer[2] = {sequence[i], '\0'};
		OLED_PutString(&oled, buffer);
		OLED_Update(&oled);
		sleep(1); // Display each character for 1 second
		OLED_ClearBuffer(&oled);
		OLED_Update(&oled);
		usleep(250000); // Pause between characters
	}

	// After displaying the sequence fully,
	// Set the game state to wait for user input.
	gameState = 2;
}

void CheckUserInput() {
	int correct = 1;

	// Check if user input matches the generated sequence.
	for (int i = 0; i < SEQUENCE_LENGTH; i++) {
		if (sequence[i] != userInput[i]) {
			correct = 0; // Set correct flag to false if any input is wrong.
			break;
		}
	}

	OLED_ClearBuffer(&oled);

	if (correct) {
		// Input was correct
		score++; // Increment score for correct answer

		currentInputIndex = 0; // Reset input index for the next sequence
		OLED_SetCursor(&oled, 0, 1);
        OLED_PutString(&oled, "Correct!"); // Display Correct message
        sleep(2); // Allow time to read message

        GenerateSequence();     // Generate a new sequence
        DisplaySequence();      // Show new sequence to the user

	} else {
		// Input was incorrect
		OLED_SetCursor(&oled, 0, 1);
		OLED_PutString(&oled, "Wrong!");

		sleep(2); // Display result for a brief moment

		gameState = 3; // Set state to game over
	}

	UpdateDisplay(); // Update display after checking input
}

void ResetGame() {
	currentInputIndex = 0; // Reset input index

	score = 0; // Reset score

	for (int i = 0; i < SEQUENCE_LENGTH; i++) {
		userInput[i] = '\0'; // Clear user input array
	}

	GenerateSequence();     // Generate a new sequence
	gameState = 1;         // Set state to display new sequence

	DisplaySequence();      // Show new sequence to the user

	gameState = 2;         // Set state to wait for user input again
}

// Stub function to resolve _gettimeofday error
int _gettimeofday(struct timeval *tv, void *tz) {
   (void)tv;
   (void)tz;
   return 0;
}
