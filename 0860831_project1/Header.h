#pragma once
// Created by Hungle

#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <Windows.h>
#include <vector>

using namespace std;

#define USER_POS 1
#define AI_POS 2
#define AVALABLE_POS 0

#define MAX_DEPTH 3
#define MAX_NODE 10000000

#define MAX  1000
#define MIN  -1000


class Board {
private:

    unsigned char grid[8][8];  // real card
    unsigned char mark[8][8];   // who play
    unsigned char board_size;

public:
    explicit Board(int size);

    void reset();

    int get_size();
    int get_at(int row, int col);

    int is_avalable(int row, int col);
    bool check_at(int row, int col);
    void check_board();
    bool can_play(int row, int col);

    bool add_card(int row, int col, int val, int player);
    void print_at(int row, int col, int option);
    void print_board();

    int get_max_card(int player);
    int get_score(int player);

    int deleted_ai();
    int deleted_user();

};


class Player {
private:
    unsigned char cards[11]{};
    unsigned char num_card{};
    bool is_user;

public:
    Player(bool is_user, int board_size);


    void reset_card(int board_size);
    unsigned char get_card(int id);
    int get_num_card();  //how many card
    bool has_card(int val);
    void print_card();
    void remove_card(int val);
    int sum_of_cards();
    int max_card();
    void ai_turn(const Board& board, const Player& user, const Player& ai, int& row, int& col, int& val);
    //for calculate max depth of minmax_pruning
    vector <int> get_unique_card();
    int calculate_depth(Board board, Player user,Player ai);
    vector<int> get_all_card();  // no use
    int num_unique_card(vector<int> orig_card); //no use
};

class Node {
private:
    Board board;
    Player user, ai;
    int new_row, new_col, new_val;
    int score;
    vector<Node> children;

public:
    Node(Board board, Player user, Player ai);

    bool is_terminal();

    void info_newest_child(int row, int col, int val);
    void add_child(bool is_user_turn);

    int minmax_prun(int now_depth,int max_depth, int is_user_turn, int alpha, int beta);
    void best_child(int& row, int& col, int& val);
    int estimate_cost();

    Player& get_user();
    Player& get_ai();
    Board& get_board();


};

class Game {
private:
    int is_user;
    int board_size;

    Board board;
    Player user, ai;

public:
    Game(int is_user, int board_size);

    void start(bool autoplay);
    bool end();
    void print();
    int get_result(int& user_score, int& ai_score, int& user_card, int& ai_card);
    bool is_exist_card(int val);
};
