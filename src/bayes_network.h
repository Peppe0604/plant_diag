#ifndef BAYES_NETWORK_H
#define BAYES_NETWORK_H

#include <stdbool.h>

// Definizione del dominio (Feature Engineering)
#define NUM_SINTOMI 7
#define NUM_MALATTIE 5

// Parametro di Smoothing (Laplace/Dirichlet - PDF 10)
// Evita probabilità zero per eventi mai visti nel training set.
#define ALPHA 1.0 

// Struttura del Modello Bayesiano
typedef struct {
    // Probabilità a Priori P(Malattia) in scala Logaritmica
    double log_priors[NUM_MALATTIE]; 

    // Likelihoods P(Sintomo=1 | Malattia) in scala Logaritmica
    // Matrice [Malattia][Sintomo]
    double log_likelihoods[NUM_MALATTIE][NUM_SINTOMI]; 

    // Flag per indicare se il modello è pronto
    bool is_trained;
} BayesModel;

// --- API PUBBLICHE ---

// Inizializza e addestra il modello usando il file CSV di training
bool train_model(const char *train_file);

// Valuta l'accuratezza del modello usando il file CSV di test
void test_model_accuracy(const char *test_file);

// Esegue la diagnosi interattiva con l'utente
void run_bayes_network();

// Restituisce il nome della malattia dato l'ID
const char* get_disease_name(int id);

#endif