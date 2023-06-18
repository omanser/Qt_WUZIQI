#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include <QObject>
#include<QTcpSocket>
#include<QHostAddress>
#include<QPoint>
#include<gamescene.h>
#include "MyDialog.h"

class GameClient : public QObject
{
    Q_OBJECT
public:
    explicit GameClient(QObject *parent = nullptr);
    ~GameClient();
    int x, y;
    QTcpSocket * m_s;
    QString ip;
    GameScene * gsc;
    MyDialog *myDialog;
    unsigned short port = 8899;
    void p_connection();
    void SendOK(bool);
    void ReturnXY(QStringList);
    void back();
    void end();
signals:
    void connectiondata(int x,int y);
    void c_close();

private slots:
    void SendMsg(int x, int y); //发送落子的位置信息
    void SendReady();
    void SendGiveup();  //认输
    void SendRepent(); //悔棋
    void SendBack();
    void SendBegHand(QString); //先手
};

#endif // GAMECLIENT_H
