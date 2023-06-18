#include <time.h>
#include <stdlib.h>
#include "match.h"
match::match()
{
    initMap();
}

void match::initMap()
{
    availablePositions.clear();
    for(int i = 0; i < Size; i++)
    {
        for(int j = 0; j < Size; j++)
            this->mapState[i][j] = 0;
    }
}

void match::startGame()
{
    gameStates = true;
    player = true;
    chess = 1;
    initMap();
    availablePositions.clear();
}

void updateAvailable_score(){

}

void match::PdownChess(int x, int y)
{
    //根据当前玩家判断落子颜色
    this->mapState[x][y] = player ? chess : -chess;

    //将落子位置存入steps栈中
    steps.push_back({x, y});
    // qDebug() << "client" <<this->mapState[x][y];

    //交换落子方
    player = !player;
    if(isWin(x, y) || isDraw()){
        gameStates = false;
    }

    //更新有效落子位置
    updateAvailablePositions(x, y);
}

void match::AIdownChess(int x, int y)
{
    mapState[x][y] = player ? chess : -chess;
    steps.push_back({x, y});
    player = !player;
}

//判断在（x, y）出落子后是否分出胜负
bool match::isWin(int x, int y)
{
    // 水平方向
    for (int i = 0; i < 5; i++)
    {
        //连续5个同色子则分出胜负
        if (y - i >= 0 && y - i + 4 < Size &&
            5 == abs(mapState[x][y - i] + mapState[x][y - i + 1]
                + mapState[x][y - i + 2] + mapState[x][y - i + 3] + mapState[x][y - i + 4]))
            return true;
    }

    // 竖直方向
    for (int i = 0; i < 5; i++)
    {
        if (x - i + 4 < Size && x - i >= 0 &&
            5 == abs(mapState[x - i][y] + mapState[x - i + 1][y] + mapState[x - i + 2][y] +
            mapState[x - i + 3][y] + mapState[x - i + 4][y]))
            return true;
    }

    // 主对角线方向
    for (int i = 0; i < 5; i++)
    {
        if (x - i + 4 < Size &&
            x - i >= 0 &&y - i >= 0 &&y - i + 4 < Size &&
            5 == abs(mapState[x - i][y - i] + mapState[x - i + 1][y - i + 1]  + mapState[x - i + 2][y - i + 2] +
             mapState[x - i + 3][y - i + 3]  + mapState[x - i + 4][y - i + 4]))
            return true;
    }

    // 反对角线方向
    for (int i = 0; i < 5; i++)
    {
        if (x + i < Size && x + i - 4 >= 0 && y - i >= 0 && y - i + 4 < Size &&
            5 == abs(mapState[x + i][y - i] + mapState[x + i - 1][y - i + 1] + mapState[x + i - 2][y - i + 2] +
                mapState[x + i - 3][y - i + 3]+ mapState[x + i - 4][y - i + 4]))
            return true;
    }

    return false;
}

//棋满平局
bool match::isDraw()
{
    for(int i = 0; i < Size; ++i)
        for(int j = 0; j < Size; ++j)
            if(mapState[i][j] == 0)
                return false;
    return true;
}

void match::aiMove()
{
    QList<QPair<int, int>> moves;
    // 计算每个落子位置的得分
    QList<int> scores;
    available_score.clear();
    //std::vector<std::vector<int>>
    for (const auto& move: availablePositions) {
        int x = move.first;
        int y = move.second;
        //qDebug() << x << " " << y;
        //mapState[x][y] = aiChess;
        int score = downScore(x, y);
        //mapState[x][y] = 0;
        std::vector<int> t;
        t.push_back(x); t.push_back(y); t.push_back(score);
        available_score.push_back(t);
        scores.append(score);
        moves.append({x, y});
    }

    // 找到最高分数

    int maxScore = -1e8;
    for (auto score: available_score) {
        if (score[2] > maxScore) {
            maxScore = score[2];
        }
    }
    QList<QPair<int, int>> bestMoves;
    // 找到所有最高分数对应的落子位置
    for (auto score: available_score) {
        //qDebug() << "Score: " << scores[i];
        if (score[2] == maxScore) {
            bestMoves.append({score[0],score[1]});
            //qDebug() << "maxScore11111" << maxScore;
        }
    }

    // 随机选择一个最高分数的落子位置
    int randomIndex = qrand() % bestMoves.size();
    int x = bestMoves[randomIndex].first;
    int y = bestMoves[randomIndex].second;

    //qDebug() << downScore(x,y);
    // 进行落子s
    //mapState[x][y] = player?chess:-chess;
    PdownChess(x, y);
    //aiChess=-aiChess;
}

//评分
int valueScore(int count, int space)
{
    int score = 0;
    if(count >= 5) score += 10000; //可以五连
    else if(count == 4)
    {
        if(space == 2)
            score += 980;
        if(space == 1)
            score += 520;
    }
    else if(count == 3 && space == 2) score += 380;
    else if(count == 3 && space == 1) score += 130;
    else if(count == 2 && space == 2) score += 120;
    return score;
}

int valuePScore(int count, int space)
{
    int score = 0;
    if(count >= 5) score += 6000; //可以五连
    else if(count == 4)
    {
        if(space == 2)
            score += 1200;
        if(space == 1)
            score += 666;
    }
    else if(count == 3 && space == 2) score += 380;
    else if(count == 3 && space == 1) score += 130;
    else if(count == 2 && space == 2) score += 120;
    return score;
}

int theValue(int x, int y, int role)
{
    if(role == -1){
        return valueScore(x, y);
    }
    else if(role == 1)
    {
        return valuePScore(x, y);
    }
    return 0;
}

int match::calculateScore1(int row, int col, int chessColor)
{
    int score = 0;
    int count = 0; //连续棋子
    int space = 0; //空白位置
    int space2 = 0;
    // 计算AI水平方向得分
    int j = 1, f = 2;
    int lose = 300;
    int fall_number = 0, count1 = 0, count2 = 0;
    bool hBlank = false;
    count1 = -1, count2 = -1;
    while(f--)
    {
        bool isBlank = false; //判断上一个位置是不是空格
        for (int i = 0; i <= 5; i++)
        {
            if(col - i * j < 0 || col - i * j >= Size || mapState[row][col - i * j] == -chessColor) {
                if(!isBlank && space) {space--;} //前一个位置是异色棋，空格不算
                break;
            }
            else if (mapState[row][col - i * j] == 0)
            {
                space++;
                isBlank = true;
                if(space == 2) { space--; break;}  //一边最多算一个空格
            }
            else if (mapState[row][col - i * j] == chessColor && i < 5)
            {
                count++;
                if(f) count1++;
                else count2++;
                if(isBlank) {
                    //score-=lose; //上一个位置是空格，需要减分
                    fall_number++; //降级指数
                }
                isBlank = false;
            }
        }
        if(f && fall_number) {
            hBlank = true;
        }
        else if(!f){
            //根据有效棋子数对mlose进行赋值，子越多降分越多
            int mlose = lose;
            if(count - 1 == 4) mlose = lose;
            else if(count - 1 == 3) mlose = lose / 4;
            else if(count - 1 <= 2)mlose = lose / 8;
            //只有一边有降分空格，棋子数多的一侧降分少
            if(hBlank && fall_number == 1){
                if(count2 > count1) score-=mlose/2;
                else score -= mlose;
            }
            if(!hBlank && fall_number == 1){
                if(count2 < count1) score-=mlose/2;
                else score -= mlose;
            }
            if(fall_number == 2) score-=2*mlose;
        }
        space2+=space; space = 0;
        j = -j;
    }
    if(mapState[row][col] == chessColor) count--; //中点计算了两次
    //两边都有降分空格，棋子数降档
    if(fall_number == 2) {
        count--;
        if(count >= 5) count = 4;
    }
    score += theValue(count, space2, chessColor);
    //qDebug() << "score1: " << score << " " << count << " " << space2;
    // 计算竖直方向得分
    count = space2 = fall_number = 0, f = 2, hBlank = false;
    count1 = -1, count2 = -1;
    while(f--)
    {
        bool isBlank = false; //判断上一个位置是不是空格
        for (int i = 0; i <= 5; i++)
        {
            if(row - i * j < 0 || row - i * j >= Size || mapState[row - i * j][col] == -chessColor)  {
                if(!isBlank && space) {space--;}
                break;
            }
            if (mapState[row - i * j][col] == 0)
            {
                space++;
                isBlank = true;
                if(space == 2) { space--; break;}
            }
            if (mapState[row - i * j][col] == chessColor && i < 5){
                count++;
                if(f) count1++;
                else count2++;
                if(isBlank) {
                    //score-=lose; //上一个位置是空格，需要减分
                    fall_number++; //降级指数
                }
                isBlank = false;
            }
        }
        if(f && fall_number) {
            hBlank = true;
        }
        else if(!f){
            int mlose = lose;
            if(count - 1 == 4) mlose = lose;
            else if(count - 1 == 3) mlose = lose / 4;
            else if(count - 1 <= 2)mlose = lose / 8;
            if(hBlank && fall_number == 1){
                if(count2 > count1) score-=mlose/2;
                else score -= mlose;
            }
            if(!hBlank && fall_number == 1){
                if(count2 < count1) score-=mlose/2;
                else score -= mlose;
            }
            if(fall_number == 2) score-=2*mlose;
        }
        space2+=space;
        space = 0;
        j = -j;
    }
    if(mapState[row][col] == chessColor) count--; //中点计算了两次
    if(fall_number == 2) {
        count--;
        if(count >= 5) count = 4;
    }
    score += theValue(count, space2, chessColor);
    //qDebug() << "score2: " << theValue(count, space2, chessColor) << " " << count << " " << space2;
    // 计算主对角线方向得分
    count = space2 = fall_number = 0, f = 2, hBlank = false;
    count1 = -1, count2 = -1;
    while(f--)
    {
        bool isBlank = false; //判断上一个位置是不是空格
        for (int i = 0; i <= 5; i++)
        {
            if(row - i * j < 0 || row - i * j >= Size || col - i * j < 0 || col - i * j >= Size || mapState[row - i * j][col - i * j] == -chessColor)  {
                if(!isBlank && space) {space--;}
                break;
            }
            if (mapState[row - i * j][col - i * j] == 0)
            {
                space++;
                isBlank = true;
                if(space == 2) { space--; break;}
            }
            if (mapState[row - i * j][col - i * j] == chessColor && i < 5){
                count++;
                if(f) count1++;
                else count2++;
                if(isBlank) {
                    //score-=lose; //上一个位置是空格，需要减分
                    fall_number++; //降级指数
                }
                isBlank = false;
            }
        }
        if(f && fall_number) {
            hBlank = true;
        }
        else if(!f){
            int mlose = lose;
            if(count - 1 == 4) mlose = lose;
            else if(count - 1 == 3) mlose = lose / 4;
            else if(count - 1 <= 2)mlose = lose / 8;
            if(hBlank && fall_number == 1){
                if(count2 > count1) score-=mlose/2;
                else score -= mlose;
            }
            if(!hBlank && fall_number == 1){
                if(count2 < count1) score-=mlose/2;
                else score -= mlose;
            }
            if(fall_number == 2) score-=2*mlose;
        }
        space2+=space;
        space = 0;
        j = -j;
    }
    if(mapState[row][col] == chessColor) count--; //中点计算了两次
    if(fall_number == 2) {
        count--;
        if(count >= 5) count = 4;
    }
    score += theValue(count, space2, chessColor);
    //qDebug() << "score3: " << theValue(count, space2, chessColor) << " " << count << " " << space2;
    // 计算反斜方向得分
    count = space2 = fall_number = 0, f = 2, hBlank = false;
    count1 = -1, count2 = -1;
    while(f--)
    {
        bool isBlank = false; //判断上一个位置是不是空格
        for (int i = 0; i <= 5; i++)
        {
            if(row - i * j < 0 || row - i * j >= Size || col + i * j < 0 || col + i * j >= Size || mapState[row - i * j][col + i * j] == -chessColor)  {
                if(!isBlank && space) {space--;}
                break;
            }
            if (mapState[row - i * j][col + i * j] == 0)
            {
                space++;
                isBlank = true;
                if(space == 2) { space--; break;}
            }
            if (mapState[row - i * j][col + i * j] == chessColor && i < 5){
                count++;
                if(f) count1++;
                else count2++;
                if(isBlank) {
                    //score-=lose; //上一个位置是空格，需要减分
                    fall_number++; //降级指数
                }
                isBlank = false;
            }
        }
        if(f && fall_number) {
            hBlank = true;
        }
        else if(!f){
            int mlose = lose;
            if(count - 1 == 4) mlose = lose;
            else if(count - 1 == 3) mlose = lose / 4;
            else if(count - 1 <= 2)mlose = lose / 8;
            if(hBlank && fall_number == 1){
                if(count2 > count1) score-=mlose/2;
                else score -= mlose;
            }
            if(!hBlank && fall_number == 1){
                if(count2 < count1) score-=mlose/2;
                else score -= mlose;
            }
            if(fall_number == 2) score-=2*mlose;
        }
        space2+=space;
        space = 0;
        j = -j;
    }
    if(mapState[row][col] == chessColor) count--; //中点计算了两次
    if(fall_number == 2) {
        count--;
        if(count >= 5) count = 4;
    }
    score += theValue(count, space2, chessColor);
    //qDebug() << "score4: " << theValue(count, space2, chessColor) << " " << count << " " << space2;
    return score;
}

void match::generatePossibleMoves(QList<QPair<int, int>>& moves)
{
    // 生成可能的落子位置
    // 将所有空闲位置添加到moves列表中
    for (int i = 0; i < Size; ++i) {
        for (int j = 0; j < Size; ++j) {
            if (mapState[i][j] == 0) {
                moves.append(QPair<int, int>(i, j));
            }
        }
    }
}

int match::evaluate()
{
    int score = 0;
    for(auto i: steps){
        int x = i.first, y = i.second;
        score += calculateScore1(x ,y , mapState[x][y]);
    }
    return score;
}

int match::downScore(int x, int y)
{
    int t = mapState[x][y];
    mapState[x][y] = aiChess;
    int score = calculateScore1(x, y, aiChess);
    mapState[x][y] = -aiChess;
    score += calculateScore1(x, y, -aiChess);
    mapState[x][y] = t;
    return score;
}

void match::update_Score(){
    available_score.clear();
    for(auto &i: availablePositions){
        std::vector<int> t;
        t.push_back(i.first);
        t.push_back(i.second);
        t.push_back(downScore(i.first,i.second));
        available_score.push_back(t);
    }
}

void score_erase(std::vector<std::vector<int>> &t, int x, int y){
    for(auto beg = t.begin(); beg != t.end(); beg++){
        if((*beg)[0] == x && (*beg)[1] == y){
            t.erase(beg);
            return;
        }
    }
}

// 获取落子位置周围的可落子位置
void match::updateAvailablePositions(int row, int col) {
    availablePositions.erase({row, col});
    // 检查落子位置周围的8个方向
    for (int dx = -2; dx <= 2; dx++) {
        for (int dy = -2; dy <= 2; dy++) {
            if (dx == 0 && dy == 0) {
                continue; // 忽略落子位置本身
            }

            int newRow = row + dx;
            int newCol = col + dy;

            // 检查新位置是否在棋盘范围内，并且该位置未被落子
            if (newRow >= 0 && newRow < Size && newCol >= 0 && newCol < Size && mapState[newRow][newCol] == 0) {
                availablePositions.insert(std::make_pair(newRow, newCol));
            }
        }
    }
    update_Score();
}

// 最大最小值搜索
int match::miniMax(int depth, int alpha, int beta, std::unordered_set<std::pair<int, int>, PairHash> tempAvailablePositions)
{
    if (depth == 1) return evaluate(); // 达到最大深度，返回评估分数
    std::unordered_set<std::pair<int, int>, PairHash> t = tempAvailablePositions;
    if (depth % 2 == 0) { // 最大层
        int max_value = -1e9;
        for (const auto& position : t) {
            int i = position.first;
            int j = position.second;
            int score;
            if(mapState[i][j]!=0) continue;
            mapState[i][j] = -1; // 假设落子在position位置
            steps.push({i, j});
            updateAvailablePositions(i, j);
            tempAvailablePositions = availablePositions;
            if(isWin(i, j)) {
                score = evaluate();
                max_value = std::max(max_value, score); // 更新最大值
                alpha = std::max(alpha, max_value); // 更新alpha值
                break;
            }
            score = miniMax(depth + 1, alpha, beta, tempAvailablePositions); // 递归搜索
            steps.pop();
            availablePositions = tempAvailablePositions;
            mapState[i][j] = 0; // 恢复棋盘

            max_value = std::max(max_value, score); // 更新最大值
            alpha = std::max(alpha, max_value); // 更新alpha值
            if (alpha >= beta) break; // alpha剪枝
        }
        return max_value;
    }
    else { // 最小层
        int min_value = 1e9;
        for (const auto& position : t) {
            int i = position.first;
            int j = position.second;
            if(mapState[i][j]!=0) continue;
            mapState[i][j] = 1; // 假设落子在position位置
            steps.push({i, j});
            updateAvailablePositions(i, j);
            tempAvailablePositions = availablePositions;
            int score;
            if(isWin(i, j)) {
                score = evaluate();
                min_value = std::min(min_value, score); // 更新最小值
                beta = std::min(beta, min_value); // 更新beta值
                break;
            }
            score = miniMax(depth + 1, alpha, beta, tempAvailablePositions); // 递归搜索
            steps.pop();
            availablePositions = tempAvailablePositions;
            mapState[i][j] = 0; // 恢复棋盘

            min_value = std::min(min_value, score); // 更新最小值
            beta = std::min(beta, min_value); // 更新beta值
            if (alpha >= beta) break; // beta剪枝
        }
        return min_value;
    }
}

void printAva(std::unordered_set<std::pair<int, int>, PairHash> t){
    for(auto i: t)
    {
        qDebug() << i.first << " " << i.second;
    }
}

void match::makeMove() {
    int maxScore = std::numeric_limits<int>::min(), f = 0;
    std::pair<int, int> bestMove;
    std::unordered_set<std::pair<int, int>, PairHash> t = availablePositions;
    for (const auto& position : t) {
        // 在临时棋盘上尝试落子
        int x = position.first, y = position.second;
        mapState[x][y] = -1;
        updateAvailablePositions(x, y);
        steps.push({x, y});
        std::unordered_set<std::pair<int, int>, PairHash> tempAvailablePositions = availablePositions;
        // 调用 MiniMax 函数进行搜索
        int score = miniMax(0, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), tempAvailablePositions);
        // 恢复临时棋盘
        steps.pop();
        availablePositions = tempAvailablePositions;
        mapState[position.first][position.second] = 0;

        // 更新最大得分和最佳位置
        if (score > maxScore) {
            maxScore = score;
            bestMove = position;
            f++;
        }
    }
    // 进行落子
    if(f)
    {
        int x = bestMove.first;
        int y = bestMove.second;
        mapState[x][y] = aiChess;
        PdownChess(x, y);
        if (isWin(x, y)) {
            gameStates = false;
        }
    }
    else qDebug() << "AI无输出";
}
