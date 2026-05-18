#include "raylib.h"
#include "logica.h"
#include <math.h>
#include <stdio.h>

// ─── Cores globais ────────────────────────────────────────────────────────────
#define COR_FUNDO    (Color){15,  10,  30,  255}
#define COR_DOURADO  (Color){255, 200, 50,  255}
#define COR_VERMELHO (Color){200, 30,  50,  255}
#define COR_BRANCO   (Color){240, 235, 220, 255}
#define COR_CINZA    (Color){100, 95,  110, 255}
#define COR_OVERLAY  (Color){10,  5,   20,  180}

// ─── Layout das faixas ───────────────────────────────────────────────────────
#define LANE_X_START  200
#define LANE_W        100
#define NOTA_H        20
#define NOTA_MARGEM   12

typedef enum { OPCAO_JOGAR=0, OPCAO_RANKING, OPCAO_SAIR, TOTAL_OPCOES } OpcaoMenu;
typedef enum { TELA_MENU, TELA_JOGO, TELA_RANKING, TELA_SAIR }          TelaAtual;

// Cores e rótulos de cada faixa (instrumento)
static Color       COR_FAIXAS[NUM_COLUNAS]  = {
    {220, 60,  60,  255},   // Alfaia  — vermelho
    {255, 200, 50,  255},   // Gonguê  — dourado
    {60,  200, 110, 255},   // Agbê    — verde
    {100, 150, 240, 255},   // Tarol   — azul
};
static const char *TECLA_LABEL[NUM_COLUNAS] = { "D", "F", "J", "K" };
static const char *INST_LABEL[NUM_COLUNAS]  = { "ALFAIA", "GONGUÊ", "AGBÊ", "TAROL" };
static const int   KEYS_JOGO[NUM_COLUNAS]   = { KEY_D, KEY_F, KEY_J, KEY_K };

// ─── Helpers de desenho ───────────────────────────────────────────────────────
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

// ─── Tela do Jogo ─────────────────────────────────────────────────────────────
static void DesenharJogo(EstadoJogo *estado) {
    int sh = GetScreenHeight();

    // Fundos das faixas e divisórias
    for (int i = 0; i < NUM_COLUNAS; i++) {
        int lx = LANE_X_START + i * LANE_W;
        DrawRectangle(lx, 0, LANE_W, sh, (Color){20, 15, 35, 255});
        DrawLine(lx, 0, lx, sh, (Color){50, 45, 70, 255});
    }
    DrawLine(LANE_X_START + NUM_COLUNAS * LANE_W, 0,
             LANE_X_START + NUM_COLUNAS * LANE_W, sh,
             (Color){50, 45, 70, 255});

    // Linha de acerto
    DrawLine(LANE_X_START, LINHA_ACERTO,
             LANE_X_START + NUM_COLUNAS * LANE_W, LINHA_ACERTO,
             (Color){255, 255, 255, 80});

    // Por coluna: botão de acerto, notas e feedback
    for (int i = 0; i < NUM_COLUNAS; i++) {
        int   lx  = LANE_X_START + i * LANE_W;
        int   cx  = lx + LANE_W / 2;
        Color cor = COR_FAIXAS[i];

        // Glow quando tecla pressionada
        if (IsKeyDown(KEYS_JOGO[i]))
            DrawCircle(cx, LINHA_ACERTO, 38, (Color){cor.r, cor.g, cor.b, 110});

        // Botão circular da linha de acerto
        DrawCircle(cx, LINHA_ACERTO, 30, (Color){cor.r/5, cor.g/5, cor.b/5, 220});
        DrawCircleLines(cx, LINHA_ACERTO, 30, cor);

        // Letra da tecla centralizada no botão
        int tw = MeasureText(TECLA_LABEL[i], 22);
        DrawText(TECLA_LABEL[i], cx - tw/2, LINHA_ACERTO - 11, 22, cor);

        // Notas (percorre a lista encadeada)
        for (Nota *n = estado->colunas[i].cabeca; n; n = n->prox) {
            if (n->y < -NOTA_H || n->y > sh) continue;
            Rectangle r = {lx + NOTA_MARGEM, n->y - NOTA_H/2,
                           LANE_W - 2*NOTA_MARGEM, NOTA_H};
            DrawRectangleRounded(r, 0.5f, 4, cor);
            DrawRectangleRoundedLines(r, 0.5f, 4, WHITE);
        }

        // Feedback textual com fade-out
        if (estado->tempoFeedback[i] > 0.0f) {
            const char *txt  = NULL;
            Color       fcor = WHITE;
            switch (estado->ultimoAcerto[i]) {
                case ACERTO_PERFEITO: txt = "PERFEITO!"; fcor = COR_DOURADO;               break;
                case ACERTO_BOM:      txt = "BOM!";      fcor = (Color){60,200,110,255};   break;
                case ACERTO_ERROU:    txt = "ERROU";     fcor = (Color){220,60,60,255};    break;
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

        // Nome do instrumento abaixo do botão
        int itw = MeasureText(INST_LABEL[i], 11);
        DrawText(INST_LABEL[i], cx - itw/2, LINHA_ACERTO + 38, 11,
                 (Color){cor.r, cor.g, cor.b, 160});
    }

    // HUD: pontuação e combo
    char buf[64];
    snprintf(buf, sizeof(buf), "PONTUACAO: %d", estado->pontuacao);
    DrawText(buf, 10, 16, 22, COR_DOURADO);

    snprintf(buf, sizeof(buf), "COMBO x%d", estado->combo);
    DrawText(buf, 10, 44, 18, COR_BRANCO);

    DesenharTextoCentralizado("ESC — voltar ao menu", 575, 13, COR_CINZA);
}

// ─── Main ─────────────────────────────────────────────────────────────────────
int main(void) {
    const int LARGURA = 800;
    const int ALTURA  = 600;

    InitWindow(LARGURA, ALTURA, "Baque Virado - Maracatu");
    SetTargetFPS(60);

    Texture2D fundo = LoadTexture("assets/fundo.png");

    TelaAtual tela    = TELA_MENU;
    OpcaoMenu selecao = OPCAO_JOGAR;
    float     tempo   = 0.0f;
    int       yBase   = 310, espacamento = 68;

    EstadoJogo jogo     = {0};
    int        jogoVivo = 0;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        tempo += dt;

        // ── Input ──────────────────────────────────────────────────────────────
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

        if (tela == TELA_JOGO) {
            if (!jogoVivo) {
                iniciarJogo(&jogo);
                jogoVivo = 1;
            }
            atualizarJogo(&jogo, dt);

            for (int i = 0; i < NUM_COLUNAS; i++)
                if (IsKeyPressed(KEYS_JOGO[i]))
                    verificarAcerto(&jogo, i);

            if (IsKeyPressed(KEY_ESCAPE)) {
                encerrarJogo(&jogo);
                jogoVivo = 0;
                tela     = TELA_MENU;
            }
        }

        if (tela == TELA_SAIR) break;

        // ── Draw ───────────────────────────────────────────────────────────────
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
            DesenharJogo(&jogo);

        } else if (tela == TELA_RANKING) {
            DesenharTextoCentralizado("RANKING", 200, 40, COR_DOURADO);
            DesenharTextoCentralizado("Nenhuma pontuacao registrada ainda.", 270, 20, COR_BRANCO);
            DesenharTextoCentralizado("Pressione ESC para voltar", 320, 18, COR_CINZA);
            if (IsKeyPressed(KEY_ESCAPE)) tela = TELA_MENU;
        }

        EndDrawing();
        if (tela == TELA_SAIR) break;
    }

    if (jogoVivo) encerrarJogo(&jogo);
    UnloadTexture(fundo);
    CloseWindow();
    return 0;
}
