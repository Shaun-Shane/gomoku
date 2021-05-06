#define ONLINE_JUDGE
#define RELEASE
#ifdef RELEASE
#pragma GCC optimize(3, "Ofast,no-stack-protector,unroll-loops,fast-math,inline")
#endif
#include <bits/stdc++.h>
using namespace std;
// #include <Const.h>
#ifndef CONST
#define CONST
#include <bits/stdc++.h>
// 放置一些公用常量，不要格式化

#define debug(x) cerr << #x << " = " << x << endl

//#define ONLINE_JUDGE

// 为加快运行速度，用以下类型代替int(如果不超出范围)
typedef int_fast8_t Byte;
typedef uint_fast8_t UByte;
typedef long long LL;

const LL INF = 1E16;

// bestDropId 表示未设置
const int BEST_UNDEFINED = -1;

// 最大分支数
const int BRANCH_LIMIT = 32;

// 搜索深度默认为 6, 优化后再升级
const int SEARCH_DEPTH = 6;

// 各局面价值表，待完善
const int valueTable = {0};
// 棋盘大小
const int SIZE = 15;
// 方向增量
const int dr[] = {0, 1, 1, 1, 0, -1, -1, -1};
const int dc[] = {1, 1, 0, -1, -1, 1, 0, -1};

// 每个位置落子类型：
// -1 无子，0 白，1 黑，2 下标非法
const int UNPLACE = -1;
const int WHITE = 0;
const int BLACK = 1;
const int MAX = 2;
const int INVALID = -2;

// 棋子类型得分，参数可以调整
// 拆分远近活三，远中近活二
const LL LIVEFOURMARK = 2000000;
const LL SLEEPFOURMARK = 1000000;
const LL NEARLIVETHREEMARK = 102000;
const LL FARLIVETHREEMARK = 100000;
const LL SLEEPTHREEMARK = 50000;
const LL NEARLIVETWOMARK = 1200;
const LL MIDLIVETWOMARK = 1050;
const LL FARLIVETWOMARK = 1000;
const LL SLEEPTWOMARK = 500;
const LL ONEMARK = 1;

int SEARCHCNT[] = {0, 4, 4, 4, 6, 6, 9};
const LL MARKS[][2]={{3, 1}, {1000, 20}, {100000, 20000}, {10000000, 200000}};

#endif
// #include <Board.hpp>
#ifndef BOARD_H
#define BOARD_H

#include <bits/stdc++.h>

using namespace std;

struct Board {
    // 每个位置落子类型：-1 无子，0 白，1 黑
    int boardState[SIZE][SIZE];

    // 构造函数
    Board();
    // 输出棋盘
    void Show();
    // 落子
    void PlaceAt(int, int, int);
    // 随机落子
    tuple<int, int, bool> RandomPlace(int);
    // 贪心落子
    tuple<int, int, bool> GreedyPlace(int);
    // 取消落子
    void UnPlaceAt(int, int);
    // 检查是否为五子
    // 全局检查
    bool CheckFive(int);
    // 上次落子位置周围检查
    bool CheckFive(int, int, bool);
    // 更新每个位置得分
    void CalcValue();
    // 和当前点相对位置的格子值
    int RelativePosVal(int, int, int, int);
    // 判断当前点的得分情况, 详见函数处注释
    LL MarkOfPoint(int, int, int);
};

Board::Board() {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) {
            boardState[i][j] = -1;
        }
}

void Board::Show() {
    cout << "  ";
    for (int i = 0; i < SIZE; i++)
        cout << char(i < 10 ? (int)i + '0' : 'A' + (int)i - 10) << " ";
    cout << endl;
    for (int i = 0; i < SIZE; i++) {
        cout << char(i < 10 ? (int)i + '0' : 'A' + (int)i - 10) << "|";
        for (int j = 0; j < SIZE; j++) {
            if (boardState[i][j] == -1)
                cout << "* ";
            else
                cout << (int)boardState[i][j] << " ";
        }
        cout << endl;
    }
}

void Board::PlaceAt(int x, int y, int color) {
    if ((x >= 0 && x < SIZE) && (y >= 0 && y < SIZE))
        boardState[(int)x][(int)y] = color;

#ifndef ONLINE_JUDGE
    cout << "Chess Placed at: "
         << "(" << (int)x << ", " << (int)y << ")" << endl;
#endif
}

void Board::UnPlaceAt(int x, int y) { boardState[x][y] = -1; }

tuple<int, int, bool> Board::RandomPlace(int color) {
    vector<int> v;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++)
            if (boardState[i][j] == -1) v.push_back(i * 15 + j);
    }
    if (!v.size()) return {-1, -1, false};
    int id = v[rand() % v.size()];
    PlaceAt(id / 15, id % 15, color);
    return {id / 15, id % 15, true};
}

tuple<int, int, bool> Board::GreedyPlace(int color) {
    vector<int> v;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++)
            if (boardState[i][j] == UNPLACE) v.push_back(i * 15 + j);
    }
    if (!v.size()) return {-1, -1, false};
    // 鼓励在中心位置放子
    auto CentralMark = [](int x, int y) {
        return 14 - abs(x - 7) - abs(y - 7);
    };
    // 考虑放子对自己和对手的影响
    int maxMark = MarkOfPoint(v[0] / 15, v[0] % 15, color) +
                  CentralMark(v[0] / 15, v[0] % 15),
        maxPoint = v[0];
    for (size_t id = 1; id < v.size(); id++) {
        auto curMark = MarkOfPoint(v[id] / 15, v[id] % 15, color) +
                       CentralMark(v[id] / 15, v[id] % 15);
        // cout << "curMark: " << curMark << "x: " << v[id] / 15 << "y: " <<
        // v[id] % 15 << "\n";
        if (curMark > maxMark) {
            maxMark = curMark;
            maxPoint = v[id];
        }
    }
    // try to fail the oppoent to achieve more than live three
    int avoidOppoent = -1, oppoentMark = -1;
    for (size_t id = 0; id < v.size(); id++) {
        auto curMark = MarkOfPoint(v[id] / 15, v[id] % 15, !color) +
                       CentralMark(v[id] / 15, v[id] % 15);
        if (curMark > SLEEPTHREEMARK && curMark > oppoentMark) {
            avoidOppoent = v[id];
            oppoentMark = curMark;
        }
    }
    if (avoidOppoent != -1) {
        maxPoint = avoidOppoent;
    }
    PlaceAt(maxPoint / 15, maxPoint % 15, color);
    return {maxPoint / 15, maxPoint % 15, true};
}

bool Board::CheckFive(int color) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) {
            if (boardState[i][j] != color) continue;
            // 遍历每个位置
            for (int k = 0; k < 4; k++) {
                int ti = i, tj = j;
                // 遍历每个棋子
                for (int s = 1; s <= 4; s++) {
                    ti += dr[k];
                    tj += dc[k];
                    if (ti < 0 || ti >= SIZE || tj < 0 || tj >= SIZE) continue;
                    if (boardState[ti][tj] != color) break;
                    if (s == 4) return true;
                }
            }
        }
    return false;
}

bool Board::CheckFive(int i, int j, bool color) {
    for (int k = 0; k < 4; k++) {
        // 遍历每个棋子
        int cnt = 0;
        for (int s = 1; s <= 4; s++) {
            int ti = i + s * dr[k];
            int tj = j + s * dc[k];
            if (ti < 0 || ti >= SIZE || tj < 0 || tj >= SIZE) break;
            if (boardState[ti][tj] != color) break;
            cnt++;
        }
         for (int s = 1; s <= 4; s++) {
            int ti = i - s * dr[k];
            int tj = j - s * dc[k];
            if (ti < 0 || ti >= SIZE || tj < 0 || tj >= SIZE) break;
            if (boardState[ti][tj] != color) break;
            cnt++;
        }
        if(cnt >= 4) return true;
    }
    return false;
}

int Board::RelativePosVal(int curX, int curY, int direction, int offset) {
    int unitOffsetX[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int unitOffsetY[] = {1, 1, 0, -1, -1, -1, 0, 1};
    curX = curX + unitOffsetX[direction] * offset;
    curY = curY + unitOffsetY[direction] * offset;
    if (curX < 0 || curX >= SIZE || curY < 0 || curY >= SIZE) {
        return INVALID;
    }
    return boardState[curX][curY];
}

LL Board::MarkOfPoint(int curX, int curY, int playerColor) {
    LL total = 0;
    for (int i = 0; i < 4; i++) {
        int left = 0, right = 0;
        while (RelativePosVal(curX, curY, i, -left-1) == playerColor)
            left++;
        while (RelativePosVal(curX, curY, i, right+1) == playerColor)
            right++;
        int leftUnplace = RelativePosVal(curX, curY, i, -left-1) == UNPLACE, 
        rightUnplace = RelativePosVal(curX, curY, i, right+1) == UNPLACE;
        if (left + right >= 4) {
            return MARKS[3][0] * 10;
        }
        if (leftUnplace || rightUnplace) {
            total += MARKS[left + right][leftUnplace ^ rightUnplace];
        }
    }
    return total;
}

#endif
// #include <Position.hpp>
#ifndef POSITION_H

struct Position {
    int x, y;
    LL w;
};

bool operator<(const Position& lhs, const Position& rhs) {
    LL lhsw = 14 - abs(SIZE / 2 - lhs.x) - abs(SIZE / 2 - lhs.y) + lhs.w;
    LL rhsw = 14 - abs(SIZE / 2 - rhs.x) - abs(SIZE / 2 - rhs.y) + rhs.w;
    return (lhsw == rhsw)? ((lhs.x == rhs.x) ? (lhs.y < rhs.y) : (lhs.x < rhs.x)): (lhsw > rhsw);
}
#endif
// #include <Agent.hpp>
#ifndef AGENT_HPP
#define AGENT_HPP

#include <bits/stdc++.h>

#include "jsoncpp/json.h"
#define UNUSED(x, y) (x) += (y)

using namespace std;
struct Agent {
    // 默认后手
    int color = WHITE;
    //计时器
    // Timer* myTimer = nullptr;
    // 棋盘
    Board myBoard;
    // 最优落子
    int bestDropId = -1;
    // 地方上一次落子位置
    int lastDropId = -1;
    // 最高得分
    LL bestScore = -INF;
    // 落子位置和对应的权重, 用于在set中查找.
    // -1表示有棋子的点
    LL weight[2][225];
    // 按照权排序的落子位置集合
    set<Position> nextPos[3];
    // 白子和黑子的权值之和
    LL sumWeight[2];

    // 运行
    void Run();
    // 初始化局面
    void Init();
    // 判断AI是否为先手
    void DetermineBlack(const Json::Value &);
    // 局面估值
    LL Evaluate(int);
    // 更改一个点的颜色, 同时更新附近的9*9格子内的空闲点权值信息
    // 后续优化可以尝试减少更改的范围
    void Update(int x, int y, int color);
    // 局面预处理, 初始化nextPos和weight数组
    void Preplay();
    // 搜素
    LL MinMaxSearch(int, LL, LL, bool);
};

LL Agent::MinMaxSearch(int depth, LL alpha, LL beta, bool curColor) {
    // 对手五连应该直接返回 -INF
    if (lastDropId != -1 &&
        myBoard.CheckFive(lastDropId / 15, lastDropId % 15, curColor ^ 1))
        return -INF;
    // 层数用完，估值返回
    if (depth <= 0) return Evaluate(curColor);
    // 无子可走
    if(!nextPos[MAX].size()) return -INF;
    // 对所有可能局面进行搜索
    // set<Position> tmp;
    auto pos = nextPos[MAX].begin();
    // 部分拷贝
    // for(int i = 0; i < SEARCHCNT[depth] && pos != nextPos[MAX].end(); i++) {
    //     tmp.insert(*pos);
    //     pos++;
    // }
    // 全拷贝
    /*for(int i = 0; i < nextPos[MAX].size(); i++) {
        tmp.insert(*pos);
        pos++;
    }*/
    for(int i = 0; i < SEARCHCNT[depth] && pos != nextPos[MAX].end(); i++, pos++) {
        auto x = pos->x, y = pos->y;
        auto w = pos->w;
        Update(x, y, curColor);
        // 更改上一次落子位置
        int tmpId = lastDropId;
        lastDropId = x * 15 + y;
        // 继续搜索
        LL val = -MinMaxSearch(depth - 1, -beta, -alpha, !curColor);
        // 取消落子 更新得分
        Update(x, y, UNPLACE);
        pos = nextPos[MAX].find(Position{x, y, w});
        assert(pos != nextPos[MAX].end());
        // 恢复上一次落子位置
        lastDropId = tmpId;
        if (val >= beta) {
            if (depth == SEARCH_DEPTH) bestScore = val, bestDropId = x * 15 + y;
            return val;
        }
        if (val > alpha) alpha = val;  // bestDropId
        if (depth == SEARCH_DEPTH &&
            (val > bestScore || bestDropId == BEST_UNDEFINED))
            bestScore = val, bestDropId = x * 15 + y;
    }
    return alpha;
}

void Agent::Run() {
    Init();

#ifndef ONLINE_JUDGE
    myBoard.Show();
    while (true) {
        Preplay();
        lastDropId = bestDropId = -1;
        bestScore = -INF;
        int x, y;
        cout << "Your drop position: ";
        cin >> x >> y;
        Update(x, y, BLACK);
        if (myBoard.CheckFive(BLACK)) {
            myBoard.Show();
            cout << "you win" << endl;
            break;
        }
        myTimer = new Timer;
        myTimer->prepare(__LINE__);
        // auto [aix, aiy, ok] = myBoard.RandomPlace(color);
        // auto [aix, aiy, ok] = myBoard.GreedyPlace(color);
        MinMaxSearch(SEARCH_DEPTH, -INF, INF, WHITE);
        myTimer->getTimePass(__LINE__);
        cout << "Opponent: " << bestDropId / 15 << " " << bestDropId % 15 << endl;
        // 落子
        Update(bestDropId / 15, bestDropId % 15, WHITE);
        debug(nextPos[WHITE].size());
        debug(nextPos[BLACK].size());
        debug(nextPos[MAX].size());
        debug(bestScore);
        myBoard.Show();
        // if (!ok) {
        //    break;
        //}
        if (myBoard.CheckFive(WHITE)) {
            cout << "you lose" << endl;
            break;
        }
    }
#else
    // 读入JSON
    string str;
    getline(cin, str);
    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);
    // 分析自己收到的输入和自己过往的输出，并恢复状态
    int turnID = input["responses"].size();
    DetermineBlack(input);
    for (int i = 0; i < turnID; i++) {
        myBoard.PlaceAt(input["requests"][i]["x"].asInt(), input["requests"][i]["y"].asInt(), !color);
        myBoard.PlaceAt(input["responses"][i]["x"].asInt(), input["responses"][i]["y"].asInt(), color);
    }
    myBoard.PlaceAt(input["requests"][turnID]["x"].asInt(), input["requests"][turnID]["y"].asInt(), !color);
    Preplay();
    MinMaxSearch(SEARCH_DEPTH, -INF, INF, color);
    Json::Value ret;
    ret["response"]["x"] = bestDropId / 15;
    ret["response"]["y"] = bestDropId % 15;
    Json::FastWriter writer;
    cout << writer.write(ret) << endl;
#endif
}

void Agent::Init() {
    // 根据json输入恢复棋盘状态
    // 最后一对坐标为 (-1,-1) 则 color 设为 1，我方先手
    // 否则，更新棋盘状态，我方后手
    // 本地测试不需要恢复，直接跳过
}

void Agent::DetermineBlack(const Json::Value &input) {
    if (input["requests"][0u]["x"].asInt() == -1 && input["requests"][0u]["y"].asInt() == -1) {
        color = BLACK;
    } else {
        color = WHITE;
    }
}

void Agent::Preplay() {
    // 初始化weight和nextPos
    sumWeight[0] = sumWeight[1] = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            // 如果位置上已有棋子, 标记权重为-1
            // 否则计算权重, 并分别存放在weight和nextPos中
            if (myBoard.boardState[i][j] != UNPLACE) {
                weight[BLACK][i * 15 + j] = weight[WHITE][i * 15 + j] = -1;           
            } else {
                weight[BLACK][i * 15 + j] = myBoard.MarkOfPoint(i, j, BLACK);
                weight[WHITE][i * 15 + j] = myBoard.MarkOfPoint(i, j, WHITE);
                sumWeight[BLACK] += weight[BLACK][i * 15 + j];
                sumWeight[WHITE] += weight[WHITE][i * 15 + j];
                nextPos[MAX].insert(
                    Position{i, j, max(weight[WHITE][i * 15 + j], weight[BLACK][i * 15 + j])});
                nextPos[WHITE].insert(
                    Position{i, j, weight[WHITE][i * 15 + j]});
                nextPos[BLACK].insert(
                    Position{i, j, weight[BLACK][i * 15 + j]});
            }
        }
    }
}

void Agent::Update(int x, int y, int color) {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
        return;
    }
    // 检查是否均为或均不为未放置
    assert(!(weight[BLACK][x * 15 + y] == -1) ^ (weight[WHITE][x * 15 + y] == -1));
    // 原始为未放置=>清除点在weight和nextPos中的记录
    if (weight[BLACK][x * 15 + y] != -1 && weight[WHITE][x * 15 + y] != -1) {
#ifndef ONLINE_JUDGE
        assert(nextPos[MAX].count(
                   Position{x, y, max(weight[WHITE][x * 15 + y], weight[BLACK][x * 15 + y])}) == 1);
        assert(nextPos[WHITE].count(Position{x, y, weight[WHITE][x * 15 + y]}) == 1);
        assert(nextPos[BLACK].count(Position{x, y, weight[BLACK][x * 15 + y]}) == 1);
#endif
        nextPos[MAX].erase(Position{x, y, max(weight[WHITE][x * 15 + y], weight[BLACK][x * 15 + y])});
        nextPos[WHITE].erase(Position{x, y, weight[WHITE][x * 15 + y]});
        nextPos[BLACK].erase(Position{x, y, weight[BLACK][x * 15 + y]});
        sumWeight[WHITE] -= weight[WHITE][x * 15 + y];
        sumWeight[BLACK] -= weight[BLACK][x * 15 + y];
        weight[BLACK][x * 15 + y] = weight[WHITE][x * 15 + y] = -1;
    }
    myBoard.boardState[x][y] = color;
    // 更改为未放置=>增加点在weight和nextPos中的记录
    if (color == UNPLACE) {
        weight[BLACK][x * 15 + y] = myBoard.MarkOfPoint(x, y, BLACK);
        weight[WHITE][x * 15 + y] = myBoard.MarkOfPoint(x, y, WHITE);
        sumWeight[WHITE] += weight[WHITE][x * 15 + y];
        sumWeight[BLACK] += weight[BLACK][x * 15 + y];
        nextPos[MAX].insert(Position{x, y, max(weight[WHITE][x * 15 + y], weight[BLACK][x * 15 + y])});
        nextPos[WHITE].insert(Position{x, y, weight[WHITE][x * 15 + y]});
        nextPos[BLACK].insert(Position{x, y, weight[BLACK][x * 15 + y]});
    }
    // 修改完成后, 在9*9范围内修改空闲点的权值
    for (int i = max(0, x - 5); i < min(15, x + 6); i++) {
        for (int j = max(0, y - 5); j < min(15, y + 6); j++) {
            if ((i != x || j != y) && myBoard.boardState[i][j] == UNPLACE) {
// 删除现存权值记录
#ifndef ONLINE_JUDGE
                assert(nextPos[MAX].count(Position{
                           i, j, max(weight[WHITE][i * 15 + j], weight[BLACK][i * 15 + j])}) == 1);
                assert(nextPos[BLACK].count(Position{i, j, weight[BLACK][i * 15 + j]}) == 1);
                assert(nextPos[WHITE].count(Position{i, j, weight[WHITE][i * 15 + j]}) == 1);
#endif
                nextPos[MAX].erase(Position{i, j, max(weight[WHITE][i * 15 + j], weight[BLACK][i * 15 + j])});
                nextPos[WHITE].erase(Position{i, j, weight[WHITE][i * 15 + j]});
                nextPos[BLACK].erase(Position{i, j, weight[BLACK][i * 15 + j]});
                sumWeight[WHITE] -= weight[WHITE][i * 15 + j];
                sumWeight[BLACK] -= weight[BLACK][i * 15 + j];
                // 求出新权值记录并保存
                weight[BLACK][i * 15 + j] = myBoard.MarkOfPoint(i, j, BLACK);
                weight[WHITE][i * 15 + j] = myBoard.MarkOfPoint(i, j, WHITE);
                sumWeight[WHITE] += weight[WHITE][i * 15 + j];
                sumWeight[BLACK] += weight[BLACK][i * 15 + j];
                // 新增记录
                nextPos[MAX].insert(Position{i, j, max(weight[WHITE][i * 15 + j], weight[BLACK][i * 15 + j])});
                nextPos[WHITE].insert(Position{i, j, weight[WHITE][i * 15 + j]});
                nextPos[BLACK].insert(Position{i, j, weight[BLACK][i * 15 + j]});
            }
        }
    }
}

LL Agent::Evaluate(int color) {
    // LL tot = 0;
    // for (auto& pos : nextPos[color]) tot += pos.w;
    // assert(tot == sumWeight[color]);
    // for (auto& pos : nextPos[color ^ 1]) tot -= pos.w * 8 / 10;
    return sumWeight[color] - (sumWeight[color ^ 1] * 8 / 10);
    //return nextPos[color].begin()->w;
    // return tot;
}

#endif

int main() {
    ios::sync_with_stdio(false);
    auto ai = new Agent;
    ai->Run();
    delete ai;
    return 0;
}