#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ini();
}

Widget::~Widget()
{
    delete ui;
}
void Widget::render()//画图函数
{
    for(int i = 0; i < 8; ++i){
        for (int j = 0; j < 8; ++j) {
            if(now_chessboard.board[i][j]==-1)
                all_chessboard_label[i*8+j]->setPixmap(QPixmap::fromImage(black_chess_img));
            if(now_chessboard.board[i][j]==1)
                all_chessboard_label[i*8+j]->setPixmap(QPixmap::fromImage(white_chess_img));
        }
    }
}
void Widget::mousePressEvent(QMouseEvent *event)    //单击事件
{
    // 如果是鼠标左键按下
    if (event->button() == Qt::LeftButton){
        for(int i = 0; i < all_chessboard_label.size(); ++i){
            if(all_chessboard_label[i]->geometry().contains(this->mapFromGlobal(QCursor::pos()))){
                qDebug()<<"在控件"<<i<<endl;
                vector<int>able_pos;
                now_chessboard.getAblePos(1,able_pos);
                now_chessboard.outputAblePos(1);
                auto fi = ::find(able_pos.begin(), able_pos.end(), i);
                if(fi == able_pos.end()){
                    qDebug()<<"不允许下这里"<<endl;
                    return;
                }
                now_chessboard.chessDown(POINT(i/8,i%8), 1);//人下棋
                now_chessboard.reserve(i, 1);//翻转棋子
                my_algo.moveCurrentRoot(POINT(i/8,i%8));
                my_algo.updateChessboard(now_chessboard);
                if(my_algo.checkWin() == 1){
                    qDebug()<<"游戏结束"<<endl;
                    return;
                }
            }
        }
    }
    render();

    clock_t start, stop;//计时函数的返回值类型
    double  duration = 0;     /* 测量一个事件持续的时间*/
    start = clock();
    while (duration < TIME_LIMITATION){       //机器下,宏定义时间
        stop = clock();
        duration = (double)(stop - start)/ CLOCKS_PER_SEC;
        stack<MCTreeNode*> temp_stk;
        MCTreeNode* cur = my_algo.selection(temp_stk);
        my_algo.simulation_is_finished = 0;
        my_algo.simulation(cur, temp_stk);
        my_algo.bp(temp_stk);
    }
    cout<<(*my_algo.cur_root)->win_cnt<<'\t'<<(*my_algo.cur_root)->total_cnt<<endl;
    POINT p = my_algo.getBestOption();
    now_chessboard.chessDown(p, -1);
    now_chessboard.reserve(p.first*8+p.second, -1);

    render();

    my_algo.moveCurrentRoot(p);
    my_algo.updateChessboard(now_chessboard);
    if(my_algo.checkWin() == 1){
        qDebug()<<"游戏结束"<<endl;
        return;
    }


}
void Widget::ini()//读取文件
{
    if(!(white_chess_img.load("/Users/chenziwei/Qt project/AI_MCTS_BlackWhiteChess/whitechess_image.jpg"))) { //加载图像
        qDebug()<<"cant open this image"<<endl;
        return;
    }
    if(!(black_chess_img.load("/Users/chenziwei/Qt project/AI_MCTS_BlackWhiteChess/blackchess_image.jpg"))) { //加载图像
        qDebug()<<"cant open this image"<<endl;
        return;
    }
    if(!(white_backg.load("/Users/chenziwei/Qt project/AI_MCTS_BlackWhiteChess/bckg_image.jpg"))) { //加载图像
        qDebug()<<"cant open this image"<<endl;
        return;
    }

    for(int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8 ;++j ) {
            QLabel*t = new QLabel;
            all_chessboard_label.push_back(t);
            t->setParent(this);
            t->setGeometry(0+j*100+j,100*i+i,100,100);
            t->setPixmap(QPixmap::fromImage(ScaleImage2Label(white_backg,t)));
        }
    }
    white_backg = ScaleImage2Label(white_backg,all_chessboard_label[0]);
    black_chess_img = ScaleImage2Label(black_chess_img,all_chessboard_label[0]);
    white_chess_img = ScaleImage2Label(white_chess_img,all_chessboard_label[0]);
    now_chessboard.ini();//棋盘初始化
}
QImage Widget::ScaleImage2Label(QImage qImage, QLabel* qLabel)//lable图片裁剪函数
{
    QImage qScaledImage;
    QSize qImageSize = qImage.size();

    QSize qLabelSize = qLabel->size();
    double dWidthRatio = 1.0*qImageSize.width() / qLabelSize.width();
    double dHeightRatio = 1.0*qImageSize.height() / qLabelSize.height();
    if (dWidthRatio>dHeightRatio) {
        qScaledImage = qImage.scaledToWidth(qLabelSize.width());
    }
    else {
        qScaledImage = qImage.scaledToHeight(qLabelSize.height());
    }
    return qScaledImage;
}

void Widget::on_pushButton_compFirst_clicked()
{
    my_algo.current_chessboard = now_chessboard;
    clock_t start, stop;//计时函数的返回值类型
    double  duration = 0;     /* 测量一个事件持续的时间*/
    start = clock();
    while (duration < TIME_LIMITATION){       //机器下,宏定义时间
        stop = clock();
        duration = (double)(stop - start)/ CLOCKS_PER_SEC;
        stack<MCTreeNode*> temp_stk;
        MCTreeNode* cur = my_algo.selection(temp_stk);
        my_algo.simulation_is_finished = 0;
        my_algo.simulation(cur, temp_stk);
        my_algo.bp(temp_stk);
    }
    cout<<(*my_algo.cur_root)->win_cnt<<'\t'<<(*my_algo.cur_root)->total_cnt<<endl;
    POINT p = my_algo.getBestOption();//机器获得最好解
    now_chessboard.chessDown(p, -1);//机器下棋
    now_chessboard.reserve(p.first*8+p.second, -1);//翻转函数

    render();//画图
    my_algo.moveCurrentRoot(p);
    my_algo.updateChessboard(now_chessboard);
    now_chessboard.outputAblePos(1);//获取可下位置

}

void Widget::on_pushButton_humanFirst_clicked()//人先手
{
        render();
}
