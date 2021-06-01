//
//  MCTS_Algo.hpp
//  Ai_MCTSearch(翻转棋)
//
//  Created by 陈梓玮 on 2021/4/27.
//

#ifndef MCTS_Algo_hpp
#define MCTS_Algo_hpp

#include <iostream>
#include <vector>
#include <random>
#include <stack>
#include <utility>
#include <map>
#include <math.h>
#include <algorithm>
using namespace std;


#define white 1
#define black -1

#define POINT pair<int, int>//宏定义，二元组，x,y坐标
#define BALANCE_FACTOR 20
#define TIME_LIMITATION 1
class chessboard
{
public:
    vector<vector<int>> board;//棋盘
    //constructor
    chessboard():board(8,vector<int>(8,0)){}
    chessboard(const chessboard& b);
    chessboard& operator=(const chessboard& b){
        for(int i = 0; i < 8; ++i)
        {
            for(int j = 0; j < 8; ++j)
            board[i][j] = b.board[i][j];
        }
        return *this;
    }

    void ini();//初始化棋盘
    void clear();
    bool getAblePos(int player,vector<int>& pos);//获得可以下的位置
    int checkwin();    //检查是否获胜赢
//    int checkwin2();
    void reserve(int pos,int player);//给定0-63落子位置，实现棋子翻转    //参数palyer如宏定义，白棋1黑棋-1
    void output();//输出当前棋盘
    bool chessDown(POINT p, int player);//第一个参数是落子位置，用point(x,y)作为参数传入即可
    void outputAblePos(int player);
    
};

struct PointCmp{
    bool operator()(const POINT p1, const POINT p2)const{
        return p1.first * 8 + p1.second < p2.first * 8 + p2.second;
    }
};

struct MCTreeNode{
    int win_cnt = 0;
    int total_cnt = 0;
    map<POINT, MCTreeNode*, PointCmp> next;
//    vector<pair<POINT, MCTreeNode*>> next;
    int player = 1;
    MCTreeNode(int p):player(p){}
};
class MCTS_Algo{
    stack<MCTreeNode*> path;
    mt19937 e;
    MCTreeNode* root;
    
public:
    MCTreeNode** cur_root;
    MCTS_Algo(){
        my_chessboard.ini();
        root = new MCTreeNode(black);
        cur_root = &root;
    }
    chessboard current_chessboard;
    chessboard my_chessboard;
    
    void updateChessboard(chessboard& b){
        current_chessboard = b;
    }
    MCTreeNode* selection(stack<MCTreeNode*>& path);
    bool ableToExpand(MCTreeNode* cur);
    double getReward(MCTreeNode* const& n,const int& total_cnt);
    MCTreeNode* expansion(MCTreeNode* cur);
    bool simulation_is_finished = 0;
    void simulation(MCTreeNode* cur, stack<MCTreeNode*>& path);//dfs自带向上回溯
    void bp(stack<MCTreeNode*>& path);
    
    POINT getBestOption();
    void moveCurrentRoot(POINT p);
    bool checkWin();
};



#endif /* MCTS_Algo_hpp */
