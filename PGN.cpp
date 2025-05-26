#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <regex>

using namespace std;

char board[8][8];

// Zmiana z np. "e4" na indeksy tablicy
pair<int, int> squareToIndex(const string& square) {
    return { 8 - (square[1] - '0'), square[0] - 'a' };
}

string indexToSquare(int row, int col) {
    return string(1, 'a' + col) + to_string(8 - row);
}

void initBoard() {
    string setup[8] = {
        "rnbqkbnr",
        "pppppppp",
        "........",
        "........",
        "........",
        "........",
        "PPPPPPPP",
        "RNBQKBNR"
    };
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            board[i][j] = setup[i][j];
}

void printBoard() {
    cout << "  a b c d e f g h" << endl;
    for (int i = 0; i < 8; ++i) {
        cout << 8 - i << ' ';
        for (int j = 0; j < 8; ++j) {
            cout << board[i][j] << ' ';
        }
        cout << 8 - i << endl;
    }
    cout << "  a b c d e f g h\n" << endl;
}

// Czy figura pasuje do danego typu i koloru
bool isPiece(char c, char type, bool white) {
    return white ? c == type : c == tolower(type);
}

// Czy figura typu `type` mo¿e dojœæ na pole (toRow, toCol) z (r, c)
bool isValidDirection(char type, int r, int c, int toRow, int toCol,bool white) {
    int dr = toRow - r;
    int dc = toCol - c;
    switch (type) {
    case 'P': {
        if (white) {
            if (dc == 0 && dr == -1 && board[toRow][toCol] == '.') return true; // e2-e3
            if (dc == 0 && dr == -2 && r == 6 && board[r - 1][c] == '.' && board[toRow][toCol] == '.') return true; // e2-e4
            if (abs(dc) == 1 && dr == -1 && islower(board[toRow][toCol])) return true; // bicie
        }
        else {
            if (dc == 0 && dr == 1 && board[toRow][toCol] == '.') return true; // e7-e6
            if (dc == 0 && dr == 2 && r == 1 && board[r + 1][c] == '.' && board[toRow][toCol] == '.') return true; // e7-e5
            if (abs(dc) == 1 && dr == 1 && isupper(board[toRow][toCol])) return true; // bicie
        }
        return false;
    }
    case 'N': return (abs(dr) == 2 && abs(dc) == 1) || (abs(dr) == 1 && abs(dc) == 2);
    case 'B': return abs(dr) == abs(dc);
    case 'R': return (dr == 0 || dc == 0);
    case 'Q': return (abs(dr) == abs(dc) || dr == 0 || dc == 0);
    case 'K': return abs(dr) <= 1 && abs(dc) <= 1;
    }
    return false;
}

// Znajduje pasuj¹c¹ figurê danego typu, z opcjonalnym okreœleniem kolumny/wiersza Ÿród³a
pair<int, int> findMatchingPiece(char type, bool white, int toRow, int toCol, char disambigFile = 0, char disambigRank = 0) {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            char piece = board[r][c];
            if (!isPiece(piece, type, white))
                continue;

            if (disambigFile && c != disambigFile - 'a') continue;
            if (disambigRank && r != 8 - (disambigRank - '0')) continue;

            if (isValidDirection(type, r, c, toRow, toCol,white)) {
                // sprawdŸ, czy droga nie jest zablokowana (dla wie¿y, goñca, królowej)
                if (type == 'R' || type == 'B' || type == 'Q') {
                    int dr = (toRow - r) == 0 ? 0 : (toRow - r) / abs(toRow - r);
                    int dc = (toCol - c) == 0 ? 0 : (toCol - c) / abs(toCol - c);
                    int nr = r + dr, nc = c + dc;
                    while (nr != toRow || nc != toCol) {
                        if (board[nr][nc] != '.') goto blocked;
                        nr += dr;
                        nc += dc;
                    }
                }
                return { r, c };
            blocked:
                continue;
            }
        }
    }
    return { -1, -1 };
}

void movePiece(const string& move, bool whiteToMove) {
    if (move == "O-O") {
        if (whiteToMove) {
            board[7][4] = '.'; board[7][6] = 'K';
            board[7][7] = '.'; board[7][5] = 'R';
        } else {
            board[0][4] = '.'; board[0][6] = 'k';
            board[0][7] = '.'; board[0][5] = 'r';
        }
        return;
    } else if (move == "O-O-O") {
        if (whiteToMove) {
            board[7][4] = '.'; board[7][2] = 'K';
            board[7][0] = '.'; board[7][3] = 'R';
        } else {
            board[0][4] = '.'; board[0][2] = 'k';
            board[0][0] = '.'; board[0][3] = 'r';
        }
        return;
    }

    string m = move;
    m.erase(remove(m.begin(), m.end(), '+'), m.end());
    m.erase(remove(m.begin(), m.end(), '#'), m.end());

    char type = 'P';
    int idx = 0;

    if (isupper(m[0]) && m[0] != 'O') {
        type = m[0];
        idx = 1;
    }

    smatch match;
    regex re("([NBRQK]?)([a-h1-8]?)(x?)([a-h][1-8])");

    if (!regex_match(m, match, re)) {
        cerr << "Nieobs³ugiwany format ruchu: " << move << endl;
        return;
    }

    string pieceStr = match[1];
    string disamb = match[2];
    string dest = match[4];

    char disFile = 0, disRank = 0;
    if (!disamb.empty()) {
        for (char ch : disamb) {
            if (isalpha(ch)) disFile = ch;
            else if (isdigit(ch)) disRank = ch;
        }
    }

    pair<int, int> to = squareToIndex(dest);

    // Specjalna obs³uga dla piona bij¹cego w bok, np. hxg5
    if (type == 'P' && !pieceStr.empty() && move.find('x') != string::npos) {
        int fromCol = disamb[0] - 'a';
        int dir = whiteToMove ? -1 : 1;
        int fromRow = to.first + dir;

        if (fromRow >= 0 && fromRow < 8 && board[fromRow][fromCol] == (whiteToMove ? 'P' : 'p')) {
            board[to.first][to.second] = whiteToMove ? 'P' : 'p';
            board[fromRow][fromCol] = '.';
        } else {
            cerr << "Nie znaleziono pionka do ruchu " << move << endl;
        }
        return;
    }

    pair<int, int> from = findMatchingPiece(type, whiteToMove, to.first, to.second, disFile, disRank);

    if (from.first == -1) {
        cerr << "Nie znaleziono figury " << type << " do ruchu " << move << endl;
        return;
    }

    board[to.first][to.second] = whiteToMove ? type : tolower(type);
    board[from.first][from.second] = '.';
}


void readPGN(const string& pgn) {
    initBoard();
    printBoard();

    istringstream iss(pgn);
    string token;
    bool whiteToMove = true;

    while (iss >> token) {
        if (token[0] == '{') {
            while (token.back() != '}') iss >> token; // pomiñ komentarz
            continue;
        }

        token = regex_replace(token, regex("^[0-9]+\\."), "");

        if (token == "1-0" || token == "0-1" || token == "1/2-1/2") break;
        if (token.empty()) continue;

        cout << (whiteToMove ? "[Bia³y] " : "[Czarny] ") << token << endl;
        movePiece(token, whiteToMove);
        printBoard();
        whiteToMove = !whiteToMove;
    }
}
