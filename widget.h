#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QPen>
#include <QImage>
#include <QPixmap>
#include <QSize>
#include <qcursor.h>
#include <QDebug>
#include <QMouseEvent>
#include "MCTS_Algo.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT
    QImage white_chess_img;
    QImage black_chess_img;
    QImage white_backg;
    vector<QLabel*> all_chessboard_label;//放置所有label

    chessboard now_chessboard;
    MCTS_Algo my_algo;

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QImage ScaleImage2Label(QImage qImage, QLabel* qLabel);
    void ini();
    void render();
    void mousePressEvent(QMouseEvent *event);        //单击
private slots:
    void on_pushButton_compFirst_clicked();

    void on_pushButton_humanFirst_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
