
#include "Header.h"

Board::Board(int size) : grid{ 0 }, mark{ 0 } {
    this->board_size = size;
    for (size_t row = 1; row <= board_size; row++) {
        for (size_t col = 1; col <= board_size; col++) {
            grid[row][col] = 0;
            mark[row][col] = AVALABLE_POS;
        }
    }
}

bool Board::add_card(int row, int col, int val, int player) {
    if (!can_play(row, col)) {
        return false;
    }
    if (row <= 0 || row > board_size) {
        return false;
    }
    if (col <= 0 || col > board_size) {
        return false;
    }
    grid[row][col] = val;
    mark[row][col] = player;
    return true;
}

bool Board::check_at(int row, int col) {
    char around_row[9] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
    char around_col[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };

    int sum = 0;
    for (unsigned char i = 0; i < 9; i++) {
        sum += (int)grid[row + around_row[i]][col + around_col[i]];
    }
    return sum <= 15;
}


void Board::check_board() {
    std::pair<unsigned char, unsigned char> temp[64];
    int len = 0;
    for (int row = 1; row <= board_size; row++) {
        for (int col = 1; col <= board_size; col++) {
            if (!check_at(row, col)) temp[len++] = std::pair<unsigned char, unsigned char>(row, col);
        }
    }

    for (int i = 0; i < len; i++) {
        auto pos = temp[i];
        grid[pos.first][pos.second] = 0;
    }
}

void Board::reset() {
    for (int row = 1; row <= board_size; row++) {
        for (int col = 1; col <= board_size; col++) {
            grid[row][col] = 0;
            mark[row][col] = AVALABLE_POS;
        }
    }
}

int Board::get_at(int row, int col) {
    return (int)grid[row][col];
}

void Board::print_at(int row, int col, int option) {
    HANDLE hConsolerColor;
    hConsolerColor = GetStdHandle(STD_OUTPUT_HANDLE);
    int card_value = grid[row][col];
    if (option == AVALABLE_POS) {     // not yet add any card in this position
        cout << setw(4) << card_value;
    }
    else if (card_value == 0) {  // removed and mark '*'
        cout << setw(4) << 'X';
    }

    else if (option == AI_POS)  //AI
    {
        SetConsoleTextAttribute(hConsolerColor, 10);
        cout << setw(4) << card_value ;
    }
    else  //User
    {
        SetConsoleTextAttribute(hConsolerColor, 13);
        cout << setw(4) << card_value ;
    }
    SetConsoleTextAttribute(hConsolerColor, 15);
}
void Board::print_board() {
    int option;
    for (size_t row = 1; row <= board_size; row++) {
        for (size_t col = 1; col <= board_size; col++) {
            option = mark[row][col];
            print_at(row, col, option);
        }
        cout << endl;
    }
    cout << endl;
}

int Board::get_score(int player) {
    int sum = 0;
    for (int row = 1; row <= board_size; row++) {
        for (int col = 1; col <= board_size; col++) {
            if (mark[row][col] == player) {
                sum += (int)grid[row][col];
            }
        }
    }
    return sum;
}

int Board::get_size() {
    return board_size;
}

int Board::is_avalable(int row, int col) {
    if (mark[row][col] == AVALABLE_POS) {
        return 1;
    }
    return 0;
}

bool Board::can_play(int row, int col) {
    if (mark[row][col] == AVALABLE_POS) {
        return 1;
    }
    return 0;
}

int Board::get_max_card(int player) {
    int max = 0;
    for (int row = 1; row <= board_size; row++) {
        for (int col = 1; col <= board_size; col++) {
            if (grid[row][col] > max && mark[row][col] == player) {
                max = (int)grid[row][col];
            }
        }
    }
    return max;
}

int Board::deleted_ai() {
    int count = 0;
    for (int row = 1; row <= board_size; row++) {
        for (int col = 1; col <= board_size; col++) {
            if (mark[row][col] == AI_POS && grid[row][col]==0 ) {
                count += 1;
            }
        }
    }
    return count;
}

int Board::deleted_user() {
    int count = 0;
    for (int row = 1; row <= board_size; row++) {
        for (int col = 1; col <= board_size; col++) {
            if (mark[row][col] ==USER_POS && grid[row][col] == 0) {
                count += 1;
            }
        }
    }
    return count;
}