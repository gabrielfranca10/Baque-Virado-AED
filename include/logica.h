#ifndef LOGICA_H
#define LOGICA_H

#include "lista.h"
#include <stdlib.h>

#define NUM_COLUNAS      4
#define TOTAL_FASES      3
#define VELOCIDADE_NOTA  194.3f
#define LINHA_ACERTO     500
#define NOTE_START_Y     (-30.0f)
#define JANELA_ACERTO    45
#define JANELA_ACERTO_F3 28
#define FEEDBACK_DURACAO 0.5f
#define DURACAO_FASE     80.0f
#define TEMPO_QUEDA      ((LINHA_ACERTO - NOTE_START_Y) / VELOCIDADE_NOTA)

typedef struct {
    const char *titulo;
    const char *arquivo;
    const char *artista;
} InfoFase;

extern const InfoFase FASES[TOTAL_FASES];

typedef enum {
    ACERTO_NENHUM = 0,
    ACERTO_PERFEITO,
    ACERTO_BOM,
    ACERTO_ERROU
} TipoAcerto;

typedef struct {
    Lista      colunas[NUM_COLUNAS];
    int        pontuacao;
    int        combo;
    float      tempoJogo;
    int        idxSequencia;
    int        faseAtual;
    TipoAcerto ultimoAcerto[NUM_COLUNAS];
    float      tempoFeedback[NUM_COLUNAS];
    int        permutacaoColunas[NUM_COLUNAS];
} EstadoJogo;

#define MAX_RANKING 10
typedef struct {
    int pontuacao;
} EntradaRanking;

void       iniciarJogo    (EstadoJogo *estado, int fase);
void       atualizarJogo  (EstadoJogo *estado, float dt);
TipoAcerto verificarAcerto(EstadoJogo *estado, int coluna);
void       encerrarJogo   (EstadoJogo *estado);
int        faseConcluida  (const EstadoJogo *estado);
void       ordenarRanking (EntradaRanking *ranking, int n);
void       adicionarScore (EntradaRanking *ranking, int *n, int pontuacao);

#endif
