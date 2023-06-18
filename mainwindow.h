#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include "gamescene.h"
#include <QLabel>
#include<QPainter>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void sqlConnect();
    //绘图事件
    void paintEvent(QPaintEvent *);
private slots:

    void on_humanVsAiButton_clicked();

    void on_singlePlayerButton_clicked();

    void on_exitBtn_clicked();

    void on_onlinePlayButton_clicked();

    void on_aiVai_clicked();

private:
    Ui::MainWindow *ui;
};

// 创建自定义悬浮窗口类
class PopupWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PopupWindow(QWidget *parent = nullptr);
    void showMessage(const QString &message);

private:
    QLabel *label;
};

#endif // MAINWINDOW_H
