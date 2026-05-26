#ifndef RENDER_H
#define RENDER_H

#include "raylib.h"
#include "logica.h"

#define COR_FUNDO    (Color){15,  10,  30,  255}
#define COR_DOURADO  (Color){255, 200, 50,  255}
#define COR_VERMELHO (Color){200, 30,  50,  255}
#define COR_BRANCO   (Color){240, 235, 220, 255}
#define COR_CINZA    (Color){100, 95,  110, 255}
#define COR_OVERLAY  (Color){10,  5,   20,  180}

extern const int KEYS_JOGO[NUM_COLUNAS];

Music CarregarMusicaFase      (int fase);

void  DesenharTextoCentralizado(const char *texto, int y, int tam, Color cor);
void  DesenharLinhaDourada     (int y, int margem);
int   DesenharBotao            (const char *texto, int y, int selecionado);
void  DesenharJogo             (EstadoJogo *estado, Texture2D fundo_jogo);
int   DesenharPausa            (int selecao);
void  DesenharPontuacao        (int pontuacao, int fase, const char *lore);
void  DesenharJogoEncerrado    (int pontuacao);
void  DesenharRanking          (EntradaRanking *ranking, int n);
void  DesenharContagem         (int fase, float tempoRestante, Texture2D fundo_jogo);
void  DesenharEntradaNome      (int pontuacao, const char *nomeInput, float tempo);

#endif
