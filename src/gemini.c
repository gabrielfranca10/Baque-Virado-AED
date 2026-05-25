#include "gemini.h"
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RESP_MAX 8192

typedef struct { char buf[RESP_MAX]; int len; } Buffer;

static size_t escreverResposta(void *ptr, size_t size, size_t nmemb, void *userdata) {
    Buffer *b   = (Buffer *)userdata;
    size_t  n   = size * nmemb;
    size_t  esp = RESP_MAX - b->len - 1;
    if (n > esp) n = esp;
    memcpy(b->buf + b->len, ptr, n);
    b->len += n;
    b->buf[b->len] = '\0';
    return size * nmemb;
}

static void extrairTexto(const char *json, const char *fallback, char *out, int tamanho) {
    const char *erro = fallback ? fallback : "Nao foi possivel carregar a historia da musica.";
    if (strstr(json, "\"error\"")) {
        strncpy(out, erro, tamanho - 1);
        out[tamanho - 1] = '\0';
        return;
    }

    const char *inicio = NULL;
    const char *tentativas[] = { "\"text\": \"", "\"text\":\"", NULL };
    for (int i = 0; tentativas[i] != NULL; i++) {
        inicio = strstr(json, tentativas[i]);
        if (inicio) {
            inicio += strlen(tentativas[i]);
            break;
        }
    }
    if (!inicio) {
        strncpy(out, erro, tamanho - 1);
        out[tamanho - 1] = '\0';
        return;
    }

    int i = 0;
    while (*inicio && i < tamanho - 1) {
        if (*inicio == '\\' && *(inicio + 1) == '"') {
            out[i++] = '"';
            inicio += 2;
        } else if (*inicio == '\\' && *(inicio + 1) == 'n') {
            out[i++] = '\n';
            inicio += 2;
        } else if (*inicio == '"') {
            break;
        } else {
            out[i++] = *inicio++;
        }
    }
    out[i] = '\0';
}

void buscarLoreGemini(const char *titulo, const char *artista, const char *fallback, char *out, int tamanho) {
    const char *key = getenv("GEMINI_API_KEY");
    if (!key) {
        strncpy(out, fallback ? fallback : "Defina GEMINI_API_KEY para ver o lore.", tamanho - 1);
        out[tamanho - 1] = '\0';
        return;
    }

    char url[512];
    snprintf(url, sizeof(url),
        "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash-lite:generateContent?key=%s",
        key);

    char prompt[512];
    snprintf(prompt, sizeof(prompt),
        "Em 2 frases curtas e empolgantes, conte algo interessante sobre a musica '%s' de '%s'. Responda em portugues.",
        titulo, artista);

    char body[1024];
    snprintf(body, sizeof(body),
        "{\"contents\":[{\"parts\":[{\"text\":\"%s\"}]}]}", prompt);

    Buffer resp = {0};

    CURL *curl = curl_easy_init();
    if (!curl) {
        strncpy(out, "Erro ao inicializar curl.", tamanho - 1);
        out[tamanho - 1] = '\0';
        return;
    }

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL,           url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,    body);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER,    headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escreverResposta);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,     &resp);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT,       10L);

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        strncpy(out, fallback ? fallback : "Erro de conexao com o Gemini.", tamanho - 1);
        out[tamanho - 1] = '\0';
        return;
    }

    FILE *f = fopen("/tmp/gemini_debug.txt", "w");
    if (f) { fprintf(f, "%s\n", resp.buf); fclose(f); }

    extrairTexto(resp.buf, fallback, out, tamanho);
}
