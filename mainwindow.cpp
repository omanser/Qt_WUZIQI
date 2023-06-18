#include "mainwindow.h"
#include "conngame.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QString>
#include <QDebug>
#include <QToolTip>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("五子棋");
    QPixmap pixmap(":/Image/black.png");
    this->setWindowIcon(pixmap);
    //mypushbutton(ui->humanVsAiButton);
    // 在需要显示提示消息的地方，创建悬浮窗口实例并显示消息
    //PopupWindow *popupWindow = new PopupWindow(this);
    //popupWindow->showMessage("提示消息");
//    // 在任意窗口或控件上设置浮动提示
//    QToolTip::showText(this->mapToGlobal(this->rect().center()), "提示消息");
    //GameScene *g = new GameScene;
//    connect(ui->in, &QPushButton::clicked, g, [=](){
//        this->close();
//        g->show();
//    });
}

void MainWindow::sqlConnect()
{
    QStringList list = QSqlDatabase::drivers();
    qDebug()<< list;
    // 创建数据库实例
    QSqlDatabase db = QSqlDatabase::addDatabase("QOCI");
    // 设置连接信息
    db.setHostName("127.0.0.1");
    db.setDatabaseName("orcl");
    db.setUserName("scott");
    db.setPassword("tiger");
    // 连接数据库
    bool bl = db.open();
    if(!bl)
    {
        //qDebug() << "连接数据失败";
        QMessageBox::critical(this, QString::fromLocal8Bit("daa"), QString::fromLocal8Bit("fail!"), QString::fromLocal8Bit("no"));
    }
    else
    {
        qDebug() << "连接数据成功";
    }
    // 对数据操作使用sql语句
    // 创建一个操作sql语句的类
    QSqlQuery query;
    QString sql = "select * from users_info";
    bl = query.exec(sql);
    if(!bl)
    {
        qDebug() <<"查询失败...";
        return;
    }
    // 遍历结果
//    QString username = ui->lineEdit->text();
//    QString password = ui->lineEdit_2->text();
//    qDebug() << username << " " << password;
//    while(query.next())
//    {
//        if(username == query.value("username").toString()
//                && password == query.value("password").toString())
//        {
//            qDebug() << "登录成功";
//        }
        // 将当前记录的字段值取出
//        qDebug() << "用户名: " << query.value("username").toString()
//            << ", 密码: " << query.value("password").toString();
    //}
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::paintEvent(QPaintEvent *)
{
        QPainter painter(this);
        QPixmap pix;
        pix.load(":/Image/4.png");
        //设置背景大小
        painter.drawPixmap(0,0,this->width(),this->height(),pix);

}

void MainWindow::on_humanVsAiButton_clicked()
{
    GameScene* gameScene = new GameScene;
    gameScene->setWindowTitle("人机模式");
    gameScene->startAiVsPlayerGame();
    this->hide();
    gameScene->show();
}

void MainWindow::on_singlePlayerButton_clicked()
{
    GameScene* gameScene = new GameScene;
    gameScene->setWindowTitle("本机模式");
    gameScene->initGame();
    this->hide();
    gameScene->show();
}

void MainWindow::on_aiVai_clicked()
{
    GameScene* gameScene = new GameScene;
    gameScene->setWindowTitle("机机模式");
    gameScene->aiVSai();
    this->hide();
    gameScene->show();

}

void MainWindow::on_exitBtn_clicked()
{
    this->close();
    this->deleteLater();
}

void MainWindow::on_onlinePlayButton_clicked()
{
    this->hide();
    ConnGame *cg = new ConnGame;
    cg->show();
}


/*---------------------------------------------------------------*/
PopupWindow::PopupWindow(QWidget *parent)
    : QWidget(parent, Qt::ToolTip | Qt::FramelessWindowHint)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background-color: #000000; color: #ff0000; padding: 8px;");
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);

    label = new QLabel(this);
    label->setStyleSheet("background-color: black;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

void PopupWindow::showMessage(const QString &message)
{
    label->setText(message);
    adjustSize();
    show();
}

