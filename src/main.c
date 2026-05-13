#include "raylib.h"
#include <string.h>
#include <math.h>

#define COR_FUNDO       (Color){15, 10, 30, 255}
#define COR_DOURADO     (Color){255, 200, 50, 255}
#define COR_VERMELHO    (Color){200, 30, 50, 255}
#define COR_BRANCO      (Color){240, 235, 220, 255}
#define COR_CINZA       (Color){100, 95, 110, 255}
#define COR_SELECIONADO (Color){255, 220, 80, 255}

typedef enum { OPCAO_JOGAR = 0, OPCAO_RANKING, OPCAO_SAIR, TOTAL_OPCOES } OpcaoMenu;
typedef enum { TELA_MENU, TELA_JOGO, TELA_RANKING, TELA_SAIR } TelaAtual;

void DesenharTextoCentralizado(const char *texto, int y, int tamanho, Color cor) {
    int largura = MeasureText(texto, tamanho);
    DrawText(texto, (GetScreenWidth() - largura) / 2, y, tamanho, cor);
}

void DesenharLinhaDourada(int y, int margem) {
    int w = GetScreenWidth();
    DrawLine(margem, y, w - margem, y, COR_DOURADO);
    DrawCircle(margem, y, 4, COR_DOURADO);
    DrawCircle(w - margem, y, 4, COR_DOURADO);
}

int DesenharBotao(const char *texto, int y, int selecionado, int hover __attribute__((unused))) {
    int w = GetScreenWidth();
    int larguraBotao = 320;
    int alturaBotao  = 54;
    int x = (w - larguraBotao) / 2;

    Rectangle rect = {x, y, larguraBotao, alturaBotao};

    Vector2 mouse = GetMousePosition();
    int dentroDoRetangulo = CheckCollisionPointRec(mouse, rect);

    if (selecionado || dentroDoRetangulo) {
        DrawRectangleRounded(rect, 0.3f, 8, COR_DOURADO);
        DrawRectangleRoundedLines(rect, 0.3f, 8, COR_VERMELHO);
        int larguraTexto = MeasureText(texto, 26);
        DrawText(texto, x + (larguraBotao - larguraTexto) / 2,
                 y + (alturaBotao - 26) / 2, 26, COR_FUNDO);
    } else {
        DrawRectangleRoundedLines(rect, 0.3f, 8, COR_CINZA);
        int larguraTexto = MeasureText(texto, 24);
        DrawText(texto, x + (larguraBotao - larguraTexto) / 2,
                 y + (alturaBotao - 24) / 2, 24, COR_BRANCO);
    }

    return (dentroDoRetangulo && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

int main(void) {
    const int LARGURA  = 800;
    const int ALTURA   = 600;

    InitWindow(LARGURA, ALTURA, "Baque Virado - Maracatu");
    SetTargetFPS(60);

    TelaAtual tela     = TELA_MENU;
    OpcaoMenu selecao  = OPCAO_JOGAR;
    float tempo        = 0.0f;

    int yBase   = 300;
    int espacamento = 70;

    while (!WindowShouldClose()) {

        tempo += GetFrameTime();

        if (tela == TELA_MENU) {
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                selecao = (selecao + 1) % TOTAL_OPCOES;
            }
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                selecao = (selecao - 1 + TOTAL_OPCOES) % TOTAL_OPCOES;
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                if (selecao == OPCAO_JOGAR)   tela = TELA_JOGO;
                if (selecao == OPCAO_RANKING) tela = TELA_RANKING;
                if (selecao == OPCAO_SAIR)    tela = TELA_SAIR;
            }
        }

        if (tela == TELA_SAIR) break;

        BeginDrawing();
        ClearBackground(COR_FUNDO);

        if (tela == TELA_MENU) {

            float pulso = 1.0f + 0.02f * sinf(tempo * 2.5f);
            (void)pulso;

            for (int i = 0; i < 5; i++) {
                DrawCircleLines(LARGURA / 2, ALTURA / 2,
                                80 + i * 60,
                                (Color){255, 200, 50, 15 + i * 5});
            }

            DesenharLinhaDourada(80, 60);

            DesenharTextoCentralizado("BAQUE VIRADO", 100, 52, COR_DOURADO);
            DesenharTextoCentralizado("M  A  R  A  C  A  T  U", 160, 18, COR_VERMELHO);

            DesenharLinhaDourada(200, 60);

            DesenharTextoCentralizado("Recife, Pernambuco", 220, 16, COR_CINZA);

            const char *rotulos[TOTAL_OPCOES] = {"  JOGAR  ", "  RANKING  ", "  SAIR  "};

            for (int i = 0; i < TOTAL_OPCOES; i++) {
                int y = yBase + i * espacamento;
                int clicado = DesenharBotao(rotulos[i], y, selecao == (OpcaoMenu)i, 0);
                if (clicado) {
                    if (i == OPCAO_JOGAR)   tela = TELA_JOGO;
                    if (i == OPCAO_RANKING) tela = TELA_RANKING;
                    if (i == OPCAO_SAIR)    tela = TELA_SAIR;
                }
            }

            DesenharLinhaDourada(520, 60);
            DesenharTextoCentralizado("Use SETAS ou MOUSE para navegar  |  ENTER para confirmar",
                                      535, 14, COR_CINZA);

        } else if (tela == TELA_JOGO) {
            DesenharTextoCentralizado("JOGO EM CONSTRUCAO...", 270, 30, COR_DOURADO);
            DesenharTextoCentralizado("Pressione ESC para voltar", 320, 18, COR_CINZA);
            if (IsKeyPressed(KEY_ESCAPE)) tela = TELA_MENU;

        } else if (tela == TELA_RANKING) {
            DesenharTextoCentralizado("RANKING", 200, 40, COR_DOURADO);
            DesenharTextoCentralizado("Nenhuma pontuacao registrada ainda.", 270, 20, COR_BRANCO);
            DesenharTextoCentralizado("Pressione ESC para voltar", 320, 18, COR_CINZA);
            if (IsKeyPressed(KEY_ESCAPE)) tela = TELA_MENU;
        }

        EndDrawing();

        if (tela == TELA_SAIR) break;
    }

    CloseWindow();
    return 0;
}
