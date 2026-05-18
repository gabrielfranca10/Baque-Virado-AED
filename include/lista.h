#ifndef LISTA_H
#define LISTA_H

typedef struct Nota {
    float        y;
    struct Nota *prox;
} Nota;

typedef struct {
    Nota *cabeca;
} Lista;

Lista criarLista(void);
void  inserirNota(Lista *lista, float y);
void  removerNota(Lista *lista);
int   listaVazia(const Lista *lista);
void  atualizarNotas(Lista *lista, float dt, float velocidade);
void  limparLista(Lista *lista);

#endif
