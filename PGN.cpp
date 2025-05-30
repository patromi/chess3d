#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <regex>

using namespace std;

char board[8][8];
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

// Czy figura typu `type` � na pole (toRow, toCol) z (r, c)
bool isValidDirection(char type, int r, int c, int toRow, int toCol, bool white) {
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

// Znajduje pasuj�c� figur� danego typu, z opcjonalnym okre�leniem kolumny/wiersza �r�d�a
pair<int, int> findMatchingPiece(char type, bool white, int toRow, int toCol, char disambigFile = 0, char disambigRank = 0) {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            char piece = board[r][c];
            if (!isPiece(piece, type, white))
                continue;

            if (disambigFile && c != disambigFile - 'a') continue;
            if (disambigRank && r != 8 - (disambigRank - '0')) continue;

            if (isValidDirection(type, r, c, toRow, toCol, white)) {
                // sprawd�, czy droga nie jest zablokowana (dla wie�y, go�ca, kr�lowej)
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
        }
        else {
            board[0][4] = '.'; board[0][6] = 'k';
            board[0][7] = '.'; board[0][5] = 'r';
        }
        return;
    }
    else if (move == "O-O-O") {
        if (whiteToMove) {
            board[7][4] = '.'; board[7][2] = 'K';
            board[7][0] = '.'; board[7][3] = 'R';
        }
        else {
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
        cerr << "Nieobs�ugiwany format ruchu: " << move << endl;
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

    // Specjalna obs�uga dla piona bij�cego w bok, np.hxg5
    if (type == 'P' && !pieceStr.empty() && move.find('x') != string::npos) {
        int fromCol = disamb[0] - 'a';
        int dir = whiteToMove ? -1 : 1;
        int fromRow = to.first + dir;

        if (fromRow >= 0 && fromRow < 8 && board[fromRow][fromCol] == (whiteToMove ? 'P' : 'p')) {
            board[to.first][to.second] = whiteToMove ? 'P' : 'p';
            board[fromRow][fromCol] = '.';
        }
        else {
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

string token;
bool whiteToMove = true;
std::string pgn = "1.d4 Nf6 2.c4 e6 3.Nf3 d5 4.Nc3 Bb4 5.e3 O-O 6.Bd3 c5 "
"7.O-O Nc6 8.a3 Ba5 9.Ne2 dxc4 10.Bxc4 Bb6 11.dxc5 Qxd1 "
"12.Rxd1 Bxc5 13.b4 Be7 14.Bb2 Bd7 15.Rac1 Rfd8 16.Ned4 "
"Nxd4 17.Nxd4 Ba4 18.Bb3 Bxb3 19.Nxb3 Rxd1+ 20.Rxd1 Rc8 "
"21.Kf1 Kf8 22.Ke2 Ne4 23.Rc1 Rxc1 24.Bxc1 f6 25.Na5 Nd6 "
"26.Kd3 Bd8 27.Nc4 Bc7 28.Nxd6 Bxd6 29.b5 Bxh2 30.g3 h5 "
"31.Ke2 h4 32.Kf3 Ke7 33.Kg2 hxg3 34.fxg3 Bxg3 35.Kxg3 Kd6 "
"36.a4 Kd5 37.Ba3 Ke4 38.Bc5 a6 39.b6 f5 40.Kh4 f4 "
"41.exf4 Kxf4 42.Kh5 Kf5 43.Be3 Ke4 44.Bf2 Kf5 45.Bh4 e5 "
"46.Bg5 e4 47.Be3 Kf6 48.Kg4 Ke5 49.Kg5 Kd5 50.Kf5 a5 "
"51.Bf2 g5 52.Kxg5 Kc4 53.Kf5 Kb4 54.Kxe4 Kxa4 55.Kd5 Kb5 "
"56.Kd6 1-0";
bool end_game = false; // Flaga do sprawdzenia, czy gra si�� zako��czy��a
bool flag = true;
istringstream iss(pgn);

void readPGN(bool permission) {
    static bool initialized = false;
    static std::string nextToken;

    if (!initialized) {
        initBoard();
        initialized = true;
    }

    if (!permission) {
        return; // Nie pobieramy nowego tokena, nie ruszamy iss
    }

    // Jeśli mamy już zapamiętany token z poprzedniego wywołania, użyj go;
    // w przeciwnym razie pobierz nowy
    if (nextToken.empty()) {
        if (!(iss >> nextToken)) {
            return; // Nie ma już więcej tokenów
        }
    }

    if (nextToken[0] == '{') {
        while (nextToken.back() != '}') {
            if (!(iss >> nextToken)) return; // Niepełny komentarz
        }
        nextToken.clear();
        return;
    }

    nextToken = std::regex_replace(nextToken, std::regex("^[0-9]+\\."), "");

    if (nextToken == "1-0" || nextToken == "0-1" || nextToken == "1/2-1/2") {
        end_game = true;
        nextToken.clear();
        return;
    }

    if (nextToken.empty()) return;

    std::cout << (whiteToMove ? "[White] " : "[Black] ") << nextToken << std::endl;
    movePiece(nextToken, whiteToMove);
    printBoard();
    whiteToMove = !whiteToMove;

    // Wyczyszczenie tokenu, aby przy kolejnym `permission == true` pobrać nowy
    nextToken.clear();
}