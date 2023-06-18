#include "gameclient.h"
#include"mainwindow.h"
#include<QMessageBox>
GameClient::GameClient(QObject *parent) : QObject(parent)
{
    m_s=new QTcpSocket(this);
    gsc = new GameScene;
    myDialog = new MyDialog;
    gsc->P = -1;
    gsc->showConnStatus();
    qDebug()<<2;
    connect(gsc, &GameScene::SendReady, this, &GameClient::SendReady);
    connect(gsc, &GameScene::ClientSelect, this, &GameClient::SendMsg);
    connect(gsc, &GameScene::SendExit, this, &GameClient::SendBack);
    connect(gsc, &GameScene::SendRepent, this, &GameClient::SendRepent);
    connect(gsc, &GameScene::SendBegHand, this, &GameClient::SendBegHand);
    connect(gsc, &GameScene::SendGiveup, this, &GameClient::SendGiveup);
    //建立连接
    connect(m_s,&QTcpSocket::readyRead,[=](){
        QByteArray data = m_s->readAll();  // 读取接收到的数据
            // 解析接收到的数据
        QString receivedData = QString::fromUtf8(data);  // 将字节数组转换为QString
        QStringList dividelist = receivedData.split(",");  // 将字符串按逗号分割成字符串列表
        qDebug() << "读取成了：" << receivedData;

        if(dividelist.indexOf("认输") != -1)
        {
            gsc->timer->stop();
            gsc->gameOver();
            myDialog->showMsg("对方认输，你赢了");
        }
        if(dividelist.indexOf("准备") != -1)
        {
            gsc->ServePrepare = true;
            if(gsc->ClientPrepare) gsc->startTime();
            //QMessageBox::information(gsc, "提示", "对方已准备");
            gsc->setReady();
        }
        if(dividelist.indexOf("悔棋") != -1)
        {
            gsc->timer->stop();
            myDialog->showMsg("是否同意悔棋");
            if(myDialog->exec() == QDialog::Accepted)
            {
                SendOK(true);
                gsc->startTime();
            }
            else
            {
                SendOK(false);
                gsc->timer->start();
            }
        }
        if(dividelist.indexOf("同意") != -1)
        {
            gsc->repentNum--;
            gsc->repent();
            gsc->startTime();
            //QMessageBox::information(gsc, "消息", "随便你悔，怎么悔都下不过我的");
        }
        else if(dividelist.indexOf("不同意") != -1)
        {
            gsc->timer->start();
            myDialog->showMsg("别想悔棋哦！");
        }
        if(dividelist.indexOf("退出") != -1)
        {
            qDebug() << "对方退出，连接中断！";
            gsc->timer->stop();
            QMessageBox::information(gsc, "五子棋", "对方退出，你赢了，即将返回");
            //gsc->gameOver();
            MainWindow *m = new MainWindow;
            m->show();
            delete this;
        }

        ReturnXY(dividelist);
        //触发信号
    });
    connect(m_s,&QTcpSocket::disconnected,[=](){
        qDebug() << "对方退出，连接中断！";
        gsc->timer->stop();
        m_s->close();
        gsc->ConnOther = false;

        qDebug() << "断开连接";
    });
}

GameClient::~GameClient()
{
    delete gsc;
    delete m_s;
}

void GameClient::p_connection()
{
    m_s->connectToHost(QHostAddress(ip),port);
    if(m_s->waitForConnected()){
        qDebug()<<"链接成功";
        emit c_close();
        gsc->setWindowTitle("联机模式");
        gsc->show();
        gsc->ConnOther = true;
        gsc->setnReady();
    }
    else {
        qDebug()<<"链接失败";
        QMessageBox::warning(NULL,"Error","先创房或者ip端口输错了");
    }
}

void GameClient::SendOK(bool f)
{
    QString sigString = QString("不同意,");
    if(f)
    {
        sigString = QString("同意,");
        gsc->repOther = true;
        gsc->repent();
        gsc->repOther = false;
    }
    this->m_s->write(sigString.toUtf8());
    this->m_s->flush();  // 确保数据立即发送
}

void GameClient::ReturnXY(QStringList dividelist)
{
    int x, y;
    bool ok;
    for(auto i = dividelist.begin(); i != dividelist.end(); i++){
        i->toInt(&ok, 10);
        if(ok){
            x = i->toInt(&ok, 10);
            y = (++i)->toInt(&ok, 10);
            qDebug()<<x<<y<<"坐标";
            gsc->tongbugsc(x,y);
            gsc->isSelect = true;
            return ;
        }
    }
}

void GameClient::end()
{
    gsc->game->gameStates = false;
    gsc->ClientPrepare = false;
    gsc->ServePrepare = false;
    gsc->set6nReady();
    gsc->setnReady();
}

void GameClient::SendGiveup()
{
    QString sigString = QString("认输,");
    this->m_s->write(sigString.toUtf8());
    this->m_s->flush();  // 确保数据立即发送
    myDialog->showMsg("你输了，再来一局?");
    gsc->gameOver();
    gsc->timer->stop();
}

void GameClient::SendRepent()
{
    QString sigString = QString("悔棋,");
    this->m_s->write(sigString.toUtf8());
    this->m_s->flush();  // 确保数据立即发送
    gsc->timer->stop();
}

void GameClient::SendBack()
{
    QString sigString = QString("退出,");
    this->m_s->write(sigString.toUtf8());
    this->m_s->flush();  // 确保数据立即发送
    delete this;
    MainWindow *m = new MainWindow;
    m->show();
}

void GameClient::SendBegHand(QString str)
{
    //qDebug() << "先手信息";
    gsc->ClientPrepare = true;
    this->m_s->write(str.toUtf8());
    this->m_s->flush();  // 确保数据立即发送
}

void GameClient::SendMsg(int posX, int posY)
{
    qDebug() << "成功！";
    QPoint point(posX, posY);
    QString pointString = QString("%1,%2,").arg(point.x()).arg(point.y());
    this->m_s->write(pointString.toUtf8());
    this->m_s->flush();  // 确保数据立即发送
}

void GameClient::SendReady()
{
    QString sigString = QString("准备,");
    this->m_s->write(sigString.toUtf8());
    this->m_s->flush();  // 确保数据立即发送
    gsc->ClientPrepare = true;
    if(gsc->ServePrepare && gsc->ClientPrepare) gsc->startTime();
}
