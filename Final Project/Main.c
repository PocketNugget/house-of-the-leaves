/*
    temas a cubrir:
    jerarquía de procesos: (en main)
        LISTO
        pidB puede ser el hijo que imprima el menu y pidA puede ser el que tenga el loop
    waitpid:(en main)
        LISTO
        Aqui podemos esperar a que el jugador termine su turno antes de imprimir el tablero
    hilos:(en start_game)
        LISTO
        Se esta creando para verificar la palabra introducida
    señales:(main y sigint handler)
        CASI LISTO FUNCIONA CANCELAR PERO NO CONTINUAR
        Aqui se puede implementar una señal para que cuando el jugador presione ctrl+c se termine el juego
        pero preguntar si esta seguro de terminar el juego
    alarmas:(main y alarm handler)
        LISTO
        Aqui podemos meter una alarma que sirva para cambiar la palabra en un intervalo de tiempo
    sincronización:(wordIndex)
        LISTO
        Aqui podemos sincronizar los procesos para que se ejecuten en el orden correcto
        pidB puede ser el hijo que imprima el menu y pidA puede ser el que tenga el loop
*/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#define WORD_COUNT 6  // Número de palabras en el arreglo
#define BOARD_SIZE 13

pthread_mutex_t lock;
int wordtype[6]={0,0,0,0,0,0};
int wordStatus[6]={0,0,0,0,0,0};
int choice = 0;
bool shouldExit = false;  // Variable para controlar la salida del programa
char board[BOARD_SIZE][BOARD_SIZE];  // Tablero
int score = 0;  // Puntaje
int boardStatus = 0;
pthread_t tid;




// Estructura para representar una palabra en el tablero
typedef struct {
    char word[20];    // Almacenar la palabra
    int row;          // Fila de inicio en el tablero
    int col;          // Columna de inicio en el tablero
    bool horizontal;  // Dirección: true para horizontal, false para vertical
    bool placed;      // Estado: true si la palabra está colocada en el tablero
} Word;

// Arreglo global de palabras
Word word[] = {
    {"11111", 5, 4, true, false},    // Horizontal en (5, 4)
    {"22222", 7, 0, true, false},    // Horizontal en (7, 0)
    {"333333", 2, 7, true, false},   // Horizontal en (2, 7)
    {"4*44*4", 1, 7, false, false},   // Vertical en (1, 7)
    {"555*5*", 2, 4, false, false},   // Vertical en (2, 4)
    {"*6666", 7, 2, false, false}    // Vertical en (7, 2)
};


// Función para imprimir el tablero
void printBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    if (boardStatus==0){
        printf("\n");

        // Imprimir línea superior del tablero con color verde
        printf("   \x1b[32m*");  // Cambiar el color a verde para la línea superior
        for (int col = 0; col < BOARD_SIZE; col++) {
            printf("\x1b[32m---\x1b[32m*");  // Cambiar el color a verde para las líneas horizontales
        }
        printf("\n");

        // Imprimir filas del tablero con contenido
        for (int row = 0; row < BOARD_SIZE; row++) {
            // Imprimir línea horizontal antes de cada fila con color verde
            printf("   \x1b[32m|");  // Cambiar el color a verde para las líneas verticales
            for (int col = 0; col < BOARD_SIZE; col++) {
                // Cambiar temporalmente el color a azul solo para el contenido de las celdas
                printf("\x1b[34m %c \x1b[32m|", board[row][col]);  // Contenido de la celda con color azul y separadores verdes
            }
            printf("\n");

            // Imprimir línea separadora entre cada fila con color verde
            printf("   \x1b[32m*");
            for (int col = 0; col < BOARD_SIZE; col++) {
                printf("\x1b[32m---\x1b[32m*");  // Cambiar el color a verde para las líneas horizontales
            }
            printf("\n");
        }

        // Restaurar el color original al final de la impresión del tablero
        printf("\x1b[0m\n");
    }   else if (boardStatus==1){
        printf("\n");

        // Imprimir línea superior del tablero con color rojo
        printf("   \x1b[31m*");  // Cambiar el color a rojo para la línea superior
        for (int col = 0; col < BOARD_SIZE; col++) {
            printf("\x1b[31m---\x1b[31m*");  // Cambiar el color a rojo para las líneas horizontales
        }
        printf("\n");

        // Imprimir filas del tablero con contenido
        for (int row = 0; row < BOARD_SIZE; row++) {
            // Imprimir línea horizontal antes de cada fila con color rojo
            printf("   \x1b[31m|");  // Cambiar el color a rojo para las líneas verticales
            for (int col = 0; col < BOARD_SIZE; col++) {
                // Cambiar temporalmente el color a azul solo para el contenido de las celdas
                printf("\x1b[34m %c \x1b[31m|", board[row][col]);  // Contenido de la celda con color azul y separadores rojos
            }
            printf("\n");

            // Imprimir línea separadora entre cada fila con color rojo
            printf("   \x1b[31m*");
            for (int col = 0; col < BOARD_SIZE; col++) {
                printf("\x1b[31m---\x1b[31m*");  // Cambiar el color a rojo para las líneas horizontales
            }
            printf("\n");
        }
        // Restaurar el color original al final de la impresión del tablero
        printf("\x1b[0m\n");
    }
}

// Función para colocar una palabra en el tablero
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
//Funcion para colocar palabras en el tablero
void placeWordsOnBoard(char board[BOARD_SIZE][BOARD_SIZE], Word words[], int numWords) {
    for (int i = 0; i < numWords; i++) {
        placeWord(board, &words[i]);
        words[i].placed = true;
    }
}
//Funcion para verificar si la palabra se encuentra en el tablero
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
// Función para manejar la señal SIGINT (Ctrl+C)
void sigintHandler(int signum) {
    char answer;
    printf("\n¿Estás seguro de que quieres salir del juego? Presiona 'y' para confirmar, o cualquier otra tecla para continuar: ");
    scanf(" %c", &answer);

    if (answer == 'y' || answer == 'Y') {
        shouldExit = true;
        exit(0);
    } else {
        shouldExit = false;
        printf("Continuando con el juego...\n");
        pthread_join(tid, NULL);
        return;
    }
}
// Función principal
int main() {
    memset(board, '.', sizeof(board));   // Inicializar el tablero con puntos (.)

    // Crear proceso hijo (pidB)
    pid_t pidB = fork();

   if (pidB == 0) {
        // Proceso hijo (pidB) - lugar para las palabras en el tablero y mostrarlo
        intro();
        placeWordsOnBoard(board, word, sizeof(word) / sizeof(word[0]));
        printBoard(board);
        exit(EXIT_SUCCESS);
    } else {
        // Proceso padre (pidA)
        // Esperar a que el proceso hijo (pidB) termine
        waitpid(pidB, NULL, 0);
        alarm(30);

        // Ahora, continuar con el inicio del juego (pidA)
        signal(SIGINT, sigintHandler);
        signal(SIGALRM, alarm_handler);
        while(!shouldExit) {
        start_game(board, word, sizeof(word) / sizeof(word[0]));
    }
    }
    return 0;
}
