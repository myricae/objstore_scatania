#ifndef HTABLE_H
#define HTABLE_H
#define HT_DIM 128

typedef struct _node{
    char* name;
    struct _node* next;
}node;

/*IF HTABLE.DAT !exists: crea ed inizializza a zero hash table con n entry, 
ELSE: ripristina HTABLE.DAT in memoria principale. 
RETURNS: un puntatore alla prima posizione della tabella*/
node** init(node** ht);
int add(char* name,node** ht); // Usa 'name' come hash key e DJB2 come algoritmo di hashing: 0 non riuscita, 1 riuscita senza collisione, >1 con collisione
int destroy(node** ht);
int exists(char* name,node** ht); //0 utente 'name' non registrato, 1 registrato
int edit(char* name,node** ht);
#endif