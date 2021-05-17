#include "Header.h"

Game::Game(int is_user, int board_size) :
    board{ board_size }, user{ true, board_size }, ai{ false, board_size } {

    this->is_user = is_user;
    this->board_size = board_size;
}

void Game::start(bool autoplay) {
    int row, col, val;

    print();
    do {
        if (is_user) {  //is user =1?
            cout << "Input (row, col, weight): ";
            cin >> row >> col >> val;
            cout << "[User]: (" << row << ", " << col << ", " << val << ")" << endl;
            if (!is_exist_card(val) || !board.add_card(row, col, val, USER_POS)) {
                cout << "wrong play.Try again!" << endl;
                continue;
            }
            user.remove_card(val);
        }
        else {
            ai.ai_turn(board, user, ai, row, col, val);
            board.add_card(row, col, val, AI_POS);
            ai.remove_card(val);
            cout << "[AI]: (" << row << ", " << col << ", " << val << ")" << endl;
        }

        board.check_board();
        print();
        is_user = !is_user;

    } while (!end());

    int human_score, ai_score, human_card, ai_card;
    int who_win = get_result(human_score, ai_score, human_card, ai_card);
    //std::string s = (res == 1) ? "\033[1;32mAI\033[0m win!!!" : "\033[1;31mUser\033[0m win!!!";
    HANDLE hConsolerColor;
    hConsolerColor = GetStdHandle(STD_OUTPUT_HANDLE);
    //cout << s << std::endl;
    if (who_win == 1) {
        SetConsoleTextAttribute(hConsolerColor, 10);
        cout << "AI win!!!" << endl;
    }
    if (who_win == -1) {
        SetConsoleTextAttribute(hConsolerColor, 13);
        cout << "[USER win!!!" << endl;
    }
    if (who_win == 0) {
        SetConsoleTextAttribute(hConsolerColor, 15);
        cout << "[AI DRAW USER!!!" << endl;
    }
    SetConsoleTextAttribute(hConsolerColor, 15);
    cout << "========================================" << std::endl;
    cout << "User score   " << "score = " << human_score << ". max card = " << human_card << endl;
    cout << "AI score     " << "score = " << ai_score << ". max card = "<< ai_card << endl;
}

void Game::print() {
    board.print_board();
    user.print_card();
    ai.print_card();
}

bool Game::end() {
    return user.get_num_card() == 0 && ai.get_num_card() == 0;
}

bool Game::is_exist_card(int val) {
    return user.has_card(val);
}

int Game::get_result(int& human_score, int& ai_score, int& human_card, int& ai_card) {
    human_score = board.get_score(USER_POS);
    ai_score = board.get_score(AI_POS);
    human_card = board.get_max_card(USER_POS);
    ai_card = board.get_max_card(AI_POS);

    if (human_score > ai_score) return -1;
    if (human_score == ai_score && human_card > ai_card) return -1;
    if (human_score == ai_score && human_card == ai_card) return 0;
    return 1;
}