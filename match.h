#ifndef MATCH_H
#define MATCH_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QStack>
#include <QPair>
#include <unordered_set>
#include <list>
#include<vector>
#include<cmath>

const int Size = 15;    //棋盘尺寸
const int wide = 48;

struct PairHash {
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U>& p) const {
        auto hash1 = std::hash<T>{}(p.first);
        auto hash2 = std::hash<U>{}(p.second);
        return hash1 ^ hash2;
    }
};

class match
{
public:
    match();
    std::unordered_set<std::pair<int, int>, PairHash> availablePositions;
    std::unordered_set<std::pair<int, int>, PairHash> temp;
    std::vector<std::vector<int>> available_score;
    int aiChess = -1;  // AI棋子颜色
    bool gameStates = false; //游戏是否在进行
    bool player = true;
    int chess = 1;   //棋子颜色
    int isAi = false; //是否为人机模式;
    int mapState[Size][Size];   //棋盘局面状态，0未下，1黑子，-1白子
    QStack<QPair<int, int> > steps;  //将每一步坐标存入栈
    void initMap();     //棋盘初始化为0
    void startGame();
    void PdownChess(int x, int y);   //人落子
    void AIdownChess(int x, int y);   //AI落子
    bool isWin(int x, int y);   //判断是否产生胜负
    bool isDraw();  //判断是否平局
    void aiMove();  // AI下棋
    //int calculateScore(int x, int y, int chess);  // 计算指定位置的得分
    void generatePossibleMoves(QList<QPair<int, int>>& moves);  // 生成可能的落子位置
    void updateAvailablePositions(int row, int col);
    int miniMax(int depth, int alpha, int beta, std::unordered_set<std::pair<int, int>, PairHash> tempAvailablePositions);
    int evaluate();
    int downScore(int x, int y);
    void makeMove();
    int calculateScore1(int row, int col, int chessColor);
    void update_Score();
private:
};

#endif // MATCH_H
