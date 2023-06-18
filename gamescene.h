#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QWidget>
#include <QPushButton>
#include <QTimer>
#include <QDialog>
#include <QLabel>
#include "match.h"
#include "mainwindow.h"
#include "MyDialog.h"

namespace Ui {
class GameScene;
}

class GameScene : public QWidget
{
    Q_OBJECT
public:
    explicit GameScene(QWidget *parent = nullptr);
    ~GameScene();
    MyDialog * myDialog;
    QTimer* timer;
    int posX = -1, posY = -1; //棋盘坐标
    int P = 0;   //1为服务端用户，-1为客户端用户
    bool aa = 0, pause = true; // 机机模式
    int elapsedTime = 60; // 倒计时
    bool isSelect = 0;  //  能否落子
    bool repOther = false; //对方是否悔棋
    bool ConnOther = false; //是否连接成功
    int repentNum = 2; // 悔棋次数
    bool ServePrepare = false;
    bool ClientPrepare = false;
    const int aiTime = 300;
    bool flag=0;
    void initGame();
    void begHand();
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void aiVSai();
    void PDown();
    void gameOver();
    void startAiVsPlayerGame();
    void tongbugsc(int x,int y);
    void repent();
    void showConnStatus();
    void Set_Asp();
    void setReady();
    void setnReady();
    void set6nReady();
    void startTime(); //开始倒计时
    match *game;
private:
    Ui::GameScene *ui;
    QTimer* aiTimer;
    QTimer* aiTimer2;
signals:
    void ServeSelect(int x, int y);
    void ClientSelect(int x, int y);
    void SendReady();
    void SendBegHand(QString bh);
    void SendGiveup();
    void SendExit();
    void SendRepent();

private slots:
    void updateTimer();
    void AiDown();
    void on_backBtn_clicked();
    void on_restart_clicked();
    void on_repentBtn_clicked();
    void on_giveUpBtn_clicked();
};

#endif // GAMESCENE_H
