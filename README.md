# Baque Virado

Jogo desenvolvido para a disciplina de **Algoritmos e Estrutura de Dados**.

**Estudantes:**
- Fernando Soares da Silva
- Gabriel França de Albuquerque Pernambuco
- Luis Guilherme Alves Lauria Soares

---

## Demonstração

<!-- Adicione aqui o vídeo do jogo rodando -->

**Vídeo no YouTube:** [Assistir](https://youtu.be/Qtre8Q7bIlw)

**Slides da apresentação:** [Ver slides](https://canva.link/qpesdstooek012v)
---

## Dependências

- [Raylib](https://www.raylib.com/)
- [libcurl](https://curl.se/libcurl/)
- gcc, make

---

## Como executar

**1. Clone o repositório:**
```bash
git clone https://github.com/gabrielfranca10/Baque-Virado-AED.git
cd Baque-Virado-AED
```

---

### Linux (Ubuntu/Debian)

**Instalar dependências:**
```bash
sudo apt install gcc make libcurl4-openssl-dev
```

Instale o Raylib seguindo: https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux

**Compilar e executar:**
```bash
make run
```

---

### Windows

**Pré-requisitos:**
- Instale o [MSYS2](https://www.msys2.org/)
- Abra o terminal **MSYS2 MinGW 64-bit** e rode:

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-raylib mingw-w64-x86_64-curl
```

**Compilar e executar:**
```bash
make run
```

---

### macOS

**Instalar dependências via Homebrew:**
```bash
brew install raylib curl
```

Se não tiver o Homebrew: https://brew.sh

**Compilar e executar:**
```bash
make run
```

---

**Para limpar os arquivos compilados:**
```bash
make clean
```
