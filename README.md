# 🥁 Baque Virado

Jogo de ritmo em C inspirado no Maracatu pernambucano, desenvolvido como projeto acadêmico da disciplina de **Algoritmos e Estruturas de Dados** — CESAR School.

**Estudantes:**
- Fernando Soares da Silva
- Gabriel França de Albuquerque Pernambuco
- Luis Guilherme Alves Lauria Soares

---

## Demonstração

**Vídeo no YouTube:** [Assistir](https://youtu.be/Qtre8Q7bIlw)

**Slides da apresentação:** [Ver slides](https://canva.link/qpesdstooek012v)

---

## Como jogar

O jogo é dividido em **3 fases**, cada uma com uma música do universo do Maracatu e Manguebeat. Notas descem por 4 colunas e você deve pressionar a tecla certa no momento em que a nota chega à linha de acerto.

| Coluna | Tecla | Instrumento |
|--------|-------|-------------|
| 1ª | `D` | Alfaia |
| 2ª | `F` | Gonguê |
| 3ª | `J` | Agbê |
| 4ª | `K` | Tarol |

**Acertos:**
- **PERFEITO** — tecla pressionada exatamente na linha → mais pontos + combo
- **BOM** — pressionada perto da linha → pontos + combo
- **ERROU** — fora do tempo ou coluna vazia → combo zerado

**Combo:** acertos consecutivos multiplicam sua pontuação (máximo x8).

**Outras teclas:**
- `ESC` — pausar o jogo
- `R` — repetir a fase atual
- `ENTER` — confirmar / avançar fase

Ao final das 3 fases, você pode digitar seu nome para salvar sua pontuação no ranking.

---

## Estrutura de dados

O jogo utiliza uma **Lista Simplesmente Encadeada** para gerenciar as notas de cada coluna em tempo real.

Cada coluna possui sua própria lista, onde cada nó representa uma nota caindo na tela. As principais operações são:

| Função | O que faz |
|--------|-----------|
| `criarLista()` | Inicializa uma lista vazia para cada coluna |
| `inserirNota()` | Adiciona uma nova nota no fim da lista |
| `removerNota()` | Remove a nota da cabeça quando acertada ou perdida |
| `listaVazia()` | Verifica se não há mais notas (fase concluída) |
| `atualizarNotas()` | Percorre a lista a cada frame e move as notas para baixo |
| `limparLista()` | Libera toda a memória da lista ao fim de uma fase |

---

## Algoritmo de ordenação

O ranking de pontuações é ordenado com **Insertion Sort**, implementado na função `ordenarRanking()` em `src/logica.c`.

A escolha foi motivada pelo tamanho pequeno e fixo do ranking (máximo 10 entradas), contexto em que o Insertion Sort é eficiente e simples de implementar.

---

## Estrutura do projeto

```
Baque-Virado-AED/
├── assets/
│   ├── fundo.png           # Tela de menu
│   ├── fundo_jogo.png      # Tela de jogo
│   ├── maracatu_atomico.mp3
│   ├── fase2.mp3
│   └── fase3.mp3
├── include/
│   ├── lista.h             # Interface da lista encadeada
│   ├── logica.h            # Interface da lógica do jogo
│   ├── render.h            # Interface das funções de desenho
│   └── gemini.h            # Interface da integração com a API Gemini
├── src/
│   ├── main.c              # Loop principal e máquina de estados de telas
│   ├── lista.c             # Lista simplesmente encadeada
│   ├── logica.c            # Lógica do jogo, pontuação e ranking
│   ├── render.c            # Todas as funções de renderização
│   └── gemini.c            # Requisição HTTP para a API do Gemini
├── ranking.txt             # Pontuações salvas localmente
└── Makefile
```

---

## Dependências

- [Raylib](https://www.raylib.com/) — biblioteca gráfica e de áudio
- [libcurl](https://curl.se/libcurl/) — requisições HTTP para a API Gemini
- `gcc`, `make`

---

## Como executar

**1. Clone o repositório:**
```bash
git clone https://github.com/gabrielfranca10/Baque-Virado-AED.git
cd Baque-Virado-AED
```

### Linux (Ubuntu/Debian)

```bash
sudo apt install gcc make libcurl4-openssl-dev
```

Instale o Raylib: https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux

```bash
make run
```

### Windows

- Instale o [MSYS2](https://www.msys2.org/)
- Abra o terminal **MSYS2 MinGW 64-bit** e rode:

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-raylib mingw-w64-x86_64-curl
make run
```

### macOS

```bash
brew install raylib curl
make run
```

**Para limpar os arquivos compilados:**
```bash
make clean
```
