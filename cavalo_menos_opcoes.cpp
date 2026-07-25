#include <iostream>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <vector>
using namespace std;

struct Movimento {
    int x;
    int y;
    int grau;
};

const int N = 8;
const int TEMPO_LIMITE = 600; // segundos
unsigned long long passos = 0;

chrono::steady_clock::time_point inicio;
bool timeout = false;

// Movimentos possíveis do cavalo
int movX[8] = {2, 1, -1, -2, -2, -1, 1, 2};
int movY[8] = {1, 2, 2, 1, -1, -2, -2, -1};
// int movX[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
// int movY[8] = {-1, -2, -2, -1, 1, 2, 2, 1};

bool posicaoValida( int x, int y, int tabuleiro[N][N] ) {
    return ( x >= 0 && x < N &&
             y >= 0 && y < N && 
             tabuleiro[x][y] == -1 );
}

int grau(int x, int y, int tabuleiro[N][N]) {
    int cont = 0;

    for (int i = 0; i < 8; i++) {
        int nx = x + movX[i];
        int ny = y + movY[i];

        if (posicaoValida(nx, ny, tabuleiro))
            cont++;
    }

    return cont;
}

bool passeioCavalo( int x, int y, int movimento, int tabuleiro[N][N] ) {

    auto agora = chrono::steady_clock::now();
    auto tempo = chrono::duration_cast<chrono::seconds>(agora - inicio).count();

    if ( tempo >= TEMPO_LIMITE ) {
        timeout = true;
        return false;
    }

    if ( movimento == N * N ) // Se todas as casas foram visitadas
        return true;


    vector<Movimento> candidatos;
    for (int i = 0; i < 8; i++) {
        int proxX = x + movX[i];
        int proxY = y + movY[i];

        if (posicaoValida(proxX, proxY, tabuleiro)) {
            candidatos.push_back({
                proxX,
                proxY,
                grau(proxX, proxY, tabuleiro)
            });
        }
    }
    sort(candidatos.begin(), candidatos.end(),
        [](const Movimento &a, const Movimento &b) {
            return a.grau < b.grau;
        });

    for (const auto &m : candidatos) {
        tabuleiro[m.x][m.y] = movimento;
        passos++;

        if (passeioCavalo(m.x, m.y, movimento + 1, tabuleiro))
            return true;

        tabuleiro[m.x][m.y] = -1;
    }

    return false;
}

void imprimirTabuleiro ( int tabuleiro[N][N] ) {
    for ( int i = 0; i < N; i++ ) {
        for ( int j = 0; j < N; j++ ) {
            cout << setw(3) << tabuleiro[i][j] << " ";
        }
        cout << endl;
    }
}

bool detectaSolucao ( int iniX, int iniY, int tabuleiro[N][N] ) {
    for ( int i = 0; i < 8; i++ ) {
        int x = iniX + movX[i];
        int y = iniY + movY[i];

        if ( x >= 0 && x < N && y >= 0 && y < N && tabuleiro[x][y] == 63 ) {
            return true;
        }
    }
    return false;
}

int main() {

    int tabuleiro[N][N];

    // Inicializa o tabuleiro
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            tabuleiro[i][j] = -1;

    int linha, coluna;

    cout << "Digite a linha inicial (0-7): ";
    cin >> linha;

    cout << "Digite a coluna inicial (0-7): ";
    cin >> coluna;

    if (linha < 0 || linha >= N || coluna < 0 || coluna >= N) {
        cout << "Posicao invalida!" << endl;
        return 1;
    }

    tabuleiro[linha][coluna] = 0;

    timeout = false;
    inicio = chrono::steady_clock::now();

    bool encontrou = passeioCavalo(linha, coluna, 1, tabuleiro);

    auto fim = chrono::steady_clock::now();
    auto tempoTotal = chrono::duration_cast<chrono::microseconds>(fim - inicio).count();

    cout << "\nTempo de execucao: " << tempoTotal << " us";
    cout << "\nNumero de passos: " << passos << " passos\n";
    
    string tipoSolucao;

    if (encontrou) {
        ( detectaSolucao( linha, coluna, tabuleiro ) ) ? tipoSolucao = "Fechada\n" : tipoSolucao = "Aberta\n";
        cout << "Solucao encontrada!\n";
        cout << "Solucao: " << tipoSolucao << endl;
        imprimirTabuleiro(tabuleiro);
    } else if (timeout) {
        cout << "Tempo limite de " << TEMPO_LIMITE
             << " segundos excedido.\n";
        cout << "A busca foi interrompida.\n";
    } else {
        cout << "Nenhuma solucao encontrada.\n";
    }

    return 0;
}