#include "logica.h"

typedef struct { int coluna; float tempo; } EventoNota;

static const EventoNota MAPA[] = {
    {0, 1.645f},  {2, 2.303f},  {1, 2.962f},  {3, 3.621f},
    {0, 4.279f},  {3, 4.938f},  {2, 5.596f},  {1, 6.255f},
    {0, 6.914f},  {1, 7.572f},  {2, 8.231f},  {3, 8.89f},
    {2, 9.548f},  {0, 10.207f}, {3, 10.865f}, {1, 11.524f},
    {0, 12.183f}, {2, 12.841f}, {1, 13.5f},   {3, 14.158f},
    {0, 14.817f}, {3, 15.476f}, {2, 16.134f}, {1, 16.793f},
    {0, 17.452f}, {1, 18.11f},  {2, 18.769f}, {3, 19.427f},
    {2, 20.086f}, {0, 20.745f}, {3, 21.403f}, {1, 22.062f},
    {0, 22.721f}, {2, 23.379f}, {1, 24.038f}, {3, 24.696f},
    {0, 25.355f}, {3, 26.014f}, {2, 26.672f}, {1, 27.331f},
    {0, 27.989f}, {1, 28.648f}, {2, 29.307f}, {3, 29.965f},
    {2, 30.624f}, {0, 31.283f}, {3, 31.941f}, {1, 32.6f},
    {0, 33.258f}, {2, 33.917f}, {1, 34.576f}, {3, 35.234f},
    {0, 35.893f}, {3, 36.551f}, {2, 37.21f},  {1, 37.869f},
    {0, 38.527f}, {1, 39.186f}, {2, 39.845f}, {3, 40.503f},
    {2, 41.162f}, {0, 41.82f},  {3, 42.479f}, {1, 43.138f},
    {0, 43.796f}, {2, 44.455f}, {1, 45.113f}, {3, 45.772f},
    {0, 46.431f}, {3, 47.089f}, {2, 47.748f}, {1, 48.407f},
    {0, 49.065f}, {1, 49.724f}, {2, 50.382f}, {3, 51.041f},
    {2, 51.7f},   {0, 52.358f}, {3, 53.017f}, {1, 53.675f},
    {0, 54.334f}, {2, 54.993f}, {1, 55.651f}, {3, 56.31f},
    {0, 56.969f}, {3, 57.627f}, {2, 58.286f}, {1, 58.944f},
    {0, 59.603f}, {1, 60.262f}, {2, 60.92f},  {3, 61.579f},
    {2, 62.238f}, {0, 62.896f}, {3, 63.555f}, {1, 64.213f},
    {0, 64.872f}, {2, 65.531f}, {1, 66.189f}, {3, 66.848f},
    {0, 67.506f}, {3, 68.165f}, {2, 68.824f}, {1, 69.482f},
    {0, 70.141f}, {1, 70.8f},   {2, 71.458f}, {3, 72.117f},
    {2, 72.775f}, {0, 73.434f}, {3, 74.093f}, {1, 74.751f},
    {0, 75.41f},  {2, 76.068f}, {1, 76.727f}, {3, 77.386f},
    {0, 78.044f}, {3, 78.703f}, {2, 79.362f},
};

#define TOTAL_EVENTOS ((int)(sizeof(MAPA) / sizeof(MAPA[0])))

void iniciarJogo(EstadoJogo *estado) {
    for (int i = 0; i < NUM_COLUNAS; i++) {
        estado->colunas[i]       = criarLista();
        estado->ultimoAcerto[i]  = ACERTO_NENHUM;
        estado->tempoFeedback[i] = 0.0f;
    }
    estado->pontuacao    = 0;
    estado->combo        = 0;
    estado->tempoJogo    = 0.0f;
    estado->idxSequencia = 0;
}

void atualizarJogo(EstadoJogo *estado, float dt) {
    estado->tempoJogo += dt;

    while (estado->idxSequencia < TOTAL_EVENTOS) {
        float tempoSpawn = MAPA[estado->idxSequencia].tempo - TEMPO_QUEDA;
        if (estado->tempoJogo >= tempoSpawn) {
            int col = MAPA[estado->idxSequencia].coluna;
            inserirNota(&estado->colunas[col], NOTE_START_Y);
            estado->idxSequencia++;
        } else {
            break;
        }
    }

    for (int i = 0; i < NUM_COLUNAS; i++) {
        atualizarNotas(&estado->colunas[i], dt, VELOCIDADE_NOTA);

        while (!listaVazia(&estado->colunas[i]) &&
               estado->colunas[i].cabeca->y > LINHA_ACERTO + JANELA_ACERTO) {
            removerNota(&estado->colunas[i]);
            estado->combo = 0;
        }

        if (estado->tempoFeedback[i] > 0.0f)
            estado->tempoFeedback[i] -= dt;
    }
}

TipoAcerto verificarAcerto(EstadoJogo *estado, int coluna) {
    if (listaVazia(&estado->colunas[coluna])) {
        estado->combo = 0;
        estado->ultimoAcerto[coluna]  = ACERTO_ERROU;
        estado->tempoFeedback[coluna] = FEEDBACK_DURACAO;
        return ACERTO_ERROU;
    }

    float dist = estado->colunas[coluna].cabeca->y - (float)LINHA_ACERTO;
    if (dist < 0.0f) dist = -dist;

    TipoAcerto resultado;

    if (dist <= JANELA_ACERTO * 0.4f) {
        resultado = ACERTO_PERFEITO;
        estado->combo++;
        int mult = estado->combo > 8 ? 8 : estado->combo;
        estado->pontuacao += 100 * mult;
    } else if (dist <= (float)JANELA_ACERTO) {
        resultado = ACERTO_BOM;
        estado->combo++;
        int mult = estado->combo > 8 ? 8 : estado->combo;
        estado->pontuacao += 50 * mult;
    } else {
        estado->combo = 0;
        estado->ultimoAcerto[coluna]  = ACERTO_ERROU;
        estado->tempoFeedback[coluna] = FEEDBACK_DURACAO;
        return ACERTO_ERROU;
    }

    removerNota(&estado->colunas[coluna]);
    estado->ultimoAcerto[coluna]  = resultado;
    estado->tempoFeedback[coluna] = FEEDBACK_DURACAO;
    return resultado;
}

void encerrarJogo(EstadoJogo *estado) {
    for (int i = 0; i < NUM_COLUNAS; i++)
        limparLista(&estado->colunas[i]);
}

void ordenarRanking(EntradaRanking *ranking, int n) {
    for (int i = 1; i < n; i++) {
        EntradaRanking chave = ranking[i];
        int j = i - 1;
        while (j >= 0 && ranking[j].pontuacao < chave.pontuacao) {
            ranking[j + 1] = ranking[j];
            j--;
        }
        ranking[j + 1] = chave;
    }
}

void adicionarScore(EntradaRanking *ranking, int *n, int pontuacao) {
    if (pontuacao <= 0) return;
    if (*n < MAX_RANKING) {
        ranking[(*n)++].pontuacao = pontuacao;
    } else if (pontuacao > ranking[*n - 1].pontuacao) {
        ranking[*n - 1].pontuacao = pontuacao;
    } else {
        return;
    }
    ordenarRanking(ranking, *n);
}
