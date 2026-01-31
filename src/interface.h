#ifndef INTERFACE_H
#define INTERFACE_H

// Ritorna 1 per Sì, 0 per No
int ask_question(const char *question);

// Chiede un intero all'utente
int ask_integer(const char *question);

// Mostra un menu e ritorna la scelta
int user_menu(const char *title, const char **options, int num_options);

// Attende input utente
void wait_user();

#endif