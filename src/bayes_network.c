#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bayes_network.h"
#include "interface.h" // Per le funzioni di input utente

// Mappatura Nomi Malattie
const char *NOMI_MALATTIE_BAYES[] = {
    "Sano", 
    "Bolla del Pesco", 
    "Monilia", 
    "Cocciniglia", 
    "Gommosi"
};

// Istanza globale del modello
static BayesModel model;

// Buffer temporanei per il training
static int counts_class[NUM_MALATTIE];
static int counts_feature[NUM_MALATTIE][NUM_SINTOMI];
static int total_samples = 0;

// --- FUNZIONI AUSILIARIE ---

const char* get_disease_name(int id) {
    if (id >= 0 && id < NUM_MALATTIE) return NOMI_MALATTIE_BAYES[id];
    return "Sconosciuto";
}

void reset_model() {
    model.is_trained = false;
    total_samples = 0;
    memset(counts_class, 0, sizeof(counts_class));
    memset(counts_feature, 0, sizeof(counts_feature));
}

// --- FASE 1: TRAINING (Apprendimento dai Dati) ---

// Legge il CSV riga per riga e aggiorna i conteggi
bool load_training_data(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Errore: Impossibile aprire il file di training '%s'.\n", filename);
        return false;
    }

    char buffer[1024];
    // Salta l'header
    fgets(buffer, sizeof(buffer), fp);

    while (fgets(buffer, sizeof(buffer), fp)) {
        int s[NUM_SINTOMI];
        int disease_id;
        int specie_dummy; 

        // Parsing CSV: Specie, S1..S7, Malattia
        int res = sscanf(buffer, "%d,%d,%d,%d,%d,%d,%d,%d,%d", 
               &specie_dummy, 
               &s[0], &s[1], &s[2], &s[3], &s[4], &s[5], &s[6], 
               &disease_id);

        if (res == 9 && disease_id >= 0 && disease_id < NUM_MALATTIE) {
            counts_class[disease_id]++;
            total_samples++;
            
            // Aggiorna frequenze condizionali P(S|M)
            for (int i = 0; i < NUM_SINTOMI; i++) {
                if (s[i] == 1) {
                    counts_feature[disease_id][i]++;
                }
            }
        }
    }
    fclose(fp);
    return (total_samples > 0);
}

// Converte i conteggi in Log-Probabilita' (Laplace Smoothing)
void compute_model_parameters() {
    for (int m = 0; m < NUM_MALATTIE; m++) {
        // 1. Priors: P(Malattia)
        // Usiamo logaritmi per evitare underflow numerico (PDF 9)
        double p_class = (double)counts_class[m] / total_samples;
        model.log_priors[m] = log(p_class > 0 ? p_class : 1e-10);

        // 2. Likelihoods: P(Sintomo | Malattia)
        // Applicazione Smoothing (PDF 10): (Count + Alpha) / (Total + 2*Alpha)
        int denominator = counts_class[m] + (int)(ALPHA * 2);
        
        for (int s = 0; s < NUM_SINTOMI; s++) {
            double num = counts_feature[m][s] + ALPHA;
            double prob_true = num / denominator;
            model.log_likelihoods[m][s] = log(prob_true);
        }
    }
    model.is_trained = true;
    printf("Training completato con successo su %d campioni.\n", total_samples);
}

bool train_model(const char *train_file) {
    reset_model();
    if (load_training_data(train_file)) {
        compute_model_parameters();
        return true;
    }
    return false;
}

// --- FUNZIONE DI PREVISIONE (INFERENZA) ---
// Prende un array di sintomi e restituisce l'ID della malattia più probabile (MAP)
int predict_disease(int symptoms[NUM_SINTOMI]) {
    double best_score = -1e9;
    int best_class = -1;

    for (int m = 0; m < NUM_MALATTIE; m++) {
        double score = model.log_priors[m];
        
        for (int s = 0; s < NUM_SINTOMI; s++) {
            if (symptoms[s] == 1) {
                score += model.log_likelihoods[m][s]; // + log P(S|C)
            } else {
                // Se il sintomo è assente, aggiungiamo log(1 - P(S|C))
                double p_true = exp(model.log_likelihoods[m][s]);
                score += log(1.0 - p_true);
            }
        }
        
        if (score > best_score) {
            best_score = score;
            best_class = m;
        }
    }
    return best_class;
}

// --- FASE 2: TESTING (Validazione Modello) ---
// Legge il file di test e calcola l'accuratezza
void test_model_accuracy(const char *test_file) {
    if (!model.is_trained) {
        printf("Errore: Il modello non è stato addestrato.\n");
        return;
    }

    FILE *fp = fopen(test_file, "r");
    if (!fp) {
        printf("Errore: Impossibile aprire il file di test '%s'.\n", test_file);
        return;
    }

    printf("\n--- AVVIO VALIDAZIONE SU TEST SET (%s) ---\n", test_file);

    char buffer[1024];
    int correct = 0;
    int total = 0;
    
    // Matrice di confusione
    int confusion_matrix[NUM_MALATTIE][NUM_MALATTIE] = {0};

    fgets(buffer, sizeof(buffer), fp); // Salta header

    while (fgets(buffer, sizeof(buffer), fp)) {
        int s[NUM_SINTOMI];
        int actual_disease;
        int specie_dummy;

        int res = sscanf(buffer, "%d,%d,%d,%d,%d,%d,%d,%d,%d", 
               &specie_dummy, 
               &s[0], &s[1], &s[2], &s[3], &s[4], &s[5], &s[6], 
               &actual_disease);

        if (res == 9) {
            int predicted_disease = predict_disease(s);
            
            confusion_matrix[actual_disease][predicted_disease]++;
            
            if (predicted_disease == actual_disease) {
                correct++;
            }
            total++;
        }
    }
    fclose(fp);

    if (total > 0) {
        double accuracy = (double)correct / total * 100.0;
        printf("Campioni testati: %d\n", total);
        printf("Predizioni corrette: %d\n", correct);
        printf(">>> ACCURATEZZA DEL MODELLO: %.2f%%\n", accuracy);
    } else {
        printf("Nessun dato valido trovato nel file di test.\n");
    }
}

// --- INTERFACCIA UTENTE (Diagnosi Interattiva) ---

void run_bayes_network() {
    if (!model.is_trained) {
        printf("ATTENZIONE: Modello non addestrato.\n");
        return;
    }

    printf("\n*** DIAGNOSTICA RETE BAYESIANA ***\n");
    printf("Rispondi alle domande per l'analisi probabilistica.\n");

    int s[NUM_SINTOMI];
    const char* questions[] = {
        "Foglie arricciate?",
        "Macchie scure/muffa sulle foglie?",
        "Muffa sui frutti?",
        "Insetti visibili?",
        "Melata (sostanza appiccicosa)?",
        "Gommosi sul tronco?",
        "Frutti deformi?"
    };

    for (int i = 0; i < NUM_SINTOMI; i++) {
        s[i] = ask_question(questions[i]);
    }

    // Inferenza
    int result = predict_disease(s);
    
    printf("\n>>> RISULTATO INFERENZA: %s\n", get_disease_name(result));
    printf("(Diagnosi basata sulla massima probabilita' a posteriori)\n");
}
