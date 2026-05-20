#include "logica.h"

const InfoFase FASES[TOTAL_FASES] = {
    {"FASE 1 — Maracatu Atomico", "assets/maracatu_atomico.mp3", "Chico Science & Nacao Zumbi"},
    {"FASE 2 — Sangue de Bairro", "assets/fase2.mp3",            "Nacao Zumbi"},
};

typedef struct { int coluna; float tempo; } EventoNota;

static const EventoNota MAPA_FASE1[] = {
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

static const EventoNota MAPA_FASE2[] = {
    {0, 1.773f},  {1, 2.153f},  {2, 2.534f},  {3, 2.915f},
    {0, 3.296f},  {2, 3.677f},  {3, 4.058f},  {1, 4.439f},
    {2, 4.82f},   {0, 5.201f},  {1, 5.582f},  {3, 5.963f},
    {0, 6.344f},  {3, 6.725f},  {1, 7.106f},  {2, 7.487f},
    {1, 7.868f},  {0, 8.249f},  {3, 8.63f},   {2, 9.011f},
    {0, 9.392f},  {1, 9.773f},  {3, 10.153f}, {2, 10.534f},
    {0, 10.915f}, {1, 11.296f}, {2, 11.677f}, {3, 12.058f},
    {0, 12.439f}, {2, 12.82f},  {3, 13.201f}, {1, 13.582f},
    {2, 13.963f}, {0, 14.344f}, {1, 14.725f}, {3, 15.106f},
    {0, 15.487f}, {3, 15.868f}, {1, 16.249f}, {2, 16.63f},
    {1, 17.011f}, {0, 17.392f}, {3, 17.773f}, {2, 18.153f},
    {0, 18.534f}, {1, 18.915f}, {3, 19.296f}, {2, 19.677f},
    {0, 20.058f}, {1, 20.439f}, {2, 20.82f},  {3, 21.201f},
    {0, 21.582f}, {2, 21.963f}, {3, 22.344f}, {1, 22.725f},
    {2, 23.106f}, {0, 23.487f}, {1, 23.868f}, {3, 24.249f},
    {0, 24.63f},  {3, 25.011f}, {1, 25.392f}, {2, 25.773f},
    {1, 26.153f}, {0, 26.534f}, {3, 26.915f}, {2, 27.296f},
    {0, 27.677f}, {1, 28.058f}, {3, 28.439f}, {2, 28.82f},
    {0, 29.201f}, {1, 29.582f}, {2, 29.963f}, {3, 30.344f},
    {0, 30.725f}, {2, 31.106f}, {3, 31.487f}, {1, 31.868f},
    {2, 32.249f}, {0, 32.63f},  {1, 33.011f}, {3, 33.392f},
    {0, 33.773f}, {3, 34.153f}, {1, 34.534f}, {2, 34.915f},
    {1, 35.296f}, {0, 35.677f}, {3, 36.058f}, {2, 36.439f},
    {0, 36.82f},  {1, 37.201f}, {3, 37.582f}, {2, 37.963f},
    {0, 38.344f}, {1, 38.725f}, {2, 39.106f}, {3, 39.487f},
    {0, 39.868f}, {2, 40.249f}, {3, 40.63f},  {1, 41.011f},
    {2, 41.392f}, {0, 41.773f}, {1, 42.153f}, {3, 42.534f},
    {0, 42.915f}, {3, 43.296f}, {1, 43.677f}, {2, 44.058f},
    {1, 44.439f}, {0, 44.82f},  {3, 45.201f}, {2, 45.582f},
    {0, 45.963f}, {1, 46.344f}, {3, 46.725f}, {2, 47.106f},
    {0, 47.487f}, {1, 47.868f}, {2, 48.249f}, {3, 48.63f},
    {0, 49.011f}, {2, 49.392f}, {3, 49.773f}, {1, 50.153f},
    {2, 50.534f}, {0, 50.915f}, {1, 51.296f}, {3, 51.677f},
    {0, 52.058f}, {3, 52.439f}, {1, 52.82f},  {2, 53.201f},
    {1, 53.582f}, {0, 53.963f}, {3, 54.344f}, {2, 54.725f},
    {0, 55.106f}, {1, 55.487f}, {3, 55.868f}, {2, 56.249f},
    {0, 56.63f},  {1, 57.011f}, {2, 57.392f}, {3, 57.773f},
    {0, 58.153f}, {2, 58.534f}, {3, 58.915f}, {1, 59.296f},
    {2, 59.677f}, {0, 60.058f}, {1, 60.439f}, {3, 60.82f},
    {0, 61.201f}, {3, 61.582f}, {1, 61.963f}, {2, 62.344f},
    {1, 62.725f}, {0, 63.106f}, {3, 63.487f}, {2, 63.868f},
    {0, 64.249f}, {1, 64.63f},  {3, 65.011f}, {2, 65.392f},
    {0, 65.773f}, {1, 66.153f}, {2, 66.534f}, {3, 66.915f},
    {0, 67.296f}, {2, 67.677f}, {3, 68.058f}, {1, 68.439f},
    {2, 68.82f},  {0, 69.201f}, {1, 69.582f}, {3, 69.963f},
    {0, 70.344f}, {3, 70.725f}, {1, 71.106f}, {2, 71.487f},
    {1, 71.868f}, {0, 72.249f}, {3, 72.63f},  {2, 73.011f},
    {0, 73.392f}, {1, 73.773f}, {3, 74.153f}, {2, 74.534f},
    {0, 74.915f}, {1, 75.296f}, {2, 75.677f}, {3, 76.058f},
    {0, 76.439f}, {2, 76.82f},  {3, 77.201f}, {1, 77.582f},
    {2, 77.963f}, {0, 78.344f}, {1, 78.725f}, {3, 79.106f},
    {0, 79.487f}, {3, 79.868f},
};

static const EventoNota *MAPAS[TOTAL_FASES] = { MAPA_FASE1, MAPA_FASE2 };
static const int TAMANHOS[TOTAL_FASES] = {
    sizeof(MAPA_FASE1) / sizeof(MAPA_FASE1[0]),
    sizeof(MAPA_FASE2) / sizeof(MAPA_FASE2[0]),
};

void iniciarJogo(EstadoJogo *estado, int fase) {
    for (int i = 0; i < NUM_COLUNAS; i++) {
        estado->colunas[i]       = criarLista();
        estado->ultimoAcerto[i]  = ACERTO_NENHUM;
        estado->tempoFeedback[i] = 0.0f;
    }
    estado->pontuacao    = 0;
    estado->combo        = 0;
    estado->tempoJogo    = 0.0f;
    estado->idxSequencia = 0;
    if (fase >= 0 && fase < TOTAL_FASES)
        estado->faseAtual = fase;
    else
        estado->faseAtual = 0;
}

void atualizarJogo(EstadoJogo *estado, float dt) {
    estado->tempoJogo += dt;

    const EventoNota *mapa  = MAPAS[estado->faseAtual];
    int               total = TAMANHOS[estado->faseAtual];

    while (estado->idxSequencia < total) {
        float tempoSpawn = mapa[estado->idxSequencia].tempo - TEMPO_QUEDA;
        if (estado->tempoJogo >= tempoSpawn) {
            int col = mapa[estado->idxSequencia].coluna;
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
        int mult;
        if (estado->combo > 8)
            mult = 8;
        else
            mult = estado->combo;
        estado->pontuacao += 100 * mult;
    } else if (dist <= (float)JANELA_ACERTO) {
        resultado = ACERTO_BOM;
        estado->combo++;
        int mult;
        if (estado->combo > 8)
            mult = 8;
        else
            mult = estado->combo;
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

int faseConcluida(const EstadoJogo *estado) {
    if (estado->idxSequencia < TAMANHOS[estado->faseAtual])
        return 0;
    for (int i = 0; i < NUM_COLUNAS; i++)
        if (!listaVazia((Lista *)&estado->colunas[i]))
            return 0;
    return 1;
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
