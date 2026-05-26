#include "raylib.h"
#include "logica.h"
#include "gemini.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define COR_FUNDO    (Color){15,  10,  30,  255}
#define COR_DOURADO  (Color){255, 200, 50,  255}
#define COR_VERMELHO (Color){200, 30,  50,  255}
#define COR_BRANCO   (Color){240, 235, 220, 255}
#define COR_CINZA    (Color){100, 95,  110, 255}
#define COR_OVERLAY  (Color){10,  5,   20,  180}

#define LANE_X_START  200
#define LANE_W        100
#define NOTA_H        20
#define NOTA_MARGEM   12

typedef enum { OPCAO_JOGAR=0, OPCAO_RANKING, OPCAO_SAIR, TOTAL_OPCOES } OpcaoMenu;
typedef enum { TELA_MENU, TELA_JOGO, TELA_PONTUACAO, TELA_ENCERRADO, TELA_RANKING, TELA_SAIR } TelaAtual;

static Color       COR_FAIXAS[NUM_COLUNAS]  = {
    {220, 60,  60,  255},
    {255, 200, 50,  255},
    {60,  200, 110, 255},
    {100, 150, 240, 255},
};
static const char *TECLA_LABEL[NUM_COLUNAS] = { "D", "F", "J", "K" };
static const char *INST_LABEL[NUM_COLUNAS]  = { "ALFAIA", "GONGUÊ", "AGBÊ", "TAROL" };
static const int   KEYS_JOGO[NUM_COLUNAS]   = { KEY_D, KEY_F, KEY_J, KEY_K };

static void DesenharTextoCentralizado(const char *texto, int y, int tam, Color cor) {
    int larg = MeasureText(texto, tam);
    DrawText(texto, (GetScreenWidth() - larg) / 2, y, tam, cor);
}

static void DesenharLinhaDourada(int y, int margem) {
    int w = GetScreenWidth();
    DrawLine(margem, y, w - margem, y, COR_DOURADO);
    DrawCircle(margem,     y, 4, COR_DOURADO);
    DrawCircle(w - margem, y, 4, COR_DOURADO);
}

static int DesenharBotao(const char *texto, int y, int selecionado) {
    int w = GetScreenWidth();
    int bw = 320, bh = 54;
    int x = (w - bw) / 2;
    Rectangle rect  = {x, y, bw, bh};
    Vector2   mouse = GetMousePosition();
    int       hover = CheckCollisionPointRec(mouse, rect);

    if (selecionado || hover) {
        DrawRectangleRounded(rect, 0.3f, 8, COR_DOURADO);
        DrawRectangleRoundedLines(rect, 0.3f, 8, COR_VERMELHO);
        int tw = MeasureText(texto, 26);
        DrawText(texto, x + (bw - tw)/2, y + (bh - 26)/2, 26, COR_FUNDO);
    } else {
        DrawRectangleRounded(rect, 0.3f, 8, (Color){10,5,20,160});
        DrawRectangleRoundedLines(rect, 0.3f, 8, COR_CINZA);
        int tw = MeasureText(texto, 24);
        DrawText(texto, x + (bw - tw)/2, y + (bh - 24)/2, 24, COR_BRANCO);
    }
    return (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

static void DesenharJogo(EstadoJogo *estado, Texture2D fundo_jogo) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    if (fundo_jogo.id > 0) {
        Rectangle src = {0, 0, fundo_jogo.width, fundo_jogo.height};
        Rectangle dst = {0, 0, sw, sh};
        DrawTexturePro(fundo_jogo, src, dst, (Vector2){0, 0}, 0.0f, WHITE);
    }
    DrawRectangle(0, 0, sw, sh, (Color){10, 5, 25, 155});

    for (int i = 0; i < NUM_COLUNAS; i++) {
        int lx = LANE_X_START + i * LANE_W;
        DrawRectangle(lx, 0, LANE_W, sh, (Color){20, 15, 35, 190});
        DrawLine(lx, 0, lx, sh, (Color){50, 45, 70, 255});
    }
    DrawLine(LANE_X_START + NUM_COLUNAS * LANE_W, 0,
             LANE_X_START + NUM_COLUNAS * LANE_W, sh,
             (Color){50, 45, 70, 255});

    DrawLine(LANE_X_START, LINHA_ACERTO,
             LANE_X_START + NUM_COLUNAS * LANE_W, LINHA_ACERTO,
             (Color){255, 255, 255, 80});

    for (int i = 0; i < NUM_COLUNAS; i++) {
        int   lx  = LANE_X_START + i * LANE_W;
        int   cx  = lx + LANE_W / 2;
        Color cor = COR_FAIXAS[i];

        if (estado->tempoFeedback[i] > 0.0f && estado->ultimoAcerto[i] != ACERTO_ERROU) {
            float t = estado->tempoFeedback[i] / FEEDBACK_DURACAO;
            DrawRectangle(lx, 0, LANE_W, sh, (Color){cor.r, cor.g, cor.b, (unsigned char)(t * 45)});
        }

        if (estado->tempoFeedback[i] > 0.0f && estado->ultimoAcerto[i] != ACERTO_ERROU) {
            float t  = estado->tempoFeedback[i] / FEEDBACK_DURACAO;
            float p  = 1.0f - t;
            float r1 = 32.0f + p * 65.0f;
            float r2 = 32.0f + p * 105.0f;
            DrawCircleLines(cx, LINHA_ACERTO, r1, (Color){cor.r, cor.g, cor.b, (unsigned char)(t * 220)});
            DrawCircleLines(cx, LINHA_ACERTO, r2, (Color){cor.r, cor.g, cor.b, (unsigned char)(t * 120)});
            if (estado->ultimoAcerto[i] == ACERTO_PERFEITO)
                DrawCircle(cx, LINHA_ACERTO, 34, (Color){255, 220, 80, (unsigned char)(t * 70)});
            for (int s = 0; s < 8; s++) {
                float a  = s * (3.14159f / 4.0f);
                float d1 = 32.0f;
                float d2 = 32.0f + p * 55.0f;
                DrawLine(cx + (int)(cosf(a)*d1), LINHA_ACERTO + (int)(sinf(a)*d1),
                         cx + (int)(cosf(a)*d2), LINHA_ACERTO + (int)(sinf(a)*d2),
                         (Color){cor.r, cor.g, cor.b, (unsigned char)(t * 200)});
            }
        }

        if (IsKeyDown(KEYS_JOGO[i]))
            DrawCircle(cx, LINHA_ACERTO, 40, (Color){cor.r, cor.g, cor.b, 120});

        float btnR;
        if (IsKeyDown(KEYS_JOGO[i]))
            btnR = 25.0f;
        else
            btnR = 30.0f;
        DrawCircle(cx, LINHA_ACERTO, (int)btnR, (Color){cor.r/5, cor.g/5, cor.b/5, 220});
        DrawCircleLines(cx, LINHA_ACERTO, (int)btnR, cor);

        int tw = MeasureText(TECLA_LABEL[i], 22);
        DrawText(TECLA_LABEL[i], cx - tw/2, LINHA_ACERTO - 11, 22, cor);

        for (Nota *n = estado->colunas[i].cabeca; n; n = n->prox) {
            if (n->y < -NOTA_H || n->y > sh) continue;
            Rectangle r = {lx + NOTA_MARGEM, n->y - NOTA_H/2,
                           LANE_W - 2*NOTA_MARGEM, NOTA_H};
            DrawRectangleRounded(r, 0.5f, 4, cor);
            DrawRectangleRoundedLines(r, 0.5f, 4, WHITE);
        }

        if (estado->tempoFeedback[i] > 0.0f) {
            const char *txt  = NULL;
            Color       fcor = WHITE;
            switch (estado->ultimoAcerto[i]) {
                case ACERTO_PERFEITO: txt = "PERFEITO!"; fcor = COR_DOURADO;             break;
                case ACERTO_BOM:      txt = "BOM!";      fcor = (Color){60,200,110,255}; break;
                case ACERTO_ERROU:    txt = "ERROU";     fcor = (Color){220,60,60,255};  break;
                default: break;
            }
            if (txt) {
                int alpha = (int)(estado->tempoFeedback[i] / FEEDBACK_DURACAO * 255.0f);
                if (alpha > 255) alpha = 255;
                fcor.a = (unsigned char)alpha;
                int ftw = MeasureText(txt, 16);
                DrawText(txt, cx - ftw/2, LINHA_ACERTO - 75, 16, fcor);
            }
        }

        int itw = MeasureText(INST_LABEL[i], 11);
        DrawText(INST_LABEL[i], cx - itw/2, LINHA_ACERTO + 38, 11,
                 (Color){cor.r, cor.g, cor.b, 160});
    }

    char buf[64];
    snprintf(buf, sizeof(buf), "PONTUACAO: %d", estado->pontuacao);
    DrawText(buf, 10, 16, 22, COR_DOURADO);

    snprintf(buf, sizeof(buf), "COMBO x%d", estado->combo);
    DrawText(buf, 10, 44, 18, COR_BRANCO);

    DesenharTextoCentralizado(FASES[estado->faseAtual].titulo, 14, 13, COR_CINZA);
    DesenharTextoCentralizado("ESC — pausar", 575, 13, COR_CINZA);
}

static int DesenharPausa(int selecao) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawRectangle(0, 0, sw, sh, (Color){5, 3, 15, 190});
    DesenharTextoCentralizado("PAUSADO", 180, 44, COR_DOURADO);
    DesenharLinhaDourada(234, 200);

    const char *opcoes[2] = {"  CONTINUAR  ", "  VOLTAR AO MENU  "};
    int acao = -1;
    for (int i = 0; i < 2; i++) {
        if (DesenharBotao(opcoes[i], 270 + i * 70, selecao == i))
            acao = i;
    }

    DesenharTextoCentralizado("SETAS + ENTER  |  ESC para continuar", 445, 13, COR_CINZA);
    return acao;
}

static void DesenharTextoQuebrado(const char *texto, int y, int tam, Color cor, int largMax) {
    int sw      = GetScreenWidth();
    char linha[256];
    int  iLinha = 0;
    int  yAtual = y;

    for (int i = 0; texto[i] != '\0'; i++) {
        linha[iLinha++] = texto[i];
        linha[iLinha]   = '\0';

        int prox = i + 1;
        while (texto[prox] != '\0' && texto[prox] != ' ') prox++;
        char teste[256];
        strncpy(teste, linha, sizeof(teste) - 1);
        teste[sizeof(teste) - 1] = '\0';
        strncat(teste, texto + i + 1, prox - i - 1);

        if (MeasureText(teste, tam) > largMax || texto[i] == '\n') {
            int lw = MeasureText(linha, tam);
            DrawText(linha, (sw - lw) / 2, yAtual, tam, cor);
            yAtual += tam + 4;
            iLinha  = 0;
            linha[0] = '\0';
            if (texto[i] == '\n') continue;
        }
    }
    if (iLinha > 0) {
        int lw = MeasureText(linha, tam);
        DrawText(linha, (sw - lw) / 2, yAtual, tam, cor);
    }
}

static void DesenharPontuacao(int pontuacao, int fase, const char *lore) {
    char titulo[64];
    snprintf(titulo, sizeof(titulo), "FASE %d CONCLUIDA!", fase + 1);

    DesenharLinhaDourada(42, 60);
    DesenharTextoCentralizado(titulo, 54, 32, COR_DOURADO);
    DesenharLinhaDourada(95, 60);
    DesenharTextoCentralizado(FASES[fase].artista, 104, 13, COR_CINZA);

    char buf[32];
    snprintf(buf, sizeof(buf), "%d", pontuacao);
    DesenharTextoCentralizado(buf,      150, 52, COR_DOURADO);
    DesenharTextoCentralizado("PONTOS", 208, 16, COR_BRANCO);

    const char *rating;
    Color       rcor;
    if      (pontuacao >= 5000) { rating = "MESTRE DO MARACATU!"; rcor = COR_DOURADO; }
    else if (pontuacao >= 2000) { rating = "IMPRESSIONANTE!";     rcor = (Color){60,200,110,255}; }
    else if (pontuacao >= 500)  { rating = "BEM JOGADO!";         rcor = COR_BRANCO; }
    else                        { rating = "BOM COMECO!";         rcor = COR_CINZA; }

    DesenharTextoCentralizado(rating, 238, 20, rcor);
    DesenharLinhaDourada(270, 60);

    if (lore && lore[0] != '\0')
        DesenharTextoQuebrado(lore, 282, 13, (Color){200, 190, 230, 220}, 640);

    DesenharLinhaDourada(380, 60);

    if (fase < TOTAL_FASES - 1) {
        DesenharTextoCentralizado("ENTER — proxima fase",   396, 15, COR_DOURADO);
        DesenharTextoCentralizado("R — jogar novamente",    418, 15, COR_CINZA);
        DesenharTextoCentralizado("ESC — voltar ao menu",   440, 15, COR_CINZA);
    } else {
        DesenharTextoCentralizado("ENTER — voltar ao menu", 396, 15, COR_BRANCO);
        DesenharTextoCentralizado("R — jogar novamente",    418, 15, COR_CINZA);
    }
}

static void DesenharJogoEncerrado(int pontuacao) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    for (int i = 0; i < 7; i++)
        DrawCircleLines(sw/2, sh/2, 50 + i*65, (Color){200, 30, 50, 6 + i*3});

    DesenharLinhaDourada(55, 40);
    DesenharTextoCentralizado("JOGO", 68, 52, COR_VERMELHO);
    DesenharTextoCentralizado("ENCERRADO", 126, 52, COR_DOURADO);
    DesenharLinhaDourada(188, 40);

    DesenharTextoCentralizado("Todas as 3 fases concluidas!", 206, 15, COR_CINZA);

    char buf[32];
    snprintf(buf, sizeof(buf), "%d", pontuacao);
    DesenharTextoCentralizado(buf,              250, 68, COR_DOURADO);
    DesenharTextoCentralizado("PONTUACAO FINAL", 328, 16, COR_BRANCO);

    const char *rating;
    Color       rcor;
    if      (pontuacao >= 12000) { rating = "MESTRE DO MARACATU!"; rcor = COR_DOURADO; }
    else if (pontuacao >= 6000)  { rating = "IMPRESSIONANTE!";     rcor = (Color){60, 200, 110, 255}; }
    else if (pontuacao >= 2000)  { rating = "BEM JOGADO!";         rcor = COR_BRANCO; }
    else                         { rating = "CONTINUE TREINANDO!"; rcor = COR_CINZA; }

    DesenharTextoCentralizado(rating, 370, 24, rcor);
    DesenharLinhaDourada(415, 40);

    DesenharTextoCentralizado("ENTER ou ESC — voltar ao menu",  434, 15, COR_BRANCO);
    DesenharTextoCentralizado("R — jogar a fase 3 novamente",   458, 15, COR_CINZA);
}

static void DesenharRanking(EntradaRanking *ranking, int n) {
    DesenharLinhaDourada(60, 60);
    DesenharTextoCentralizado("RANKING", 72, 40, COR_DOURADO);
    DesenharLinhaDourada(122, 60);

    if (n == 0) {
        DesenharTextoCentralizado("Nenhuma pontuacao registrada ainda.", 280, 20, COR_CINZA);
    } else {
        static Color medalha[3] = {
            {255, 200, 50,  255},
            {200, 200, 210, 255},
            {200, 120, 60,  255},
        };
        int yInicio = 150, passo = 40;

        for (int i = 0; i < n; i++) {
            Color cor;
            if (i < 3)
                cor = medalha[i];
            else
                cor = COR_BRANCO;

            if (i < 3) {
                Rectangle fundo = {100, yInicio + i*passo - 6, 600, 34};
                DrawRectangleRounded(fundo, 0.3f, 4, (Color){cor.r, cor.g, cor.b, 25});
                DrawRectangleRoundedLines(fundo, 0.3f, 4, (Color){cor.r, cor.g, cor.b, 60});
            }

            char pos[16];
            snprintf(pos, sizeof(pos), "#%d", i + 1);
            DrawText(pos, 130, yInicio + i*passo, 22, cor);

            char pts[32];
            snprintf(pts, sizeof(pts), "%d PTS", ranking[i].pontuacao);
            int tw = MeasureText(pts, 22);
            DrawText(pts, 670 - tw, yInicio + i*passo, 22, cor);

            if (i < n - 1)
                DrawLine(130, yInicio + i*passo + 32, 670, yInicio + i*passo + 32,
                         (Color){60, 55, 80, 255});
        }
    }

    DesenharTextoCentralizado("ESC — voltar ao menu", 560, 14, COR_CINZA);
}

static Music CarregarMusicaFase(int fase) {
    Music m = LoadMusicStream(FASES[fase].arquivo);
    m.looping = false;
    return m;
}

int main(void) {
    const int LARGURA = 800;
    const int ALTURA  = 600;

    srand((unsigned int)time(NULL));
    InitWindow(LARGURA, ALTURA, "Baque Virado - Maracatu");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    InitAudioDevice();

    Texture2D fundo       = LoadTexture("assets/fundo.png");
    Texture2D fundo_jogo  = LoadTexture("assets/fundo_jogo.png");
    Music     musica      = CarregarMusicaFase(0);

    TelaAtual tela    = TELA_MENU;
    OpcaoMenu selecao = OPCAO_JOGAR;
    float     tempo   = 0.0f;
    int       yBase   = 310, espacamento = 68;

    EstadoJogo     jogo           = {0};
    int            jogoVivo       = 0;
    int            jogoEmPausa    = 0;
    int            selecaoPausa   = 0;
    int            pontuacaoFinal = 0;
    int            faseAtual      = 0;
    EntradaRanking ranking[MAX_RANKING] = {0};
    int            numScores            = 0;
    carregarRanking(ranking, &numScores);
    char           loreFase[1024]       = {0};

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        tempo += dt;

        if (tela == TELA_RANKING && IsKeyPressed(KEY_ESCAPE))
            tela = TELA_MENU;

        if (tela == TELA_MENU) {
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
                selecao = (selecao + 1) % TOTAL_OPCOES;
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
                selecao = (selecao - 1 + TOTAL_OPCOES) % TOTAL_OPCOES;
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                if (selecao == OPCAO_JOGAR)   tela = TELA_JOGO;
                if (selecao == OPCAO_RANKING) tela = TELA_RANKING;
                if (selecao == OPCAO_SAIR)    tela = TELA_SAIR;
            }
        }

        if (tela == TELA_PONTUACAO) {
            if (IsKeyPressed(KEY_ENTER)) {
                if (faseAtual < TOTAL_FASES - 1) {
                    faseAtual++;
                    UnloadMusicStream(musica);
                    musica = CarregarMusicaFase(faseAtual);
                    iniciarJogo(&jogo, faseAtual);
                    jogoVivo = 1; jogoEmPausa = 0;
                    PlayMusicStream(musica);
                    tela = TELA_JOGO;
                } else {
                    faseAtual = 0;
                    tela = TELA_MENU;
                }
            }
            if (IsKeyPressed(KEY_R)) {
                UnloadMusicStream(musica);
                musica = CarregarMusicaFase(faseAtual);
                iniciarJogo(&jogo, faseAtual);
                jogoVivo = 1; jogoEmPausa = 0;
                PlayMusicStream(musica);
                tela = TELA_JOGO;
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                faseAtual = 0;
                tela = TELA_MENU;
            }
        }

        if (tela == TELA_ENCERRADO) {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
                faseAtual = 0;
                tela = TELA_MENU;
            }
            if (IsKeyPressed(KEY_R)) {
                UnloadMusicStream(musica);
                musica = CarregarMusicaFase(faseAtual);
                iniciarJogo(&jogo, faseAtual);
                jogoVivo = 1; jogoEmPausa = 0;
                PlayMusicStream(musica);
                tela = TELA_JOGO;
            }
        }

        if (tela == TELA_JOGO) {
            if (!jogoVivo) {
                faseAtual = 0;
                UnloadMusicStream(musica);
                musica = CarregarMusicaFase(faseAtual);
                iniciarJogo(&jogo, faseAtual);
                jogoVivo = 1; jogoEmPausa = 0; selecaoPausa = 0;
                PlayMusicStream(musica);
            }

            if (jogoEmPausa) {
                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
                    selecaoPausa = (selecaoPausa + 1) % 2;
                if (IsKeyPressed(KEY_UP)   || IsKeyPressed(KEY_W))
                    selecaoPausa = (selecaoPausa + 1) % 2;
                if (IsKeyPressed(KEY_ESCAPE)) {
                    jogoEmPausa = 0;
                    ResumeMusicStream(musica);
                }
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    if (selecaoPausa == 0) {
                        jogoEmPausa = 0;
                        ResumeMusicStream(musica);
                    } else {
                        adicionarScore(ranking, &numScores, jogo.pontuacao);
                        salvarRanking(ranking, numScores);
                        encerrarJogo(&jogo);
                        StopMusicStream(musica);
                        jogoVivo = jogoEmPausa = selecaoPausa = 0;
                        faseAtual = 0;
                        tela = TELA_MENU;
                    }
                }
            } else {
                UpdateMusicStream(musica);
                atualizarJogo(&jogo, dt);

                for (int i = 0; i < NUM_COLUNAS; i++)
                    if (IsKeyPressed(KEYS_JOGO[i]))
                        verificarAcerto(&jogo, i);

                if (faseConcluida(&jogo)) {
                    StopMusicStream(musica);
                    pontuacaoFinal = jogo.pontuacao;
                    adicionarScore(ranking, &numScores, pontuacaoFinal);
                    salvarRanking(ranking, numScores);
                    encerrarJogo(&jogo);
                    jogoVivo = 0;
                    if (faseAtual == TOTAL_FASES - 1) {
                        tela = TELA_ENCERRADO;
                    } else {
                        loreFase[0] = '\0';
                        chamarGemini(FASES[faseAtual].titulo, FASES[faseAtual].artista,
                                         FASES[faseAtual].lore, loreFase, sizeof(loreFase));
                        tela = TELA_PONTUACAO;
                    }
                }

                if (IsKeyPressed(KEY_ESCAPE)) {
                    jogoEmPausa  = 1;
                    selecaoPausa = 0;
                    PauseMusicStream(musica);
                }
            }
        }

        if (tela == TELA_SAIR) break;

        BeginDrawing();
        ClearBackground(COR_FUNDO);

        if (tela == TELA_MENU) {
            if (fundo.id > 0) {
                Rectangle src = {0, 0, fundo.width, fundo.height};
                Rectangle dst = {0, 0, LARGURA, ALTURA};
                DrawTexturePro(fundo, src, dst, (Vector2){0,0}, 0.0f, WHITE);
            }
            DrawRectangle(0, 0, LARGURA, ALTURA, COR_OVERLAY);

            for (int i = 0; i < 5; i++)
                DrawCircleLines(LARGURA/2, ALTURA/2, 80 + i*60,
                                (Color){255, 200, 50, 12 + i*4});

            DesenharLinhaDourada(75, 60);
            DesenharTextoCentralizado("BAQUE VIRADO", 90, 52, COR_DOURADO);
            DesenharTextoCentralizado("M  A  R  A  C  A  T  U", 150, 18, COR_VERMELHO);
            DesenharLinhaDourada(188, 60);
            DesenharTextoCentralizado("Recife, Pernambuco", 205, 16, COR_CINZA);

            const char *rotulos[TOTAL_OPCOES] = {"  JOGAR  ", "  RANKING  ", "  SAIR  "};
            for (int i = 0; i < TOTAL_OPCOES; i++) {
                int clicado = DesenharBotao(rotulos[i], yBase + i*espacamento, (int)selecao == i);
                if (clicado) {
                    if (i == OPCAO_JOGAR)   tela = TELA_JOGO;
                    if (i == OPCAO_RANKING) tela = TELA_RANKING;
                    if (i == OPCAO_SAIR)    tela = TELA_SAIR;
                }
            }

            DesenharLinhaDourada(530, 60);
            DesenharTextoCentralizado(
                "Use SETAS ou MOUSE para navegar  |  ENTER para confirmar",
                545, 14, COR_CINZA);

        } else if (tela == TELA_JOGO) {
            DesenharJogo(&jogo, fundo_jogo);
            if (jogoEmPausa) {
                int acao = DesenharPausa(selecaoPausa);
                if (acao == 0) {
                    jogoEmPausa = 0;
                    ResumeMusicStream(musica);
                } else if (acao == 1) {
                    adicionarScore(ranking, &numScores, jogo.pontuacao);
                    salvarRanking(ranking, numScores);
                    encerrarJogo(&jogo);
                    StopMusicStream(musica);
                    jogoVivo = jogoEmPausa = selecaoPausa = 0;
                    faseAtual = 0;
                    tela = TELA_MENU;
                }
            }

        } else if (tela == TELA_PONTUACAO) {
            DesenharPontuacao(pontuacaoFinal, faseAtual, loreFase);

        } else if (tela == TELA_ENCERRADO) {
            DesenharJogoEncerrado(pontuacaoFinal);

        } else if (tela == TELA_RANKING) {
            DesenharRanking(ranking, numScores);
        }

        EndDrawing();
        if (tela == TELA_SAIR) break;
    }

    if (jogoVivo) encerrarJogo(&jogo);
    UnloadMusicStream(musica);
    UnloadTexture(fundo);
    UnloadTexture(fundo_jogo);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
