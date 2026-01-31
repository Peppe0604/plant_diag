#include <stdio.h>
#include <stdbool.h>
#include "expert_system.h"
#include "interface.h" 

// --- VARIABILI GLOBALI PER IL MOTORE ---
int working_memory[NUM_FACTS];      // 0=Unknown, 1=True, -1=False

// --- MAPPATURA DOMANDE ---
const char* FACT_QUESTIONS[NUM_FACTS] = {
    NULL, NULL, NULL, // Specie (impostate da menu)
    "Le foglie appaiono arricciate, bollose o deformi?",      // F_FOGLIE_ARRICCIATE
    "I frutti o i fiori presentano muffa grigia/marciume?",   // F_MUFFA_GRIGIA
    "Vedi insetti bianchi o scudetti sui rami?",              // F_INSETTI_BIANCHI
    "Le foglie sono appiccicose (melata) o nere?",            // F_MELATA
    "C'e' fuoriuscita di gomma (resina) dal legno?"           // F_GOMMOSI
};

// --- BASE DI CONOSCENZA (LE REGOLE) ---
Rule KB[] = {
    // Regola 0: Cocciniglia (Richiede Mandarino + Insetti + Melata)
    { 
        0, 
        {F_MANDARINO, F_INSETTI_BIANCHI, F_MELATA, -1}, 
        -1, D_COCCINIGLIA,
        "COCCINIGLIA: Usa olio bianco minerale o sapone molle potassico."
    },
    // Regola 1: Bolla del Pesco
    { 
        1, 
        {F_PESCO, F_FOGLIE_ARRICCIATE, -1}, 
        -1, D_BOLLA,
        "BOLLA DEL PESCO: Trattamenti con ditiocarbammati o rame in autunno/inverno."
    },
    // Regola 2: Monilia (Pesco)
    { 
        2, 
        {F_PESCO, F_MUFFA_GRIGIA, -1}, 
        F_INFEZIONE_FUNGINA, D_MONILIA,
        "MONILIA: Rimuovi i frutti mummificati e tratta con fungicidi specifici."
    },
    // Regola 3: Monilia (Albicocco)
    { 
        3, 
        {F_ALBICOCCO, F_MUFFA_GRIGIA, -1}, 
        F_INFEZIONE_FUNGINA, D_MONILIA,
        "MONILIA: Rimuovi i frutti mummificati e tratta con fungicidi specifici."
    },
    // Regola 4: Gommosi (Mandarino -> Phytophthora)
    { 
        4, 
        {F_MANDARINO, F_GOMMOSI, -1}, 
        -1, D_GOMMOSI_PHYTOPHTHORA,
        "PHYTOPHTHORA: Rimuovi la corteccia malata e spennella con sali di rame."
    },
    // Regola 5: Gommosi (Pesco/Albicocco -> Corineo/Cancro)
    { 
        5, 
        {F_PESCO, F_GOMMOSI, -1}, 
        -1, D_GOMMOSI_CORINEO,
        "CORINEO: Trattamenti rameici alla caduta foglie e rottura gemme."
    }
};
const int NUM_RULES = sizeof(KB) / sizeof(Rule);

// --- FUNZIONI DEL MOTORE ---

// Verifica un fatto. Se sconosciuto, chiede all'utente (Lazy Evaluation)
bool check_fact(int fact_id) {
    // 1. Se già noto in memoria, ritorna valore
    if (working_memory[fact_id] != 0) {
        return (working_memory[fact_id] == 1);
    }

    // 2. Se possiamo chiederlo all'utente, chiediamo
    if (FACT_QUESTIONS[fact_id] != NULL) {
        int response = ask_question(FACT_QUESTIONS[fact_id]);
        working_memory[fact_id] = (response == 1) ? 1 : -1;
        return (response == 1);
    }

    // 3. Fatto non derivabile (default false)
    return false;
}

void run_expert_system() {
    // Reset Memoria
    for(int i=0; i<NUM_FACTS; i++) working_memory[i] = 0;
    
    printf("\n*** DIAGNOSTICA ESPERTA  (Rule-Based) ***\n");
    
    // Acquisizione Contesto Iniziale
    const char *specie_opts[] = {"Pesco", "Mandarino", "Albicocco"};
    int scelta = user_menu("Seleziona la specie vegetale", specie_opts, 3);
    
    if (scelta == 1) working_memory[F_PESCO] = 1;
    else if (scelta == 2) working_memory[F_MANDARINO] = 1;
    else if (scelta == 3) working_memory[F_ALBICOCCO] = 1;

    // --- CICLO DI INFERENZA (Forward Chaining) ---
    bool diagnosis_found = false;

    for (int i = 0; i < NUM_RULES; i++) {
        bool rule_triggered = true;

        // Verifica tutti gli antecedenti
        for (int j = 0; j < MAX_ANTECEDENTS; j++) {
            int fact = KB[i].antecedents[j];
            if (fact == -1) break; // Fine lista antecedenti

            if (!check_fact(fact)) {
                rule_triggered = false;
                break; // Un antecedente è falso, regola scartata
            }
        }

        if (rule_triggered) {
            // Aggiorna Memoria (Conseguente intermedio)
            if (KB[i].consequent_fact != -1) {
                working_memory[KB[i].consequent_fact] = 1;
            }

            // Se c'è una diagnosi finale
            if (KB[i].diagnosis_id != -1) {
                printf("\n>>> DIAGNOSI TROVATA (Regola %d)\n", i);
                printf("Consiglio: %s\n", KB[i].advice);
                diagnosis_found = true;
                break; // Stop alla prima diagnosi
            }
        }
    }

    if (!diagnosis_found) {
        printf("\n>>> Nessuna regola attivata completamente.\n");
        printf("Suggerimento: I sintomi non corrispondono a regole certe. Usa la Rete Bayesiana.\n");
    }
}
