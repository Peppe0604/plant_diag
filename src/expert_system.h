#ifndef EXPERT_SYSTEM_H
#define EXPERT_SYSTEM_H

#define MAX_ANTECEDENTS 5

// Identificatori per Fatti (Specie e Sintomi)
typedef enum {
    // CONTESTO
    F_PESCO = 0,
    F_MANDARINO,
    F_ALBICOCCO,
    
    // SINTOMI
    F_FOGLIE_ARRICCIATE,
    F_MUFFA_GRIGIA,
    F_INSETTI_BIANCHI,
    F_MELATA,
    F_GOMMOSI,
    
    // Fatti Intermedi
    F_INFEZIONE_FUNGINA, 

    NUM_FACTS // Utile per dimensionare array
} FactID;

// Identificatori per Diagnosi
typedef enum {
    D_NESSUNA = -1,
    D_BOLLA = 0,
    D_MONILIA,
    D_COCCINIGLIA,
    D_GOMMOSI_PHYTOPHTHORA,
    D_GOMMOSI_CORINEO
} DiagnosisID;

// LA STRUTTURA DELLA REGOLA
typedef struct {
    int id;
    int antecedents[MAX_ANTECEDENTS]; // Lista di FactID necessari (-1 per terminare)
    int consequent_fact;              // Fatto che diventa VERO (o -1 se è diagnosi finale)
    int diagnosis_id;                 // ID Diagnosi finale (o -1 se è regola intermedia)
    const char* advice;               // Consiglio finale (se c'è diagnosi)
} Rule;

void run_expert_system();

#endif