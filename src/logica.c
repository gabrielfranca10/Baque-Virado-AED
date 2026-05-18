#include "logica.h"

/* Sequência de colunas que define o ritmo da fase (repete em loop) */
static const int SEQUENCIA[] = {
    0, 1, 2, 3,  0, 2, 1, 3,
    0, 3, 1, 2,  2, 1, 0, 3,
    0, 1, 2, 3,  1, 3, 0, 2,
    0, 0, 1, 2,  3, 1, 2, 3
};
#define LEN_SEQ ((int)(sizeof(SEQUENCIA) / sizeof(SEQUENCIA[0])))

void iniciarJogo(EstadoJogo *estado) {
    for (int i = 0; i < NUM_COLUNAS; i++) {
        estado->colunas[i]       = criarLista();
        estado->ultimoAcerto[i]  = ACERTO_NENHUM;
        estado->tempoFeedback[i] = 0.0f;
    }
    estado->pontuacao      = 0;
    estado->combo          = 0;
    estado->tempoJogo      = 0.0f;
    estado->tempoProxSpawn = 0.8f; /* atraso inicial para o jogador se preparar */
    estado->idxSequencia   = 0;
}

void atualizarJogo(EstadoJogo *estado, float dt) {
    estado->tempoJogo      += dt;
    estado->tempoProxSpawn -= dt;

    /* Spawn da próxima nota */
    if (estado->tempoProxSpawn <= 0.0f) {
        int col = SEQUENCIA[estado->idxSequencia % LEN_SEQ];
        inserirNota(&estado->colunas[col], NOTE_START_Y);
        estado->idxSequencia++;
        estado->tempoProxSpawn = INTERVALO_SPAWN;
    }

    for (int i = 0; i < NUM_COLUNAS; i++) {
        atualizarNotas(&estado->colunas[i], dt, VELOCIDADE_NOTA);

        /* Remove notas que passaram da janela sem acerto (miss) */
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
    /* Pressionou sem nota na coluna */
    if (listaVazia(&estado->colunas[coluna])) {
        estado->combo = 0;
        estado->ultimoAcerto[coluna]  = ACERTO_ERROU;
        estado->tempoFeedback[coluna] = FEEDBACK_DURACAO;
        return ACERTO_ERROU;
    }

    float y    = estado->colunas[coluna].cabeca->y;
    float dist = y - (float)LINHA_ACERTO;
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
        /* Pressionou cedo demais — não remove a nota */
        resultado     = ACERTO_ERROU;
        estado->combo = 0;
        estado->ultimoAcerto[coluna]  = resultado;
        estado->tempoFeedback[coluna] = FEEDBACK_DURACAO;
        return resultado;
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

/* Insertion Sort decrescente: maior pontuação primeiro */
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
        /* Substitui o último (menor) se o novo score for maior */
        ranking[*n - 1].pontuacao = pontuacao;
    } else {
        return;
    }
    ordenarRanking(ranking, *n);
}
