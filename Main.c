/*
    Topics to cover:
    Process Hierarchy: (in main)
        DONE
        pidB can be the child responsible for printing the menu, and pidA can handle the main loop.
    waitpid: (in main)
        DONE
        Here we can wait for the player to finish their turn before printing the board.
    Threads: (in start_game)
        DONE
        Being created to verify the entered word.
    Signals: (main and sigint handler)
        ALMOST DONE; CANCELLING WORKS BUT NOT CONTINUING
        Here we can implement a signal so that when the player presses ctrl+c, the game ends
        but ask if they are sure they want to end the game.
    Alarms: (main and alarm handler)
        DONE
        Here we can set an alarm to change the word after a certain interval.
    Synchronization: (wordIndex)
        DONE
        Here we can synchronize processes to ensure they execute in the correct order.
        pidB can be the child responsible for printing the menu, and pidA can handle the main loop.
*/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#define WORD_COUNT 6  // Ammount of words in the game
#define BOARD_SIZE 13 // Size of the board

pthread_mutex_t lock;
int wordtype[6]={0,0,0,0,0,0};
int wordStatus[6]={0,0,0,0,0,0};
int choice = 0;
bool shouldExit = false;
char board[BOARD_SIZE][BOARD_SIZE];
int score = 0;
int boardStatus = 0;
pthread_t tid;




// Structure to store the words
typedef struct {
    char word[20];    // Store the word
    int row;          // Starting row on the board
    int col;          // Starting column on the board
    bool horizontal;  // Direction: true for horizontal, false for vertical
    bool placed;      // Status: true if the word is placed on the board
} Word;

// Global array to store the words
Word word[] = {
    {"11111", 5, 4, true, false},    // Horizontal in (5, 4)
    {"22222", 7, 0, true, false},    // Horizontal in (7, 0)
    {"333333", 2, 7, true, false},   // Horizontal in (2, 7)
    {"4*44*4", 1, 7, false, false},   // Vertical in (1, 7)
    {"555*5*", 2, 4, false, false},   // Vertical in (2, 4)
    {"*6666", 7, 2, false, false}    // Vertical in (7, 2)
};


// Función para imprimir el tablero
void printBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    if (boardStatus == 0) {
        printf("\n");

        // Print top line of the board in green color
        printf("   \x1b[32m*");  // Change color to green for the top line
        for (int col = 0; col < BOARD_SIZE; col++) {
            printf("\x1b[32m---\x1b[32m*");  // Change color to green for horizontal lines
        }
        printf("\n");

        // Print board rows with content
        for (int row = 0; row < BOARD_SIZE; row++) {
            printf("   \x1b[32m|");  // Change color to green for vertical lines
            for (int col = 0; col < BOARD_SIZE; col++) {
                // Temporarily change color to blue only for cell content
                printf("\x1b[34m %c \x1b[32m|", board[row][col]);  // Cell content with blue color and green separators
            }
            printf("\n");

            // Print separator line between each row with green color
            printf("   \x1b[32m*");
            for (int col = 0; col < BOARD_SIZE; col++) {
                printf("\x1b[32m---\x1b[32m*");  // Change color to green for horizontal lines
            }
            printf("\n");
        }

        // Restore original color after printing the board
        printf("\x1b[0m\n");
    } else if (boardStatus == 1) {
        printf("\n");

        // Print top line of the board in red color
        printf("   \x1b[31m*");  // Change color to red for the top line
        for (int col = 0; col < BOARD_SIZE; col++) {
            printf("\x1b[31m---\x1b[31m*");  // Change color to red for horizontal lines
        }
        printf("\n");

        // Print board rows with content
        for (int row = 0; row < BOARD_SIZE; row++) {
            // Print horizontal line before each row with red color
            printf("   \x1b[31m|");  // Change color to red for vertical lines
            for (int col = 0; col < BOARD_SIZE; col++) {
                // Temporarily change color to blue only for cell content
                printf("\x1b[34m %c \x1b[31m|", board[row][col]);  // Cell content with blue color and red separators
            }
            printf("\n");

            // Print separator line between each row with red color
            printf("   \x1b[31m*");
            for (int col = 0; col < BOARD_SIZE; col++) {
                printf("\x1b[31m---\x1b[31m*");  // Change color to red for horizontal lines
            }
            printf("\n");
        }

        // Restore original color after printing the board
        printf("\x1b[0m\n");
    }

}

// Function to place a word on the board
void placeWord(char board[BOARD_SIZE][BOARD_SIZE], const Word *word) {
    int len = strlen(word->word);
    int row = word->row;
    int col = word->col;

    for (int i = 0; i < len; i++) {
        board[row][col] = word->word[i];
        if (word->horizontal) {
            col++;
        } else {
            row++;
        }
    }
}
//Function to place all the words on the board
void placeWordsOnBoard(char board[BOARD_SIZE][BOARD_SIZE], Word words[], int numWords) {
    for (int i = 0; i < numWords; i++) {
        placeWord(board, &words[i]);
        words[i].placed = true;
    }
}
//Function to check if the word is placed on the board
void *wordIndexChecker(void *arg){
    char palabra[20];
    pthread_mutex_lock(&lock);
    if(choice ==1){
        switch (wordtype[0]){
            case 0:
                printf("************************************************************************************************\n");
                printf("A EPIC Movie starring lisan al gahib (timothy chalamet)\n");
                scanf("%s",palabra);
                if(strcmp(palabra,"DUNE")==0||strcmp(palabra,"dune")==0){
                    printf("Correct\n");
                    strcpy(word[0].word, "DUNE");
                    score++;
                }else{
                    printf("Incorrect\n");
                }
                break;
            case 1:
                printf("************************************************************************************************\n");
                printf("Challenges or taunts someone to do something bold or risky.\n");
                scanf("%s",palabra);
                if(strcmp(palabra,"DARE")==0||strcmp(palabra,"dare")==0){
                    printf("Correct\n");
                    strcpy(word[0].word, "DARE");
                    score++;
                }else{
                    printf("Incorrect\n");
                }
                break;
        }
    }
    if(choice ==2){
        switch (wordtype[1]){
            case 0:
                printf("************************************************************************************************\n");
                printf("The golf kid that follows you around\n");
                scanf("%s",palabra);
                if(strcmp(palabra,"CADDY")==0||strcmp(palabra,"caddy")==0){
                    printf("Correct\n");
                    strcpy(word[1].word, "CADDY");
                    score++;
                }else{
                    printf("Incorrect\n");
                }
                break;
            case 1:
                printf("************************************************************************************************\n");
                printf("An endeering way to say dad\n");
                scanf("%s",palabra);
                if(strcmp(palabra,"DADDY")==0||strcmp(palabra,"daddy")==0){
                    printf("Correct\n");
                    strcpy(word[1].word, "DADDY");
                    score++;
                }else{
                    printf("Incorrect\n");
                }
                break;
        }
    }
    if(choice ==3){
        switch (wordtype[2]){
            case 0:
                printf("************************************************************************************************\n");
                printf("Not utilized or employed; in a state of disuse.\n");
                scanf("%s",palabra);
                if(strcmp(palabra,"UNUSED")==0||strcmp(palabra,"unused")==0){
                    printf("Correct\n");
                    strcpy(word[2].word, "UNUSED");
                    score++;
                }else{
                    printf("Incorrect\n");
                }
                break;
            case 1:
                printf("************************************************************************************************\n");
                printf("A sea ______\n");
                scanf("%s",palabra);
                if(strcmp(palabra,"URCHIN")==0||strcmp(palabra,"urchin")==0){
                    printf("Correct\n");
                    strcpy(word[2].word, "URCHIN");
                    score++;
                }else{
                    printf("Incorrect\n");
                }
                break;
        }
    }
    if(choice ==4){
        switch (wordtype[3]){
            case 0:
                printf("************************************************************************************************\n");
                printf("Happening quickly or unexpectedly, without warning.\n");
                scanf("%s",palabra);
                if(strcmp(palabra,"SUDDEN")==0||strcmp(palabra,"sudden")==0){
                    printf("Correct\n");
                    strcpy(word[3].word, "SUDDEN");
                    score++;
                }else{
                    printf("Incorrect\n");
                }
                break;
            case 1:
                printf("************************************************************************************************\n");
                printf("A place where objects of historical, cultural, or scientific significance are displayed.\n");
                scanf("%s",palabra);
                if(strcmp(palabra,"MUSSEUM")==0||strcmp(palabra,"musseum")==0){
                    printf("Correct\n");
                    strcpy(word[3].word, "MUSSEUM");
                    score++;
                }else{
                    printf("Incorrect\n");
                }
                break;
        }
    }
    if(choice ==5){
        switch (wordtype[4]){
            case 0:
                printf("************************************************************************************************\n");
                printf("The first day of the week.\n");
                scanf("%s",palabra);
                if(strcmp(palabra,"MONDAY")==0||strcmp(palabra,"monday")==0){
                    printf("Correct\n");
                    strcpy(word[4].word, "MONDAY");
                    score++;
                }else{
                    printf("Incorrect\n");
                }
                break;
            case 1:
                printf("************************************************************************************************\n");
                printf("The day of the week following Saturday and preceding Monday.\n");
                scanf("%s",palabra);
                if(strcmp(palabra,"SUNDAY")==0||strcmp(palabra,"sunday")==0){
                    printf("Correct\n");
                    strcpy(word[4].word, "SUNDAY");
                    score++;
                }else{
                    printf("Incorrect\n");
                }
                break;
        }
    }
    if(choice ==6){
        switch (wordtype[5]){
            case 0:
                printf("************************************************************************************************\n");
                printf("An expressive movement of the body, typically to music, often performed as an art form or for entertainment.\n");
                scanf("%s",palabra);
                if(strcmp(palabra,"DANCE")==0||strcmp(palabra,"dance")==0){
                    printf("Correct\n");
                    strcpy(word[5].word, "DANCE");
                    score++;
                }else{
                    printf("Incorrect\n");
                }
                break;
            case 1:
                printf("************************************************************************************************\n");
                printf("Ring-shaped pastries made of sweet dough, often fried and topped with glaze, icing, or other sweet toppings.\n");
                scanf("%s",palabra);
                if(strcmp(palabra,"DONUT")==0||strcmp(palabra,"donut")==0){
                    printf("Correct\n");
                    strcpy(word[5].word, "DONUT");
                    score++;
                }else{
                    printf("Incorrect\n");
                }
                break;
        }
    }
    pthread_mutex_unlock(&lock);  // Desbloquear el mutex
    return NULL;
}
void start_game(char board[BOARD_SIZE][BOARD_SIZE], Word words[], int numWords){
    pthread_t tid;
    pthread_mutex_init(&lock, NULL);
    while(score<=5){
        printf("Which word do you want to try?\n");
        scanf("%d",&choice);
        pthread_create(&tid, NULL, wordIndexChecker, NULL);
        pthread_join(tid, NULL);
        placeWordsOnBoard(board, word, numWords);
        printBoard(board);
    }
    printf("Congratulations, you won!\n");
    shouldExit = true;
}
void alarm_handler(int signum) {
    // Cambiar tablero cada cierto tiempo
    printf("The words have changed\n");
    for(int i = 0; i<=6;i++){
        if(wordStatus[i] == 0){
            wordtype[i] = 1;
        } else {
            wordtype[i] = 0;
        }
    }
    if (boardStatus == 0){
        boardStatus = 1;
    } else {
        boardStatus = 0;
    }
    alarm(30); // Restablecer la alarma para dentro de 30 segundos
}
void rules(){
    char enter;
    printf("***DA RULES***\n");
    printf("1. You will be presented with a crossword board where you can pick from different words to guess\n");
    printf("2. Pick the number of the word that is available\n");
    printf("3. The connection between 2 words will be cleared up when both are discovered\n");
    printf("4. The words that you haven't guessed will change every 30 seconds, be carefull (A notification will appear and the board will change of color) :)\n");
    printf("5. There is 2 words for each row/column\n");
    printf("5. If the words change while you were trying to guess a new will be designated, so if you remember the first guess then pick the previous one\n");
    printf("6. All the guesses have to be in capital letters\n");
    printf("7. You have to guess 6 words to win\n");
    printf("Press enter to continue...\n");
    scanf("%c", &enter);
    if (enter == '\n') {
        printf("Good luck\n");
    }
}
void intro(){
    char enter;
    printf("************************************\n");
    printf("Welcome to the house of the leafs\n");
    printf("The word shifting crossword game!\n");
    printf("************************************\n");
    printf("Are you ready to play?\n");
    printf("Press enter to continue...\n");
    scanf("%c", &enter);

    if (enter == '\n') {
        rules();
    }
}
// Function to handle the SIGINT signal
void sigintHandler(int signum) {
    char answer;
    printf("\nAre you sure you want to exit the game? (y/n)\n");
    scanf(" %c", &answer);

    if (answer == 'y' || answer == 'Y') {
        shouldExit = true;
        exit(0);
    } else {
        shouldExit = false;
        printf("Continuing the game...\n");
        pthread_join(tid, NULL);
        return;
    }
}
// Main function
int main() {
    memset(board, '.', sizeof(board));   // initialize board with dots
    // Create child process (pidB)
    pid_t pidB = fork();

    if (pidB == 0) {
        // Child process (pidB) - place words on the board and display it
        intro();
        placeWordsOnBoard(board, word, sizeof(word) / sizeof(word[0]));
        printBoard(board);
        exit(EXIT_SUCCESS);
    } else {
        // Parent process (pidA)
        // Wait for the child process (pidB) to finish
        waitpid(pidB, NULL, 0);
        alarm(30);

        // Now, continue with the game setup in the parent process (pidA)
        signal(SIGINT, sigintHandler);
        signal(SIGALRM, alarm_handler);
        while (!shouldExit) {
            start_game(board, word, sizeof(word) / sizeof(word[0]));
        }

    }
    return 0;
}
