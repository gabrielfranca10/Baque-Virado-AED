#include "lista.h"
#include <stdlib.h>

Lista criarLista(void) {
    return (Lista){ .cabeca = NULL };
}

void inserirNota(Lista *lista, float y) {
    Nota *novo = malloc(sizeof(Nota));
    novo->y    = y;
    novo->prox = NULL;

    if (!lista->cabeca) {
        lista->cabeca = novo;
        return;
    }
    Nota *cur = lista->cabeca;
    while (cur->prox) cur = cur->prox;
    cur->prox = novo;
}

void removerNota(Lista *lista) {
    if (!lista->cabeca) return;
    Nota *tmp     = lista->cabeca;
    lista->cabeca = tmp->prox;
    free(tmp);
}

int listaVazia(const Lista *lista) {
    return lista->cabeca == NULL;
}

void atualizarNotas(Lista *lista, float dt, float velocidade) {
    for (Nota *cur = lista->cabeca; cur; cur = cur->prox)
        cur->y += velocidade * dt;
}

void limparLista(Lista *lista) {
    while (!listaVazia(lista))
        removerNota(lista);
}
