//
//  MCTS_Algo.cpp
//  Ai_MCTSearch(翻转棋)
//
//  Created by 陈梓玮 on 2021/4/27.
//

#include "MCTS_Algo.hpp"

bool MCTS_Algo::checkWin()
{
    vector<int> able_pos;
    if (my_chessboard.getAblePos(1, able_pos)) {
        if (my_chessboard.getAblePos(-1, able_pos)) {
            return true;
        }
    }
    return false;
}
void MCTS_Algo::moveCurrentRoot(POINT p)
{
    auto it = (*cur_root)->next.find(p);
    if (it != (*cur_root)->next.end()) {
        cur_root = &it->second;
    }
    return;
}

POINT MCTS_Algo::getBestOption()
{

    MCTreeNode* cur = *cur_root;
    vector<int>able_pos;
    current_chessboard.getAblePos(black, able_pos);
    uniform_int_distribution<int> r(0,(int)able_pos.size()-1);
    double max_reward = INT_MIN;
    auto it = cur->next.begin();
    auto next_step = cur->next.begin();

    for(; it != cur->next.end(); ++it){
        double temp = getReward(it->second, cur->total_cnt);
        auto fi = find(able_pos.begin(), able_pos.end(), it->first.first*8+it->first.second);
        if (fi == able_pos.end()) {
            continue;
        }
        if(max_reward < temp){
            max_reward = temp;
            next_step = it;
        }
    }

//    int x = able_pos[p]/8, y = able_pos[p] % 8;

    cout<<"Computer's option is:"<<next_step->first.first<<'\t'<<next_step->first.second<<endl;
        return next_step->first;
//    return POINT(x,y);

}
void MCTS_Algo::bp(stack<MCTreeNode*>& path)
{
    int is_win = my_chessboard.checkwin();
    MCTreeNode* cur = nullptr;
    while (!path.empty()) {//向上回溯bp
        cur = path.top(); path.pop();
        if (is_win == -1) //黑赢了，则向上黑全部+1，白不动
            cur->win_cnt++;
        cur->total_cnt++;
    }
}
bool MCTS_Algo::ableToExpand(MCTreeNode* cur)
{
    vector<int>able_pos;
    my_chessboard.getAblePos(cur->player, able_pos);
    if (able_pos.size() <= cur->next.size()) {
        return false;
    }
    return true;
}

MCTreeNode* MCTS_Algo::selection(stack<MCTreeNode*>& path)
{
    my_chessboard = current_chessboard;
    MCTreeNode *cur = *cur_root;
    vector<int> able_pos;

    while (cur->next.size() != 0) {     //一直直到到达叶子节点
        path.push(cur);

        if (ableToExpand(cur)) {         //还有节点未扩展，则扩展节点
            return expansion(cur);
        }

        //全部节点扩展完了，则计算bestchild，并选择bestchild作为下一个深入的节点 map实现方式
        double max_reward = INT_MIN;
        auto it = cur->next.begin();
        auto next_step = cur->next.begin();
        for(; it != cur->next.end(); ++it){
            double temp = getReward(it->second, cur->total_cnt);
                    if(max_reward < temp){
                        max_reward = temp;
                        next_step = it;
                    }
        }
        my_chessboard.chessDown(next_step->first, cur->player);
        my_chessboard.reserve(next_step->first.first*8+ next_step->first.second, cur->player);
        cur = next_step->second;

    }

    return cur;
}
double MCTS_Algo::getReward(MCTreeNode* const& n,const int& total_cnt)
{

    return  (double)n->win_cnt/n->total_cnt + (double)BALANCE_FACTOR* pow(2*log(total_cnt), 0.5)/n->total_cnt;

}
MCTreeNode* MCTS_Algo::expansion(MCTreeNode* cur)
{
    path.push(cur);
    vector<int> able_pos;
    my_chessboard.getAblePos(cur->player, able_pos);
    if (able_pos.size() == 0 /*|| able_pos.size() <= cur->next.size()*/) {
        return cur;
    }

    bool is_duplicated = 0;
    int x, y;
    int next_step;
    MCTreeNode* new_node = NULL;
    while (1)
    {
        uniform_int_distribution<int> rand_pos(0,(int)able_pos.size()-1);
        next_step = rand_pos(e);
        x = able_pos[next_step] / 8;
        y = able_pos[next_step] % 8;//随机选一个可走位置

        if(cur->next.find(pair<int, int>(x, y)) != cur->next.end())
            continue;
        new_node = new MCTreeNode(cur->player*-1);
        auto it = cur->next.insert(pair<POINT,MCTreeNode*>(pair<int, int>(x, y), new_node));
        is_duplicated = !it.second;
        if (is_duplicated == false) {
            break;
        }
    }
    my_chessboard.chessDown(POINT(x, y), cur->player);
    my_chessboard.reserve(able_pos[next_step], cur->player);
    return new_node;
}
void MCTS_Algo::simulation(MCTreeNode* cur, stack<MCTreeNode*>& path)//dfs
{
    //随机向下直至游戏结束

    static bool both_no_place_to_downchess = 0;
    while (1) {
        if (simulation_is_finished == 1) {
            return;
        }
        path.push(cur);
        vector<int> able_pos;
        my_chessboard.getAblePos(cur->player, able_pos);

        if (able_pos.size() == 0 && both_no_place_to_downchess == 0) {
            //如果没棋可下，而且没下完，则换方
            path.pop();
            both_no_place_to_downchess = 1;
            simulation(cur, path);

        }
        while(able_pos.size() == 0 && both_no_place_to_downchess == 1)//递归进去了所以要两次return
        {
            simulation_is_finished = 1;
            return;//游戏结束
        }

        both_no_place_to_downchess = 0;

        int x, y;
        int next_step;

        uniform_int_distribution<int> rand_pos(0,(int)able_pos.size()-1);
        next_step = rand_pos(e);
        x = able_pos[next_step] / 8;
        y = able_pos[next_step] % 8;//随机选一个可走位置

        auto fi = cur->next.find(pair<int, int>(x, y));
        MCTreeNode *new_node = NULL;
        if (fi == cur->next.end()){
            new_node = new MCTreeNode(cur->player*-1);
            cur->next.insert(pair<POINT,MCTreeNode*>(pair<int, int>(x, y), new_node));//新建一个节点进去
            my_chessboard.chessDown(POINT(x, y), cur->player);
            my_chessboard.reserve(able_pos[next_step], cur->player);
            cur = new_node;
        }
        else{
            my_chessboard.chessDown(POINT(x, y), cur->player);
            my_chessboard.reserve(able_pos[next_step], cur->player);
            cur = fi->second;
        }

    }
    return ;
}

void chessboard::outputAblePos(int player)
{
    vector<int> able_pos;//用于获得可以下的位置
    getAblePos(player, able_pos);
    for (int i = 0; i < able_pos.size(); ++i) {
        int x = able_pos[i] / 8;
        int y = able_pos[i] % 8;
        cout<<"able chess pos:"<<x<<'\t'<<y<<endl;
    }
}
void chessboard::reserve(int pos,int player)
{
    vector<int> x;
    vector<int> y;
    int line = pos / 8;
    int col = pos % 8;
    int curi = line+1;
    int curj = col;
    while (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && board[curi][curj] == 0 - player)
    {
        x.push_back(curi);
        y.push_back(curj);
        curi++;
    }
    if (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && board[curi][curj] == player)
    {
        for (int i = 0; i < x.size(); i++)
        {
            board[x[i]][y[i]] = player;
        }
    }
    x.clear();
    y.clear();
    curi = line - 1;
    curj = col;
    while (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && board[curi][curj] == 0 - player)
    {
        x.push_back(curi);
        y.push_back(curj);
        curi--;
    }
    if (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && board[curi][curj] == player)
    {
        for (int i = 0; i < x.size(); i++)
        {
            board[x[i]][y[i]] = player;
        }
    }
    x.clear();
    y.clear();
    curi = line;
    curj = col+1;
    while (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && board[curi][curj] == 0 - player)
    {
        x.push_back(curi);
        y.push_back(curj);
        curj++;
    }
    if (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && board[curi][curj] == player)
    {
        for (int i = 0; i < x.size(); i++)
        {
            board[x[i]][y[i]] = player;
        }
    }
    x.clear();
    y.clear();
    curi = line;
    curj = col -1;
    while (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && board[curi][curj] == 0 - player)
    {
        x.push_back(curi);
        y.push_back(curj);
        curj--;
    }
    if (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && board[curi][curj] == player)
    {
        for (int i = 0; i < x.size(); i++)
        {
            board[x[i]][y[i]] = player;
        }
    }
    x.clear();
    y.clear();
    curi = line+1;
    curj = col +1;
    while (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && board[curi][curj] == 0 - player)
    {
        x.push_back(curi);
        y.push_back(curj);
        curi++;
        curj++;
    }
    if (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && board[curi][curj] == player)
    {
        for (int i = 0; i < x.size(); i++)
        {
            board[x[i]][y[i]] = player;
        }
    }
    x.clear();
    y.clear();
    curi = line - 1;
    curj = col - 1;
    while (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && board[curi][curj] == 0 - player)
    {
        x.push_back(curi);
        y.push_back(curj);
        curi--;
        curj--;
    }
    if (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && board[curi][curj] == player)
    {
        for (int i = 0; i < x.size(); i++)
        {
            board[x[i]][y[i]] = player;
        }
    }
    x.clear();
    y.clear();
    curi = line + 1;
    curj = col - 1;
    while (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && board[curi][curj] == 0 - player)
    {
        x.push_back(curi);
        y.push_back(curj);
        curi++;
        curj--;
    }
    if (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && board[curi][curj] == player)
    {
        for (int i = 0; i < x.size(); i++)
        {
            board[x[i]][y[i]] = player;
        }
    }
    x.clear();
    y.clear();
    curi = line - 1;
    curj = col + 1;
    while (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && board[curi][curj] == 0 - player)
    {
        x.push_back(curi);
        y.push_back(curj);
        curi--;
        curj++;
    }
    if (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && board[curi][curj] == player)
    {
        for (int i = 0; i < x.size(); i++)
        {
            board[x[i]][y[i]] = player;
        }
    }
    x.clear();
    y.clear();
}

chessboard::chessboard(const chessboard& b)
{
    board.resize(8,vector<int>(8,0));
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            board[i][j] = b.board[i][j];
        }
    }
}
int chessboard::checkwin()
{
    int countw = 0;
    int countb = 0;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j] == white)
                countw++;
            if (board[i][j] == black)
                countb++;
        }
    }

    if (countw > countb)
        return white;
    if (countw < countb)
        return black;
    if (countw == countb)
        return INT_MIN;//平局
    return 0;

}
//int chessboard::checkwin2()
//{
//    int countw = 0;
//    int countb = 0;
//    for (int i = 0; i < 8; i++)
//    {
//        for (int j = 0; j < 8; j++)
//        {
//            if (board[i][j] == white)
//                countw++;
//            if (board[i][j] == black)
//                countb++;
//        }
//    }
//    if (countw > countb)
//        return white;
//    if (countw < countb)
//        return black;
//    return 0;
//}
bool chessboard::getAblePos(int player,vector<int>& pos)
{
    pos.clear();
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j] == 0)
            {
                if (i + 1 <= 7 && board[i + 1][j] == 0 - player)
                {
                    bool wflag = 0;
                    bool bflag = 0;
                    int curi = i + 1;
                    int curj = j;
                    while (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && (wflag == 0 || bflag == 0))
                    {
                        if (board[curi][curj] == 0)
                            break;
                        if (board[curi][curj] == white)
                            wflag = 1;
                        if (board[curi][curj] == black)
                            bflag = 1;
                        curi++;
                    }
                    if (wflag == 1 && bflag == 1)
                        pos.push_back(i * 8 + j);
                }
                if ( i-1 >= 0 && board[i-1][j] == 0 - player)
                {
                    bool wflag = 0;
                    bool bflag = 0;
                    int curi = i -1;
                    int curj = j;
                    while (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && (wflag == 0 || bflag == 0))
                    {
                        if (board[curi][curj] == 0)
                            break;
                        if (board[curi][curj] == white)
                            wflag = 1;
                        if (board[curi][curj] == black)
                            bflag = 1;
                        curi--;
                    }
                    if (wflag == 1 && bflag == 1)
                        pos.push_back(i * 8 + j);
                }
                if (j+1 <= 7 && board[i][j+1] == 0 - player)
                {
                    bool wflag = 0;
                    bool bflag = 0;
                    int curi = i;
                    int curj = j+1;
                    while (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && (wflag == 0 || bflag == 0))
                    {
                        if (board[curi][curj] == 0)
                            break;
                        if (board[curi][curj] == white)
                            wflag = 1;
                        if (board[curi][curj] == black)
                            bflag = 1;
                        curj++;
                    }
                    if (wflag == 1 && bflag == 1)
                        pos.push_back(i * 8 + j);
                }
                if (j-1 >= 0 && board[i][j -1] == 0 - player)
                {
                    bool wflag = 0;
                    bool bflag = 0;
                    int curi = i;
                    int curj = j -1;
                    while (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && (wflag == 0 || bflag == 0))
                    {
                        if (board[curi][curj] == 0)
                            break;
                        if (board[curi][curj] == white)
                            wflag = 1;
                        if (board[curi][curj] == black)
                            bflag = 1;
                        curj--;
                    }
                    if (wflag == 1 && bflag == 1)
                        pos.push_back(i * 8 + j);
                }
                if (i + 1 <= 7 && j + 1 <= 7 && board[i+1][j +1] == 0 - player)
                {
                    bool wflag = 0;
                    bool bflag = 0;
                    int curi = i+1;
                    int curj = j+1;
                    while (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && (wflag == 0 || bflag == 0))
                    {
                        if (board[curi][curj] == 0)
                            break;
                        if (board[curi][curj] == white)
                            wflag = 1;
                        if (board[curi][curj] == black)
                            bflag = 1;
                        curj++;
                        curi++;
                    }
                    if (wflag == 1 && bflag == 1)
                        pos.push_back(i * 8 + j);
                }
                if (i - 1 >= 0 && j - 1 >= 0 && board[i - 1][j - 1] == 0 - player)
                {
                    bool wflag = 0;
                    bool bflag = 0;
                    int curi = i - 1;
                    int curj = j - 1;
                    while (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && (wflag == 0 || bflag == 0))
                    {
                        if (board[curi][curj] == 0)
                            break;
                        if (board[curi][curj] == white)
                            wflag = 1;
                        if (board[curi][curj] == black)
                            bflag = 1;
                        curj--;
                        curi--;
                    }
                    if (wflag == 1 && bflag == 1)
                        pos.push_back(i * 8 + j);
                }
                if (i + 1 <= 7 && j - 1 >= 0 && board[i + 1][j - 1] == 0 - player)
                {
                    bool wflag = 0;
                    bool bflag = 0;
                    int curi = i + 1;
                    int curj = j - 1;
                    while (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && (wflag == 0 || bflag == 0))
                    {
                        if (board[curi][curj] == 0)
                            break;
                        if (board[curi][curj] == white)
                            wflag = 1;
                        if (board[curi][curj] == black)
                            bflag = 1;
                        curj--;
                        curi++;
                    }
                    if (wflag == 1 && bflag == 1)
                        pos.push_back(i * 8 + j);
                }
                if (i - 1 >= 0 && j + 1 <= 7 && board[i -1][j + 1] == 0 - player)
                {
                    bool wflag = 0;
                    bool bflag = 0;
                    int curi = i - 1;
                    int curj = j + 1;
                    while (curi >= 0 && curi < 8 && curj >= 0 && curj < 8 && (wflag == 0 || bflag == 0))
                    {
                        if (board[curi][curj] == 0)
                            break;
                        if (board[curi][curj] == white)
                            wflag = 1;
                        if (board[curi][curj] == black)
                            bflag = 1;
                        curj++;
                        curi--;
                    }
                    if (wflag == 1 && bflag == 1)
                        pos.push_back(i * 8 + j);
                }
            }
        }
    }
    if (pos.size() == 0)
        return false;
    sort(pos.begin(), pos.end());
    pos.erase(unique(pos.begin(), pos.end()), pos.end());

    return true;
}
void chessboard::ini()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            board[i][j] = 0;
        }
    }
    board[3][3] = white;
    board[4][4] = white;
    board[3][4] = black;
    board[4][3] = black;
}

void chessboard::clear()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            board[i][j] = 0;
        }
    }
    board[3][3] = white;
    board[4][4] = white;
    board[3][4] = black;
    board[4][3] = black;
}
void chessboard::output()
{
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[i].size(); ++j) {
            cout<<board[i][j]<<'\t';
        }
        cout<<endl;
    }
    cout<<endl;
}

bool chessboard::chessDown(POINT p, int player)
{
    int x = p.first, y = p.second;
    vector<int> able_pos;
    if (x>=0 && x <= 7 && y>=0 && y<= 7) {
        getAblePos(player, able_pos);
        for (int i = 0; i < able_pos.size(); ++i) {
            if (able_pos[i] == x*8+y) {//如果该地方能下棋，则下棋
                board[x][y] = player;
                return true;
            }
        }
    }
//    cerr<<"这个地方下不了啊"<<p.first<<'\t'<<p.second<<endl;
    return 0;//不能下棋返回false
}
