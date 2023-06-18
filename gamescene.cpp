#include "gamescene.h"
#include "ui_GameScene.h"
#include <QPainter>
#include <QMessageBox>
#include <QDebug>
#include <QBrush>
#include <QMouseEvent>
#include <QPushButton>
#include <QStatusBar>
#include <cmath>
#include <QPoint>
#include <QToolTip>
GameScene::GameScene(QWidget *parent) : QWidget(parent),
    ui(new Ui::GameScene)
{

    myDialog = new MyDialog(this);
    srand((unsigned)time(NULL));
    game = new match;
    ui->setupUi(this);
    if(P==0)
    {
        ui->label->hide();
        ui->label_4->hide();
        ui->label_5->hide();
        ui->label_6->hide();
        ui->label_7->hide();
        ui->label_8->hide();
        ui->label_9->hide();
        ui->label_10->hide();
        ui->label_11->hide();
        ui->times->hide();
    }
    this->setFixedSize(1100, 750);
    this->setMouseTracking(true);
    //initGame();
    QPixmap pixmap(":/Image/black.png");
    this->setWindowIcon(pixmap);
    aiTimer = new QTimer(this);
    aiTimer2 = new QTimer(this);//设置定时器
    timer = new QTimer(this);
    ui->times->setText("60s");
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));
    update();
}


void GameScene::gameOver()
{
    ui->repentBtn->setText("回溯");
    if(P != 0)
    {
        ui->restart->setEnabled(true);
        ui->restart->setText("准备");
    }
    game->gameStates = false;
    ClientPrepare = false;
    ServePrepare = false;
    ui->label_6->setText("未准备");
    ui->label_8->setText("未准备");
    timer->stop();
    //game->initMap();
    if(!game->isDraw()){
        QString str;
        if(!game->player == game->chess)
            str = "黑棋";
        else
            str = "白棋";
        myDialog->showMsg(str + "胜利");
    }
    else {
        myDialog->showMsg("棋逢对手");
    }
}

GameScene::~GameScene()
{
    delete ui;
}

void GameScene::initGame(){
    if(game) delete game;
    game = new match;
    if(P==-1) begHand();
    else if(P == 0) Set_Asp();
    //if(P==0)startTime();
    game->startGame();
    ui->repentBtn->setText("悔棋");
}

void GameScene::begHand()
{
    if(P == 1) return ;
    isSelect = rand() % 2;  //随机先手
    if(isSelect)
    {
        emit SendBegHand("后手,");
        ui->label_3->setText("黑棋");
        myDialog->showMsg("你获得先手");
    }
    else
    {
        emit SendBegHand("先手,");
        ui->label_3->setText("白棋");
        myDialog->showMsg("对方先手");
    }
}

void GameScene::paintEvent(QPaintEvent *event)
{
    if(ConnOther)
        ui->label_4->setText("连接成功");
    else
        ui->label_4->setText("未连接");

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true); // 抗锯齿
    painter.drawPixmap(0, 0, this->width(),this->height(),QPixmap(":/Image/3.png"));

    //绘制棋盘
    painter.translate(15, 15);
    QBrush brush;
    brush.setColor(QColor(253, 218, 182));
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    painter.drawRect(0, 0, (Size-1)*wide + 50, (Size-1)*wide + 50);
    painter.translate(25, 25);
    for(int i = 0; i < Size; ++i)
    {
        painter.drawLine(0, wide*i, (Size-1)*wide, wide*i);
        painter.drawLine(wide*i, 0, wide*i, (Size-1)*wide);
    }

    //绘制中心点
    brush.setColor(Qt::black);
    painter.setBrush(brush);
    painter.drawEllipse(QPoint(Size/2*wide,Size/2*wide),5,5);
    int a = 3, b = 11;
    painter.drawEllipse(QPoint(a*wide,a*wide),5,5);
    painter.drawEllipse(QPoint(a*wide,b*wide),5,5);
    painter.drawEllipse(QPoint(b*wide,a*wide),5,5);
    painter.drawEllipse(QPoint(b*wide,b*wide),5,5);

    //绘制提示预选点
    if (posX >= 0 && posY >= 0 && posX < Size && posY < Size)
    {
       brush.setColor(Qt::gray);
       painter.setBrush(brush);
       painter.drawEllipse(QPoint(posX * wide, posY * wide), 5, 5);
    }

    //绘制棋子
    for(int i = 0; i < Size; i++)
    {
        for(int j = 0; j < Size; j++)
        {
            if(game->mapState[i][j] == 0) continue;
            if(game->mapState[i][j] == 1){
                brush.setColor(Qt::black);
            }
            else if(game->mapState[i][j] == -1)
            {
                brush.setColor(Qt::white);
            }
            painter.setBrush(brush);
            painter.drawEllipse(QPoint(i*wide,j*wide),21,21);
        }
    }

    if(ConnOther && isSelect){
        if(game->player)
            brush.setColor(Qt::black);
        else
            brush.setColor(Qt::white);
        painter.setBrush(brush);
        painter.drawEllipse(QPoint(790,94),15,15);
    }
    if(ConnOther && !isSelect){
        if(game->player)
            brush.setColor(Qt::black);
        else
            brush.setColor(Qt::white);
        painter.setBrush(brush);
        painter.drawEllipse(QPoint(790,143),15,15);
    }

    //标记当前落子
    if(game->steps.isEmpty()) return;
    brush.setColor(Qt::red);
    painter.setBrush(brush);
    painter.drawEllipse(QPoint(game->steps.top().first*wide,game->steps.top().second*wide),6,6);

    //显示分数
//    for(auto i: game->available_score){
//        painter.setPen(Qt::red);
//        painter.setFont(QFont("Arial", 10));
//        painter.drawText(i[0] * wide, i[1] * wide, QString::number(i[2]));
//    }
}

void GameScene::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->x() - 40;
    int y = event->y() - 40;
    //qDebug() << "x = " << x << "y = " << y;

    if(x >= 0 && y >= 0 && x < (Size - 1) * wide && y < (Size-1) * wide)
    {
        // 获取光标左上角的点坐标
        int pX = x / wide * wide;
        int pY = y / wide * wide;
        //qDebug() << "x = " << pX << "y = " << pY;
        if(sqrt((x - pX) * (x - pX) + (y - pY) * (y - pY)) < 28)
        {
            posX = pX / wide;
            posY = pY / wide;
        }
        if(sqrt((x - pX - wide) * (x - pX - wide) + (y - pY) * (y - pY)) < 28)
        {
            posX = pX / wide + 1;
            posY = pY / wide;
        }
        if(sqrt((x - pX- wide) * (x - pX- wide) + (y - pY- wide) * (y - pY- wide)) < 28)
        {
            posX = pX / wide + 1;
            posY = pY / wide + 1;
        }
        if(sqrt((x - pX) * (x - pX) + (y - pY- wide) * (y - pY- wide)) < 28)
        {
            posX = pX / wide;
            posY = pY / wide + 1;
        }
        //qDebug() << "x = " << posX << "y = " << posY;

    }
    else posX = posY = -1;
    update();
}
//落子逻辑
void GameScene::mouseReleaseEvent(QMouseEvent *event)
{
    if(!(posX >= 0 && posY >= 0 && posX < Size && posY < Size && game->mapState[posX][posY] == 0))
        return;
    //人机对战人机则不能落子
    if(aa)return;
    if(!game->gameStates){
        if(P!=0)
            myDialog->showMsg("请准备");
        else
            myDialog->showMsg("请点击重新开始按钮");
        //QMessageBox::information(this, "提示", "请点击重新开始按钮");
        return;
    }
    if(P!=0&&(!ServePrepare || !ClientPrepare))
    {
        myDialog->showMsg("有人没准备");
        //QMessageBox::information(this, "提示", "有人没准备");
        return;
    }
    //人机对战
    if(game->isAi){
        if(game->player)
        {
            //人落子
            game->PdownChess(posX, posY);
            //qDebug() << "my score: " << game->calculateScore1(posX, posY, 1) + game->calculateScore1(posX, posY, -1);
            //人机落子
            connect(aiTimer, SIGNAL(timeout()), this, SLOT(AiDown()));
            aiTimer->start(aiTime);
            if(!game->gameStates)
            {
                gameOver();
                aiTimer->stop();
            }
            update();
        }
    }
    //联网模式
    else if(P!=0)
    {
        if(isSelect)
        {
            game->PdownChess(posX, posY);
            startTime();
            isSelect = false;
            //是客户端则发送落子信息给服务端
            if(P == 1)
                emit ServeSelect(posX, posY);
            //是服务端则发送落子信息给客户端
            else if(P == -1)
                emit ClientSelect(posX, posY);
            if(!game->gameStates)
            {
                gameOver();
                return;
            }
            update();
        }
        else
        {
            myDialog->showMsg("请等待对方落子");
            //QMessageBox::information(this, "提示", "请等待对方落子");
            //isSelect = !isSelect;
        }
    }
    //单机模式
    else {
        game->PdownChess(posX, posY);
        //startTime();
        //qDebug() << "my score: " << game->downScore(posX, posY);
        if(!game->gameStates)
        {
            gameOver();
            return;
        }
        Set_Asp();
        update();
    }
}

void GameScene::updateTimer()
{
    elapsedTime--; // 每次计时器超时，时间加一秒
    QString timeString = QString::number(elapsedTime) + "s"; // 将时间转换为字符串
    ui->times->setText(timeString); // 更新计时器控件的显示内容
    if (elapsedTime <= 0) {
        timer->stop(); // 停止倒计时器
        gameOver();
    }
}

void GameScene::aiVSai()
{
    initGame();
    ui->giveUpBtn->setText("暂停");
    aa = true;
    game->PdownChess(Size/2,Size/2);
    if(aiTimer2)
    {
        aiTimer2->stop();
        delete aiTimer2;
    }
    aiTimer2 = new QTimer;
    connect(aiTimer2,SIGNAL(timeout()),this,SLOT(AiDown()));   //连接槽函数
    aiTimer2->start(aiTime);   //设置ai思考时间
    if(!game->gameStates)
    {
        gameOver();
        aiTimer2->stop();
    }
    update();
}

void GameScene::AiDown()
{
    //qDebug() << "我进来了00";
    //qDebug() << "我进来了11";
    if(aa && game->gameStates)
    {
        game->aiChess = game->player?game->chess:-game->chess;
        game->aiMove();
        if(!game->gameStates)
        {
            gameOver();
        }
        update();
    }
    else if(!game->player && game->gameStates)
    {
        game->aiMove();
        if(!game->gameStates)
        {
            gameOver();
        }
        update();
    }
}

void GameScene::PDown()
{

}

void GameScene::on_backBtn_clicked()
{
    MainWindow *m = new MainWindow;
    if(P != 0 && ConnOther){
        if(QMessageBox::Yes ==
        QMessageBox::question(this,"question","返回将断开连接，是否继续？",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes))
        {
            emit SendExit();
        }
    }
    else
    {
        this->close();
        m->show();
        aiTimer->stop();
        aiTimer2->stop();
        this->deleteLater();
    }
}


void GameScene::on_restart_clicked()
{
    if(game->isAi)
        startAiVsPlayerGame();
    else if(aa)
        aiVSai();
    else if(P!=0 && ConnOther)
    {
        initGame();

        ui->restart->setText("已准备");
        ui->label_6->setText("已准备");
        repentNum = 2;
        ui->label_11->setText(QString::number(repentNum));
        ui->restart->setEnabled(false);
        emit SendReady();
    }
    else if(P == 0) initGame();
    update();
}



void GameScene::on_repentBtn_clicked()
{
    if(P != 0 && ConnOther && game->gameStates && game->steps.size() > 0)
    {
        if(repentNum > 0){
            emit SendRepent();
            QMessageBox::information(this, "提示", "等待对方同意");
        }
        else {
            myDialog->showMsg("悔棋次数已用完！");
        }
    }
    else repent();
}

void GameScene::on_giveUpBtn_clicked()
{
    if(game->gameStates)
    {
        if(aa)
        {
            if(pause){
                aiTimer2->stop();
                ui->giveUpBtn->setText("继续");
                pause = false;
            }
            else {
                aiTimer2->start(aiTime);
                ui->giveUpBtn->setText("暂停");
                pause = true;
            }
        }
        else if(P!=0)
            emit SendGiveup();
        else
            gameOver();
    }
}

void GameScene::startAiVsPlayerGame()
{
    // 初始化游戏
    initGame();
    game->isAi = true;
    // 人类先手
    game->player = true;
    Set_Asp();
    // AI棋子颜色为与人类相反
    game->aiChess = game->player ? -1 : 1;
    update();
}

void GameScene::tongbugsc(int x, int y)
{
    //qDebug()<<"wew"<<x<<y;
    game->PdownChess(x, y);
    if(!game->gameStates)
    {
        timer->stop();
        repaint();
        gameOver();
    }
    else startTime();
    update();
}

//悔棋
void GameScene::repent()
{
    if(!game->gameStates) ui->repentBtn->setText("回溯");
    int repentnum = 0; //悔棋的步数，如果是ai模式悔棋两次
    if(isSelect && P != 0)
    {
        repentnum = 1;
    }
    if(repOther) repentnum ^= 1;
    if(!game->gameStates) repentnum = 0;
    while(game->steps.size() > 0 && game->isAi + 1 + repentnum--)
    {
        //steps存的是每一步的坐标的栈
        int x = game->steps.top().first;
        int y = game->steps.top().second;
        //qDebug() <<game->isAi << " " << x << " " << y;
        game->steps.pop();
        game->mapState[x][y] = 0;
        isSelect = !isSelect;
        game->player = !game->player;
        game->availablePositions.insert({x, y});
    }
    game->update_Score();
    ui->label_11->setText(QString::number(repentNum));
    update();
}

void GameScene::showConnStatus()
{
    this->setWindowTitle("联机模式");
    ui->label->show();
    ui->label_4->show();
    ui->label_5->show();
    ui->label_6->show();
    ui->label_7->show();
    ui->label_8->show();
    ui->restart->setText("准备");
    ui->label_9->show();
    ui->label_10->show();
    ui->label_11->show();
    ui->times->show();
}

void GameScene::Set_Asp()
{
    if(ConnOther)
    {
         if(isSelect)
             ui->label_3->setText("黑棋");
         else
             ui->label_3->setText("白棋");
    }
    else if(game->player)
        ui->label_3->setText("黑棋");
    else
        ui->label_3->setText("白棋");
}

void GameScene::setReady()
{
    ui->label_8->setText("已准备");
}

void GameScene::setnReady()
{
    ui->label_8->setText("未准备");
}

void GameScene::set6nReady()
{
    ui->restart->setEnabled(true);
    ui->restart->setText("准备");
}

void GameScene::startTime()
{
    elapsedTime = 60;
    ui->times->setText("60s");
    timer->start(1000); // 每隔1秒触发一次倒计时器超时事件
}
