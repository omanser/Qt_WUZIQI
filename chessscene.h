#ifndef CHESSSCENE_H
#define CHESSSCENE_H

#include <QWidget>
#include <QPainter>

QT_BEGIN_NAMESPACE
namespace Ui { class ChessScene; }
QT_END_NAMESPACE

class ChessScene : public QWidget
{
    Q_OBJECT
public:
    explicit ChessScene(QWidget *parent = nullptr);
    ~ChessScene();
    void paintEvent(QPaintEvent *event);
signals:

private:
    Ui::ChessScene *ui;
};

#endif // CHESSSCENE_H
