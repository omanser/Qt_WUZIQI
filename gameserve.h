#ifndef GAMESERVE_H
#define GAMESERVE_H

#include <QWidget>
#include<QTcpServer>
#include<QTcpSocket>
#include "gamescene.h"
#include "MyDialog.h"

class GameServe : public QWidget
{
    Q_OBJECT
public:
    explicit GameServe(QWidget *parent = nullptr);
    ~GameServe();
    QTcpServer * m_tcp;
    QTcpSocket * m_s;
    GameScene * game = new GameScene;
    MyDialog *myDialog;
    QString ip;
    int x,y;
    unsigned short port=8899;
    void p_connection();
    void SendOK(bool);
    void ReturnXY(QStringList);
    void end();

signals:
    void connectiondata(int x,int y);

private slots:
    void SendMsg(int x, int y);
    void SendGiveup();  //认输
    void SendRepent(); //悔棋
    void SendBack();
    void SendReady();
};

#endif // GAMESERVE_H
