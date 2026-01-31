#include <stdio.h>
#include <stdlib.h>
#include "interface.h"
#include "expert_system.h"
#include "bayes_network.h"

int main() {
    printf("==========================================\n");
    printf("       SISTEMA DIAGNOSTICO PIANTE         \n");
    printf("==========================================\n");

    printf("Caricamento dataset di training...\n");
    if (!train_model("doc/train.csv")) {
        printf("AVVISO: File 'doc/train.csv' non trovato. La rete Bayesiana non funzionera'.\n");
    }

    const char *options[] = {
        "Diagnostica con Sistema Esperto (Regole)",
        "Diagnostica con Rete Bayesiana (Apprendimento)",
        "Verifica Accuratezza Modello (su test.csv)",
        "Esci"
    };

    int choice = 0;
    while (choice != 4) {
        choice = user_menu("Menu Principale", options, 4);
        
        switch(choice) {
            case 1:
                run_expert_system();
                wait_user();
                break;
            case 2:
                run_bayes_network();
                wait_user();
                break;
            case 3:
                test_model_accuracy("doc/test.csv");
                wait_user();
                break;
            case 4:
                printf("Arrivederci.\n");
                break;
        }
    }

    return 0;
}
