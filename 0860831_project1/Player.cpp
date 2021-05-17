#include "Header.h"

Player::Player(bool user, int board_size) : is_user(user) {
    reset_card(board_size);
}

void Player::reset_card(int board_size) {
    if (board_size == 4) {
        cards[0] = 2;  cards[1] = 3; cards[2] = 5;
        cards[3] = 8;  cards[4] = 13;
        num_card = 5;
    }
    else {
        cards[0] = cards[1] = 2;
        cards[2] = cards[3] = 3;
        cards[4] = cards[5] = 5;
        cards[6] = cards[7] = cards[8] = 8;
        cards[9] = cards[10] = 13;
        num_card = 11;
    }
}

int Player::get_num_card() {
    return num_card;
}

void Player::print_card() {
    HANDLE hConsolerColor;
    hConsolerColor = GetStdHandle(STD_OUTPUT_HANDLE);
    if (is_user) {
        SetConsoleTextAttribute(hConsolerColor, 13);
        cout << "[User chess pieces]: ";
    }
    else
    {
        SetConsoleTextAttribute(hConsolerColor, 10);
        cout << "[AI chess pieces]: ";
    }
    SetConsoleTextAttribute(hConsolerColor, 15);
    cout << "[";
    for (int i = 0; i < num_card - 1; i++) {
        cout << int(cards[i]) << ", ";
    }
    if (num_card != 0) cout << int(cards[num_card - 1]);
    cout << "]" << endl;
}

void Player::remove_card(int val) {
    if (num_card == 1) {
        cards[0] = 0;
        num_card = 0;
        return;
    }
    for (int i = 0; i < num_card; i++) {
        if (cards[i] == val) {
            cards[i] = cards[num_card - 1];
            cards[num_card - 1] = 0;
            break;
        }
    }
    num_card--;
    sort(cards, cards + num_card);  // sort cards[]
}

void Player::ai_turn(const Board& board, const Player& user, const Player& ai, int& row, int& col, int& val) {
    Node initial_state(board, user, ai);
    int max_depth = calculate_depth(board, user, ai);
    initial_state.minmax_prun(0,max_depth, false, MIN, MAX);
    initial_state.best_child(row, col, val);
}

unsigned char Player::get_card(int card_id) {
    return cards[card_id];
}

bool Player::has_card(int val) {
    for (int i = 0; i < num_card; i++) {
        if (cards[i] == val) return true;
    }
    return false;
}

int Player::sum_of_cards() {
    int sum = 0;
    for (int i = 0; i < num_card; i++) {
        sum += (int)cards[i];
    }
    return sum;
}

vector <int> Player::get_unique_card() {
    vector <int> temp;
    for (size_t i = 0; i < num_card; i++) {
        if (temp.size() == 0) {
            temp.push_back(cards[i]);
        }
        else
        {
            bool check = false;
            for (size_t j = 0; j < temp.size(); j++) {
                if (temp.at(j) == cards[i]) {
                    check = true;
                }
            }
            if (check == true) {
                temp.push_back(cards[i]);
            }
        }
    }
    return temp;
}
vector<int> Player::get_all_card() {
    vector<int> temp;
    for (size_t i = 0; i < num_card; i++) {
        temp.push_back(cards[i]);
    }
    return temp;
}
int Player::num_unique_card(vector<int> orig_card) {
    vector <int> temp;
    for (size_t i = 0; i < orig_card.size(); i++) {
        if (temp.size() == 0) {
            temp.push_back(orig_card.at(i));
        }
        else
        {
            bool check = false;
            for (size_t j = 0; j < temp.size(); j++) {
                if (temp.at(j) == orig_card.at(i)) {
                    check = true;
                }
            }
            if (check == true) {
                temp.push_back(orig_card.at(i));
            }
        }
    }
    return temp.size();
}
int Player::calculate_depth(Board board, Player user, Player ai) {
    int num_empty, num_ai, num_user;
    num_ai = ai.get_num_card();
    num_user = user.get_num_card();

    if (board.get_size() == 4) {
        num_empty = 4 * 4 - (5 - ai.get_num_card()) - (5 - user.get_num_card());
    }
    else
    {
        num_empty = 6 * 6 - (11 - ai.get_num_card()) - (11 - ai.get_num_card());
    }
    int num_node = 1, depth = 0, turn = 0;
    do {
        depth++;
        if (turn == 0) {
            num_node *= num_empty * min(num_ai,5);
            num_ai--;
        }
        else {
            num_node *= num_empty * min(num_user,5);
            num_user--;
        }
        num_empty--;
        turn = 1 - turn;
    } while (num_node <= MAX_NODE && num_empty > 0 && num_ai + num_user > 0);

    return (depth % 2 == 0) ? depth - 1 : depth;
}

int Player::max_card() {
    int max = 0;
    for (int i = 0; i < num_card; i++) {
        if (max < cards[i]) {
            max = cards[i];
        }
    }
    return max;
}