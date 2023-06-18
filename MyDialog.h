#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <QPushButton>
#include <QDialog>
#include <QTimer>
#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QDebug>
#include <QDialogButtonBox>

class MyDialog : public QDialog
{
    Q_OBJECT
public:
    QVBoxLayout *layout;
    // 创建 QPushButton
    QPushButton *button;
    // 创建 QLabel
    QLabel *label;
    explicit MyDialog(QWidget *parent = nullptr) : QDialog(parent)
    {
        setModal(true);
        setWindowTitle("提示");
        resize(200, 80);
        // 创建布局管理器
        layout = new QVBoxLayout(this);

        label = new QLabel("msg", this);
        label->setFont(QFont("华文行楷", 15));
        label->setAlignment(Qt::AlignHCenter);
        layout->addWidget(label); // 将 QLabel 添加到布局管理器中
        // 创建 QDialogButtonBox，指定按钮的类型
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Yes | QDialogButtonBox::No);

        // 连接按钮的信号和槽函数
        connect(buttonBox, &QDialogButtonBox::accepted, this, &MyDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, this, &MyDialog::reject);

        // 将按钮组合添加到布局中
        layout->addWidget(buttonBox);
    }
    void myClose()
    {
        QTimer *timer = new QTimer(this);
        // 设置定时器的超时时间
        int timeoutDuration = 5000; // 5秒
        timer->setSingleShot(true); // 设置为单次触发
        timer->setInterval(timeoutDuration);

        connect(timer, &QTimer::timeout, this, &MyDialog::close);

        timer->start();
    }
    void showMsg(QString msg)
    {
        //myClose();
        label->setText(msg);
        show();
    }
};

#endif // MYDIALOG_H
