#include "Header.h"

Node::Node(Board board, Player user, Player ai)
    : board{ board }, user{ user }, ai{ ai }, new_col(-1), new_row(-1), new_val(-1), score(0) {}

bool Node::is_terminal() {
    if (user.get_num_card() == 0 && ai.get_num_card() == 0) {
        return 1;
    }
    return 0;
}

int Node::estimate_cost() {
    if (is_terminal()) {
        if(board.get_score(AI_POS) > board.get_score(USER_POS)){
            return MAX;
        }
        if (board.get_score(AI_POS) < board.get_score(USER_POS)) {
            return MIN;
        }
        else {
            return 0;
        }
    }
    int ai_score_on_board, user_score_on_board;
    int ai_score_on_card, user_score_on_card;
    int max_ai_card_on_board, max_user_card_on_board;
    int max_ai_card_on_card, max_user_card_on_card;
    int num_ai_card, num_user_card; //card not turn
    int num_deleted_ai, num_deleted_user;

    ai_score_on_board = board.get_score(AI_POS);
    user_score_on_board = board.get_score(USER_POS);
    
    ai_score_on_card = ai.sum_of_cards();
    user_score_on_card = user.sum_of_cards();

    max_ai_card_on_board = board.get_max_card(AI_POS);
    max_user_card_on_board = board.get_score(USER_POS);

    max_ai_card_on_card = ai.max_card();
    max_user_card_on_card = user.max_card();

    num_ai_card = ai.get_num_card();
    num_user_card = user.get_num_card();

    num_deleted_ai = board.deleted_ai();
    num_deleted_user = board.deleted_user();

    int cost_ai = ai_score_on_board + ai_score_on_card + max_ai_card_on_board
        + max_ai_card_on_card + num_ai_card - num_deleted_ai * 6;
    int cost_user = user_score_on_board + user_score_on_card + max_user_card_on_board
        + max_user_card_on_card + num_user_card - num_deleted_user * 6;
    return cost_ai - cost_user;
}

int Node::minmax_prun(int now_depth,int max_depth, int is_user_turn, int alpha, int beta) {
    if (now_depth == max_depth || is_terminal()) {
        score = estimate_cost();
        return score;
    }

    add_child(is_user_turn);

    if (!is_user_turn) {  // ai moves, maximizing
        score = MIN;
        for (size_t i = 0; i < children.size(); i++) {
            int next_minmax = children.at(i).minmax_prun(now_depth + 1,max_depth, !is_user_turn, alpha, beta);
            score = max(score, next_minmax);
            alpha = max(alpha, score);
            if (beta <= alpha) {
                break;
            }
        }
        return score;
    }
    else {  // human moves, minimizing
        score = MAX;
        for (size_t i = 0; i < children.size(); i++) {
            int next_minmax = children.at(i).minmax_prun(now_depth + 1,max_depth, !is_user_turn, alpha, beta);
            score = min(score, next_minmax);
            beta = min(beta, score);
            if (beta <= alpha) {
                break;
            }
        }
        return score;
    }
}

void Node::add_child(bool is_user_turn) {

    for (int row = 1; row <= board.get_size(); row++) {
        for (int col = 1; col <= board.get_size(); col++) {
            if (!board.can_play(row, col)) continue;

            if (is_user_turn) {
                for (int id = 0; id < user.get_unique_card().size(); id++) {
                    unsigned char card = user.get_unique_card().at(id);
                    Node temp(board, user, ai);
                    temp.get_board().add_card(row, col, card, USER_POS);
                    temp.get_board().check_board();
                    temp.info_newest_child(row, col, card);
                    temp.get_user().remove_card(card);
                    children.push_back(temp);
                }
            }
            else {
                for (int id = 0; id < ai.get_unique_card().size(); id++) {
                    unsigned char card = ai.get_unique_card().at(id);
                    Node temp(board, user, ai);
                    temp.get_board().add_card(row, col, card, AI_POS);
                    temp.get_board().check_board();
                    temp.info_newest_child(row, col, card);
                    temp.get_ai().remove_card(card);
                    children.push_back(temp);
                }
            }
        }
    }
}

Player& Node::get_user() {
    return user;
}

Player& Node::get_ai() {
    return ai;
}

Board& Node::get_board() {
    return board;
}

void Node::best_child(int& row, int& col, int& val) {
    int best_val = children.front().score;
    row = children.front().new_row;
    col = children.front().new_col;
    val = children.front().new_val;

    for (const Node& child : children) {
        if (child.score > best_val) {
            best_val = child.score;
            row = child.new_row;
            col = child.new_col;
            val = child.new_val;
        }
    }
}

void Node::info_newest_child(int row, int col, int val) {
    new_row = row;  new_col = col;  new_val = val;
}

