#ifndef CONNGAME_H
#define CONNGAME_H

#include <QWidget>
#include "gameclient.h"
#include "gameserve.h"
//#include"gamescene.h"
#include<QPainter>
namespace Ui {
class ConnGame;
}

class ConnGame : public QWidget
{
    Q_OBJECT

public:
    explicit ConnGame(QWidget *parent = nullptr);
    ~ConnGame();
    bool select = false;
    unsigned short port=8899;
   // GameScene * game;
    GameClient * gc;
    GameServe * gs;
    //绘图事件
    void paintEvent(QPaintEvent *);
signals:
    void clientconn();
    void serverconn();

private slots:
    void on_backBtn_clicked();
    
    void on_creatBtn_clicked();
    
    void on_joinBtn_clicked();
    
private:
    Ui::ConnGame *ui;
};

#endif // CONNGAME_H
