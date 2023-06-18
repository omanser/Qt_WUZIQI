#include "gameserve.h"
#include<QDebug>
#include <QMessageBox>
GameServe::GameServe(QWidget *parent) : QWidget(parent)
{
    m_s=new QTcpSocket(this);
    m_tcp=new QTcpServer(this);
    myDialog = new MyDialog;
    game->P = 1;
    game->show();
    game->showConnStatus();
    connect(game, &GameScene::SendReady, this, &GameServe::SendReady); //准备
    connect(game, &GameScene::ServeSelect, this, &GameServe::SendMsg); //落子信息
    connect(game, &GameScene::SendExit, this, &GameServe::SendBack);    //返回主界面
    connect(game, &GameScene::SendRepent, this, &GameServe::SendRepent); //请求悔棋
    connect(game, &GameScene::SendGiveup, this, &GameServe::SendGiveup);
    //建立连接
    connect(m_tcp,&QTcpServer::newConnection,[=](){
        game->ConnOther = true;
        game->setnReady();
       m_s=m_tcp->nextPendingConnection();
       connect(m_s,&QTcpSocket::readyRead,[=](){
           QByteArray data = m_s->readAll();  // 读取接收到的数据
               // 解析接收到的数据
           QString receivedData = QString::fromUtf8(data);  // 将字节数组转换为QString
           QStringList dividelist = receivedData.split(",");  // 将字符串按逗号分割成字符串列表
           qDebug() << "读取成了：" << receivedData;

           if(dividelist.indexOf("认输") != -1)
           {
               game->timer->stop();
               game->gameOver();
               myDialog->showMsg("对方认输，你赢了");
//               if(QMessageBox::Yes ==
//               QMessageBox::information(game,"结算","对方认输，?",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes))
//               {
////                   game->initGame();
////                   SendReady();
//               }
//               else
//               {
//                   MainWindow *m = new MainWindow;
//                   m->show();
//                   delete this;
//               }
           }
           if(dividelist.indexOf("准备") != -1)
           {
               game->ClientPrepare = true;
               game->setReady();
               if(game->ServePrepare) game->startTime();
               //QMessageBox::information(game, "提示", "对方已准备");
           }
           if(dividelist.indexOf("先手") != -1)
           {
               //qDebug() << "有没有来";
               game->isSelect = true;
               game->Set_Asp();
               myDialog->showMsg("先手");
               //QMessageBox::information(game, "提示", "先手");
           }
           else if(dividelist.indexOf("后手") != -1)
           {
               game->isSelect = false;
               game->Set_Asp();
               myDialog->showMsg("后手");
               //MessageBox::information(game, "提示", "后手");
           }
           if(dividelist.indexOf("悔棋") != -1)
           {
               game->timer->stop();
               myDialog->showMsg("是否同意悔棋");
               if(myDialog->exec() == QDialog::Accepted)
               {
                   SendOK(true);
                   game->startTime();
               }
               else
               {
                   SendOK(false);
                   game->timer->start();
               }
           }
           if(dividelist.indexOf("同意") != -1)
           {
               game->startTime();
               game->repentNum--;
               game->repent();
               //QMessageBox::information(game, "消息", "随便悔，怎么悔都下不过我的");
           }
           else if(dividelist.indexOf("不同意") != -1)
           {
               game->timer->start();
               myDialog->showMsg("别想悔棋哦！");
               //QMessageBox::information(game, "消息", "别想悔棋哦！");
           }
           if(dividelist.indexOf("退出") != -1)
           {
               qDebug() << "对方退出，连接中断！";
               game->timer->stop();
               if(QMessageBox::Yes ==
               QMessageBox::information(game,"消息","对方退出，你赢了！是否结束游戏",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes))
               {
                   game->ClientPrepare = false;
                   MainWindow *m = new MainWindow;
                   m->show();
                   delete this;
               }
               else
               {
                   game->ConnOther = false;
               }
           }
           ReturnXY(dividelist);
       });
       connect(m_s,&QTcpSocket::disconnected,[=](){
           qDebug() << "对方退出，连接中断！";
           game->timer->stop();
           m_s->close();
           game->ConnOther = false;
           qDebug() << "断开连接";
       });
    });

}

GameServe::~GameServe()
{
    delete game;
    delete m_s;
}



void GameServe::p_connection()
{
    m_tcp->listen(QHostAddress::Any,port);
}

void GameServe::SendOK(bool f)
{
    QString sigString = QString("不同意,");
    if(f)
    {
        sigString = QString("同意,");
        game->repOther = true;
        game->repent();
        game->repOther = false;
    }
    this->m_s->write(sigString.toUtf8());
    this->m_s->flush();  // 确保数据立即发送
}

void GameServe::ReturnXY(QStringList dividelist)
{
    int x, y;
    bool ok;
    for(auto i = dividelist.begin(); i != dividelist.end(); i++){
        i->toInt(&ok, 10);
        if(ok){
            x = i->toInt(&ok, 10);
            y = (++i)->toInt(&ok, 10);
            qDebug()<<x<<y<<"坐标";
            game->tongbugsc(x,y);
            game->isSelect = true;
            return ;
        }
    }
}

void GameServe::end()
{
    game->game->gameStates = false;
    game->ClientPrepare = false;
    game->ServePrepare = false;
    game->set6nReady();
    game->setnReady();
}

void GameServe::SendMsg(int posX, int posY)
{
    QPoint point(posX, posY);
    QString pointString = QString("%1,%2,").arg(point.x()).arg(point.y());
    this->m_s->write(pointString.toUtf8());
    this->m_s->flush();  // 确保数据立即发送
}

void GameServe::SendGiveup()
{
    QString sigString = QString("认输,");
    this->m_s->write(sigString.toUtf8());
    this->m_s->flush();  // 确保数据立即发送
    myDialog->showMsg("你输了，再来一局?");
    game->gameOver();
    game->timer->stop();
}

void GameServe::SendRepent()
{
    QString sigString = QString("悔棋,");
    this->m_s->write(sigString.toUtf8());
    this->m_s->flush();  // 确保数据立即发送
    game->timer->stop();
}

void GameServe::SendBack()
{
    if(game->ConnOther){
        QString sigString = QString("退出,");
        this->m_s->write(sigString.toUtf8());
        this->m_s->flush();  // 确保数据立即发送
        delete this;
    }
    MainWindow *m = new MainWindow;
    m->show();
}

void GameServe::SendReady()
{
    QString sigString = QString("准备,");
    this->m_s->write(sigString.toUtf8());
    this->m_s->flush();  // 确保数据立即发送
    game->ServePrepare = true;
    if(game->ServePrepare && game->ClientPrepare) game->startTime();
}
