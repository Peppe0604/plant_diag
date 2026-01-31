#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "interface.h"

int ask_question(const char *question) {
    char answer;
    printf("%s (s/n): ", question);
    while (1) {
        scanf(" %c", &answer);
        answer = tolower(answer);
        if (answer == 's' || answer == 'y') return 1;
        if (answer == 'n') return 0;
        printf("Risposta non valida. %s (s/n): ", question);
    }
}

int ask_integer(const char *question) {
    int value;
    printf("%s: ", question);
    while (scanf("%d", &value) != 1) {
        printf("Valore non valido! Inserisci un numero intero: ");
        while(getchar() != '\n'); // Pulisce il buffer
    }
    return value;
}

int user_menu(const char *title, const char **options, int num_options) {
    printf("\n--- %s ---\n", title);
    for (int i = 0; i < num_options; i++) {
        printf("%d) %s\n", i + 1, options[i]);
    }
    
    int choice = 0;
    do {
        choice = ask_integer("Seleziona una opzione");
    } while (choice < 1 || choice > num_options);
    
    return choice;
}

void wait_user() {
    printf("\nPremi Invio per continuare...");
    while(getchar() != '\n');
    getchar();
}