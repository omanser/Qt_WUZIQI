#include "conngame.h"
#include "mainwindow.h"
#include "ui_conngame.h"
#include<QDebug>
#include <QMessageBox>
ConnGame::ConnGame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnGame)
{
    ui->setupUi(this);
    ui->widget_2->hide();
    gc = new GameClient(this);
    QPixmap pixmap(":/Image/black.png");
    this->setWindowIcon(pixmap);
    setWindowTitle("connGame");
    //game = new GameScene();
    connect(gc,&GameClient::c_close,[=](){
    // game->show();
     this->hide();
    });

}

ConnGame::~ConnGame()
{
    delete ui;
}

void ConnGame::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/Image/7.png");
    //设置背景大小
    painter.drawPixmap(0,0,this->width(),this->height(),pix);
}

void ConnGame::on_backBtn_clicked()
{
    if(select){
        ui->creatBtn->show();
        ui->widget_2->hide();
        select = false;
    }
    else{
        MainWindow *m = new MainWindow;
        this->hide();
        m->show();
    }
}

void ConnGame::on_creatBtn_clicked()
{
    gs = new GameServe(this);
    gs->setWindowTitle("联机模式");
    gs->p_connection();
    //game->show();
    this->hide();
}

void ConnGame::on_joinBtn_clicked()
{
    if(!select){
        ui->widget_2->show();
        ui->creatBtn->hide();
        select = true;
    }
    else{
        gc->ip=ui->ip->text();
        gc->p_connection();
    }
}
