#include "raylib.h"
#include "logica.h"
#include "gemini.h"
#include "render.h"
#include <string.h>
#include <time.h>

typedef enum { OPCAO_JOGAR=0, OPCAO_RANKING, OPCAO_SAIR, TOTAL_OPCOES } OpcaoMenu;
typedef enum { TELA_MENU, TELA_CONTAGEM, TELA_JOGO, TELA_PONTUACAO, TELA_ENCERRADO, TELA_NOME, TELA_RANKING, TELA_SAIR } TelaAtual;

int main(void) {
    const int LARGURA = 800;
    const int ALTURA  = 600;

    srand((unsigned int)time(NULL));
    InitWindow(LARGURA, ALTURA, "Baque Virado - Maracatu");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    InitAudioDevice();

    Texture2D fundo      = LoadTexture("assets/fundo.png");
    Texture2D fundo_jogo = LoadTexture("assets/fundo_jogo.png");
    Music     musica     = CarregarMusicaFase(0);

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
    float          tempoContagem  = 0.0f;
    char           nomeInput[MAX_NOME + 1] = {0};
    int            nomeLen        = 0;
    EntradaRanking ranking[MAX_RANKING]    = {0};
    int            numScores               = 0;
    carregarRanking(ranking, &numScores);
    char           loreFase[1024]          = {0};

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
                if (selecao == OPCAO_JOGAR) {
                    faseAtual = 0;
                    UnloadMusicStream(musica);
                    musica = CarregarMusicaFase(faseAtual);
                    iniciarJogo(&jogo, faseAtual);
                    jogoVivo = 1; jogoEmPausa = 0; selecaoPausa = 0;
                    tempoContagem = 4.0f;
                    tela = TELA_CONTAGEM;
                }
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
                    tempoContagem = 4.0f;
                    tela = TELA_CONTAGEM;
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
                tempoContagem = 4.0f;
                tela = TELA_CONTAGEM;
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                faseAtual = 0;
                tela = TELA_MENU;
            }
        }

        if (tela == TELA_ENCERRADO) {
            if (IsKeyPressed(KEY_ENTER)) {
                nomeInput[0] = '\0'; nomeLen = 0;
                tela = TELA_NOME;
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                adicionarScore(ranking, &numScores, pontuacaoFinal, "");
                salvarRanking(ranking, numScores);
                faseAtual = 0;
                tela = TELA_MENU;
            }
            if (IsKeyPressed(KEY_R)) {
                adicionarScore(ranking, &numScores, pontuacaoFinal, "");
                salvarRanking(ranking, numScores);
                UnloadMusicStream(musica);
                musica = CarregarMusicaFase(faseAtual);
                iniciarJogo(&jogo, faseAtual);
                jogoVivo = 1; jogoEmPausa = 0;
                tempoContagem = 4.0f;
                tela = TELA_CONTAGEM;
            }
        }
        else if (tela == TELA_NOME) {
            int ch;
            while ((ch = GetCharPressed()) != 0) {
                if (ch >= 32 && ch < 127 && nomeLen < MAX_NOME) {
                    nomeInput[nomeLen++] = (char)ch;
                    nomeInput[nomeLen]   = '\0';
                }
            }
            if (IsKeyPressed(KEY_BACKSPACE) && nomeLen > 0)
                nomeInput[--nomeLen] = '\0';
            if (IsKeyPressed(KEY_ENTER)) {
                if (nomeLen > 0)
                    adicionarScore(ranking, &numScores, pontuacaoFinal, nomeInput);
                else
                    adicionarScore(ranking, &numScores, pontuacaoFinal, "");
                salvarRanking(ranking, numScores);
                faseAtual = 0;
                tela = TELA_RANKING;
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                adicionarScore(ranking, &numScores, pontuacaoFinal, "");
                salvarRanking(ranking, numScores);
                faseAtual = 0;
                tela = TELA_MENU;
            }
        }

        if (tela == TELA_CONTAGEM) {
            tempoContagem -= dt;
            if (tempoContagem <= 0.0f) {
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
                tempoContagem = 4.0f;
                tela = TELA_CONTAGEM;
            } else if (jogoEmPausa) {
                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
                    selecaoPausa = (selecaoPausa + 1) % 2;
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
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
                        adicionarScore(ranking, &numScores, jogo.pontuacao, "");
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
                    encerrarJogo(&jogo);
                    jogoVivo = 0;
                    if (faseAtual == TOTAL_FASES - 1) {
                        tela = TELA_ENCERRADO;
                    } else {
                        adicionarScore(ranking, &numScores, pontuacaoFinal, "");
                        salvarRanking(ranking, numScores);
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
                DrawCircleLines(LARGURA/2, ALTURA/2, 80 + i*60, (Color){255, 200, 50, 12 + i*4});

            DesenharLinhaDourada(75, 60);
            DesenharTextoCentralizado("BAQUE VIRADO", 90, 52, COR_DOURADO);
            DesenharTextoCentralizado("M  A  R  A  C  A  T  U", 150, 18, COR_VERMELHO);
            DesenharLinhaDourada(188, 60);
            DesenharTextoCentralizado("Recife, Pernambuco", 205, 16, COR_CINZA);

            const char *rotulos[TOTAL_OPCOES] = {"  JOGAR  ", "  RANKING  ", "  SAIR  "};
            for (int i = 0; i < TOTAL_OPCOES; i++) {
                int clicado = DesenharBotao(rotulos[i], yBase + i*espacamento, (int)selecao == i);
                if (clicado) {
                    if (i == OPCAO_JOGAR) {
                        faseAtual = 0;
                        UnloadMusicStream(musica);
                        musica = CarregarMusicaFase(faseAtual);
                        iniciarJogo(&jogo, faseAtual);
                        jogoVivo = 1; jogoEmPausa = 0; selecaoPausa = 0;
                        tempoContagem = 4.0f;
                        tela = TELA_CONTAGEM;
                    }
                    if (i == OPCAO_RANKING) tela = TELA_RANKING;
                    if (i == OPCAO_SAIR)    tela = TELA_SAIR;
                }
            }

            DesenharLinhaDourada(530, 60);
            DesenharTextoCentralizado(
                "Use SETAS ou MOUSE para navegar  |  ENTER para confirmar",
                545, 14, COR_CINZA);

        } else if (tela == TELA_CONTAGEM) {
            DesenharContagem(faseAtual, tempoContagem, fundo_jogo);

        } else if (tela == TELA_JOGO) {
            DesenharJogo(&jogo, fundo_jogo);
            if (jogoEmPausa) {
                int acao = DesenharPausa(selecaoPausa);
                if (acao == 0) {
                    jogoEmPausa = 0;
                    ResumeMusicStream(musica);
                } else if (acao == 1) {
                    adicionarScore(ranking, &numScores, jogo.pontuacao, "");
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
            DesenharTextoCentralizado("ENTER — salvar nome no ranking",  480, 15, COR_DOURADO);
            DesenharTextoCentralizado("ESC — voltar ao menu sem salvar", 502, 14, COR_CINZA);

        } else if (tela == TELA_NOME) {
            DesenharEntradaNome(pontuacaoFinal, nomeInput, tempo);

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
