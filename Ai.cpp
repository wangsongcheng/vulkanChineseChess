#include "Ai.h"
extern ImGuiInput g_ImGuiInput;
extern std::array<Player, MAX_COUNTRY_INDEX>g_Players;
void aiPlay(Ai *pAi){
    uint32_t dstRow, dstColumn;
    Chess *pSelect = pAi->GetSelect(pAi->GetCurrentCountry());
    const Chess *pTarget = pAi->GetTarget(pSelect, &dstRow, &dstColumn);
    pAi->SelectChess(pSelect);
    if(pAi->IsOnline()){
        //ai发送的pSelect没被正确解析
        if(pTarget){
            pAi->SendPlayChessMessage(g_Players[pAi->GetCurrentCountry()], pSelect, pTarget);
        }
        else{
            Chess target;
            target.SetPos(dstRow, dstColumn);
            target.SetCountry(MAX_COUNTRY_INDEX);
            pAi->SendPlayChessMessage(g_Players[pAi->GetCurrentCountry()], pSelect, &target);
        }
    }
    else{
        pAi->PlayChess(pSelect, dstRow, dstColumn);
    }
    pAi->UnSelectChess();
    //下完棋要调用下面的函数
    pAi->EnableNextCountry(g_ImGuiInput.enableAutoPlay);
}
void *AiPlayChess(void *userData){
    Ai *pAi = (Ai *)userData;
#ifdef __linux
    if(pAi->IsOnline()){
        // auto aiClientIndex = pAi->GetAiClientIndex();
        for (auto&it:g_Players){
            if(it.ai && it.uCountry == pAi->GetCurrentCountry()){
                pAi->Enable();
                break;
            }
        }
    }
    else if(pAi->GetCurrentCountry() != pAi->GetPlayer()){
        pAi->Enable();
    }
#endif
    printf("function %s start\n", __FUNCTION__);
    while(!pAi->GameOver() && !pAi->IsEnd()){
        pAi->Wait();
        // if(pAi->IsPause()){
        //     printf("function %s pause\n", __FUNCTION__);
        //    pAi->Wait();
        // }
        aiPlay(pAi);
        // if(!pAi->IsOnline()){
        //     pAi->NextCountry();
        //     // printf("current country:%d\n", pAi->GetCurrentCountry());
        //     // pAi->EnableNextCountry(false);
        //     pAi->EnableNextCountry(true);
        // }
    }
    printf("function %s end\n", __FUNCTION__);
    return nullptr;
}
// bool CanUseCountry(uint32_t countryIndex, const char *country){
//     bool bCanUse = true;
//     uint32_t playerIndex = 0;
//     for (auto it = g_Players.begin(); it != g_Players.end(); ++it, ++playerIndex){
//         if(countryIndex != playerIndex && !strcmp(it->country, country)){
//             bCanUse = false;
//             break;
//         }
//     }
//     return bCanUse;
// }
// void Swap(uint32_t *src, uint32_t *dst){
//     uint32_t temp = *src;
//     *src = *dst;
//     *dst = temp;
// }
/*
编写象棋游戏的AI涉及多个关键步骤，包括棋盘表示、走法生成、评估函数和搜索算法。以下是一个分步指南和示例代码片段：

1. 棋盘表示
推荐使用二维数组或位棋盘表示棋盘状态：

```cpp
enum Piece {
    EMPTY, 
    RED_KING, RED_ROOK, RED_KNIGHT, // 红方棋子
    BLACK_KING, BLACK_ROOK, // 黑方棋子
    // 其他棋子...
};

struct ChessBoard {
    Piece board[10][9]; // 10行9列象棋棋盘
    bool isRedTurn;     // 当前执棋方
    // 其他状态信息...
};
```

2. 走法生成
实现合法走法生成器：

```cpp
struct Move {
    int fromX, fromY;
    int toX, toY;
};

vector<Move> generateMoves(const ChessBoard& board) {
    vector<Move> moves;
    
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 9; ++x) {
            Piece p = board.board[y][x];
            if (p == EMPTY) continue;
            if (board.isRedTurn != isRedPiece(p)) continue;

            switch (getPieceType(p)) {
                case KING:
                    generateKingMoves(x, y, board, moves);
                    break;
                case ROOK:
                    generateRookMoves(x, y, board, moves);
                    break;
                // 其他棋子类型...
            }
        }
    }
    return moves;
}
```

3. 评估函数
包含子力价值和位置价值评估：

```cpp
int evaluate(const ChessBoard& board) {
    int score = 0;
    
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 9; ++x) {
            Piece p = board.board[y][x];
            if (p == EMPTY) continue;
            
            int value = getPieceValue(p);
            int positionBonus = getPositionScore(p, x, y);
            
            if (isRedPiece(p)) {
                score += value + positionBonus;
            } else {
                score -= value + positionBonus;
            }
        }
    }
    return board.isRedTurn ? score : -score;
}
```

4. 搜索算法（带Alpha-Beta剪枝）
```cpp
int alphaBeta(ChessBoard& board, int depth, int alpha, int beta) {
    if (depth == 0 || isGameOver(board)) {
        return evaluate(board);
    }

    vector<Move> moves = generateMoves(board);
    sortMoves(moves); // 启发式排序提升剪枝效率

    for (const Move& move : moves) {
        makeMove(board, move);
        int score = -alphaBeta(board, depth-1, -beta, -alpha);
        unmakeMove(board, move);

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

Move findBestMove(ChessBoard& board, int depth) {
    Move bestMove;
    int maxScore = INT_MIN;
    
    vector<Move> moves = generateMoves(board);
    for (const Move& move : moves) {
        makeMove(board, move);
        int score = -alphaBeta(board, depth-1, INT_MIN, INT_MAX);
        unmakeMove(board, move);
        
        if (score > maxScore) {
            maxScore = score;
            bestMove = move;
        }
    }
    return bestMove;
}
```

5. 优化技巧

- 迭代加深搜索：逐步增加搜索深度
```cpp
Move iterativeDeepening(ChessBoard& board, int maxDepth) {
    Move bestMove;
    for (int depth = 1; depth <= maxDepth; ++depth) {
        bestMove = findBestMove(board, depth);
        // 可添加时间控制逻辑
    }
    return bestMove;
}
```

- 置换表缓存搜索结果
- 历史启发式排序走法
- 开局库和残局数据库

6. 关键技术点

- 合法性检查：避免自杀移动和无效走法
- 将军检测：通过生成所有对方走法判断是否被将军
- 重复局面检测：使用Zobrist哈希记录局面
- 时间管理：控制搜索时间

建议开发顺序：
1. 实现棋盘表示和走法生成
2. 开发简单的评估函数
3. 实现Minimax算法
4. 添加Alpha-Beta剪枝
5. 逐步加入优化技术

完整实现需要处理中国象棋的特殊规则：
- 将帅不能照面
- 象不能过河
- 马蹩腿规则
- 炮的吃子规则
- 兵过河后的移动变化

可通过UCI协议实现AI与图形界面的对接：
```cpp
void uciCommunication() {
    string command;
    while (getline(cin, command)) {
        if (command == "uci") {
            cout << "id name ChessAI\n";
            cout << "uciok\n";
        }
        else if (command.starts_with("position")) {
            // 处理局面设置
        }
        else if (command == "isready") {
            cout << "readyok\n";
        }
        else if (command.starts_with("go")) {
            Move best = findBestMove(currentBoard, 5);
            cout << "bestmove " << moveToNotation(best) << endl;
        }
    }
}
```

建议进一步研究：
- 蒙特卡洛树搜索（MCTS）
- 神经网络评估函数
- 并行搜索算法
- 棋谱学习系统

开发象棋AI需要平衡搜索深度与评估准确性，建议从简单版本开始逐步优化。
*/
// //返回第一个满足条件的敌人
// const ChessInfo *GetRival(const std::vector<ChessInfo>&canplays, auto condition){
//     const ChessInfo *pRival = nullptr;
//     for (size_t i = 0; i < canplays.size(); ++i){
//         pRival = g_Chessboard.GetChessInfos(canplays[i].row, canplays[i].column);
//         if(pRival && condition(pRival)){
//             break;
//         }
//         else{
//             pRival = nullptr;
//         }
//     }
//     return pRival;
// }

// //返回rivalCountry中能吃到country的chessIndex棋子
// const ChessInfo *GetRival(uint32_t country, uint32_t rivalCountry, uint32_t chessIndex){
//     const ChessInfo *pChessInfo = nullptr;
//     for (size_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
//         const ChessInfo *pRival = nullptr;
//         pChessInfo = g_Chessboard.GetChessInfo(rivalCountry, uiChess);
//         if(pChessInfo->row <= CHESSBOARD_ROW && pChessInfo->column <= CHESSBOARD_COLUMN){
//             g_Chessboard.SetSelected(pChessInfo);
//             g_Chessboard.Select(VK_NULL_HANDLE);
//             auto canPlayInfo = g_Chessboard.GetCanPlay();
//             pRival = GetRival(canPlayInfo, [country, chessIndex](const ChessInfo *r){return r->country == country && r->chess == chessIndex;});
//             g_Chessboard.ClearCanPlay(VK_NULL_HANDLE);
//         }
//         if(pRival){
//             break;
//         }
//         else{
//             pChessInfo = nullptr;
//         }
//     }
//     return pChessInfo;
// }
// //返回所有国家中, 能吃到country的chessIndex的棋子
// const ChessInfo *GetRival(uint32_t country, uint32_t chessIndex){
//     const ChessInfo *pChessInfo = nullptr;
//     for (size_t i = 0; i < g_DefaultCountryCount; i++){
//         pChessInfo = GetRival(country, i, chessIndex);
//         if(pChessInfo)break;
//     }
//     return pChessInfo;
// }
// //返回country的第一个敌人
// const ChessInfo *GetRival(uint32_t country, const ChessInfo **pChessInfo){
//     const Chess *pRival = nullptr;
//     for (size_t uiChess = 0; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
//         *pChessInfo = g_Chessboard.GetChess(country, uiChess);
//         if(*pChessInfo && (*pChessInfo)->row <= CHESSBOARD_ROW && (*pChessInfo)->column <= CHESSBOARD_COLUMN){
//             g_Chessboard.SetSelected(*pChessInfo);
//             g_Chessboard.Select(VK_NULL_HANDLE);
//             auto canPlayInfo = g_Chessboard.GetCanPlay();
//             pRival = GetRival(canPlayInfo, [](const ChessInfo *){return true;});
//             g_Chessboard.ClearCanPlay(VK_NULL_HANDLE);
//             if(pRival)break;
//         }
//     }
//     return pRival;
// }
// //返回cCountry的chessIndex是否能被nextCountry棋子吃;
// bool IsDangerous(uint32_t cCountry, uint32_t nextCountry, uint32_t chessIndex){
//     return GetRival(cCountry, nextCountry, chessIndex);
// }
// // //返回所有国家中, 是否有棋子能吃到country的chessIndex
// // bool IsDangerous(uint32_t country, uint32_t chessIndex){
// //     bool bDangerous = false;
// //     for (size_t i = 0; i < g_DefaultCountryCount; ++i){
// //         if(country != i){
// //             bDangerous = IsDangerous(country, i, chessIndex);
// //             if(bDangerous)break;
// //         }
// //     }
// //     return bDangerous;
// // }
// void aiPlayChess(const ChessInfo*pInfo, const ChessInfo *pRival){
//     if(!g_Chessboard.IsBoundary(pInfo->row, pInfo->column)){
//         glm::vec2 pos = glm::vec2(COLUMN_TO_X(pInfo->column), ROW_TO_Y(pInfo->row));
//         // glm::vec2 pos = glm::vec2(COLUMN_TO_X(pInfo->column) - CHESSBOARD_GRID_SIZE * .5, ROW_TO_Y(pInfo->row) - CHESSBOARD_GRID_SIZE * .5);
//         // const ChessInfo *pSelected = g_Chessboard.GetSelected();
//         const ChessInfo *pChessInfo = g_Chessboard.GetSelectInfo(pos);
//         if(pChessInfo){
//             g_Chessboard.UnSelect(g_VulkanDevice.device);
//             if(pChessInfo->country == g_CurrentCountry){
//                 g_Chessboard.SetSelected(pChessInfo);
//             }
//             const ChessInfo *pSelected = nullptr;
//             auto canPlayInfo = g_Chessboard.GetCanPlay();
//             // if(canPlayInfo.empty()){
//             //     printf("canplay is empty; select row:%d, select column:%d\n", pChessInfo->row, pChessInfo->column);
//             // }
//             if(CanPlay(canPlayInfo)){
//                 if(pRival){
//                     pos = glm::vec2(COLUMN_TO_X(pRival->column), ROW_TO_Y(pRival->row));
//                     Play(pos, pRival);
//                 }
//                 else{
//                     ChessInfo canplay = canPlayInfo[rand() % canPlayInfo.size()];
//                     pos = glm::vec2(COLUMN_TO_X(canplay.column), ROW_TO_Y(canplay.row));
//                     pSelected = g_Chessboard.GetSelectInfo(pos);
//                     Play(pos, pSelected);
//                 }
//                 g_UpdateScreen = true;
//             }
//             g_Chessboard.ClearCanPlay(g_VulkanDevice.device);
//         }
//     }
// }
// /*
//     将
//         面临被吃的危险
//             1,有其他棋子(包括自身)可以解则解
//             2.没路走就走其他棋子,不然要死循环了
//             3.没路、没其他棋走则直接跳过。反正这种情况少见
//     其他棋子:
//         可以先找敌人
//         找不到的话则说明没有保护的棋子
//         面临被吃的危险
//             1.有其他棋子(包括自身)可以解则解
//             2.没有的话如果有保护的棋子，也可以不走
//             3.都没有就而且没路的话就参考上面
//     //除将外，其他棋子均可以在有保护的情况下不走
// */
// void Dangerous(const ChessInfo **pDangerous, const ChessInfo **pRival){
//     if((*pDangerous)->chess == Chess::Type::Shi_Chess_1 || (*pDangerous)->chess == Chess::Type::Shi_Chess_2){
//         *pDangerous = nullptr;
//         *pRival = nullptr;
//         return;
//     }
//     // g_Chessboard.SetSelected(*pDangerous);
//     // g_Chessboard.Select(VK_NULL_HANDLE);
//     //找一个解围的棋子
//     const ChessInfo *pChess = GetRival((*pRival)->country, (*pDangerous)->country, (*pRival)->chess);
//     if(pChess){
//         *pDangerous = pChess;
//     }
//     else{
//         if((*pDangerous)->chess != Chess::Type::Jiang_Chess){
//             //找一个可以保护pDangerous的棋子
//             pChess = GetRival((*pDangerous)->country, (*pDangerous)->country, (*pDangerous)->chess);
//             if(pChess){
//                 //如果找到就别管了
//                 *pDangerous = nullptr;
//                 *pRival = nullptr;
//                 return;
//             }
//         }
//         if(!CanPlay(g_Chessboard.GetCanPlay())){
//             *pDangerous = nullptr;//其实应该重新设置
//             *pRival = nullptr;
//         }
//     }   
//     // g_Chessboard.ClearCanPlay(VK_NULL_HANDLE);
// }
int32_t Ai::CanPlay(uint32_t country, const std::vector<glm::vec2>&canplays)const{
    int32_t index = 0;
    auto pBoard = mGame->GetChessboard();
    for (auto&it:canplays){
        if(Check(country, it.y, it.x))continue;
        const Chess *pInfo = pBoard->GetChess(it.y, it.x);
        if(!pInfo || pInfo->GetCountry() != country){
            return index;
        }
        ++index;
    }
    return -1;
}
Ai::Ai(/* args */){
#ifdef __linux__
    sem_init(&mAiSemaphore, 0, 0);
#endif  
#ifdef WIN32
    mAiSemaphore = CreateEvent(NULL, FALSE, FALSE, "AiSemaphore");
#endif
}

Ai::~Ai(){
#ifdef __linux__
    sem_destroy(&mAiSemaphore);
#endif
#ifdef WIN32
    ResetEvent(mAiSemaphore);
#endif
}
void Ai::Wait(){
#ifdef __linux__
    sem_wait(&mAiSemaphore);
#endif
#ifdef WIN32
    WaitForSingleObject(mAiSemaphore, INFINITE);
#endif // WIN32
}
void Ai::Enable(){
#ifdef __linux__
    sem_post(&mAiSemaphore);
#endif
#ifdef WIN32
    SetEvent(mAiSemaphore);
#endif // WIN32
}
void Ai::EnableNextCountry(bool autoPlay){
    if(mGame && mOnline){
        const uint32_t currentCountry = GetCurrentCountry();
        if(IsOnline() && IsServer()){
            for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
                if(!IsEnd() && it->ai && it->uCountry == currentCountry){
                    Enable();
                    break;
                }
            }
        }
        else{
            if(autoPlay || (!IsEnd() && currentCountry != GetPlayer())){
                Enable();
            }
        }
    }
}
void Ai::CreatePthread(Game *pGame, OnLine *pOnline){
    if(!mEnd)return;
    mEnd = false;
    // mPause = false;
    mGame = pGame;
    mOnline = pOnline;
#ifdef WIN32
    DWORD  pthreadId;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AiPlayChess, this, 0, &pthreadId);
#endif
#ifdef __linux
    pthread_t pthreadId;
    pthread_create(&pthreadId, nullptr, AiPlayChess, this);
#endif
}
Chess *Ai::GetTarget(const Chess *pSelect, const std::vector<glm::vec2>&canplays)const{
    Chess *pTarget = nullptr;
    auto pBoard = mGame->GetChessboard();
    for (const auto&it:canplays){
        pTarget = pBoard->GetChess(it.y, it.x);
        if(pTarget && pTarget->GetCountry() != pSelect->GetCountry())break;
    }
    return pTarget;
}

Chess *Ai::RandChess(uint32_t country) const{
    Chess *pSelect = nullptr;
    std::vector<glm::vec2>canplays;
    auto pBoard = mGame->GetChessboard();
    auto pChess = pBoard->GetChess(country);
    do{
        pSelect = pChess[rand() % DRAW_CHESS_COUNT];
        if(pSelect){
            canplays.clear();
            pSelect->Select(mGame->GetChessboard(), canplays);
            mGame->RemoveInvalidTarget(pSelect, canplays);
        }
    }while(!pSelect || -1 == CanPlay(country, canplays));
    return pSelect;
}

Chess *Ai::RandTarget(uint32_t country, const std::vector<glm::vec2> &canplays, uint32_t *row, uint32_t *column) const{
    int32_t index = 0;
    Chess *pTarget = nullptr;
    auto pBoard = mGame->GetChessboard();
    do{
        index = rand() % canplays.size();
        pTarget = pBoard->GetChess(canplays[index].y, canplays[index].x);
        if(canplays.size() <= 1)break;
    } while (Check(country, canplays[index].y, canplays[index].x));
    *row = canplays[index].y;
    *column = canplays[index].x;
    //临时测试
    if(Check(country, canplays[index].y, canplays[index].x)){
        printf("country = %d, row = %f, column = %f\n", country, canplays[index].y, canplays[index].x);
    }
    return pTarget;
}
Chess *Ai::GetSelect(uint32_t country, const Chess *pTarget)const{
    auto pBoard = mGame->GetChessboard();
    auto pAllChess = pBoard->GetChess(country);
    for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; uiChess++){
        Chess *pChess = pAllChess[uiChess];
        if(pChess){
            std::vector<glm::vec2>canplays;
            pChess->Select(pBoard, canplays);
            for (auto&it:canplays){
                if(it.x == pTarget->GetColumn() && it.y == pTarget->GetRow()){
                    return pChess;
                }
            }
        }
    }
    return nullptr;
}
//目前，该函数无法判断吃子后是否能被吃等类似情况
// 判断是否有国家一下步会走到row, column的地方
bool Ai::Check(uint32_t country, uint32_t row, uint32_t column) const{
    const Chessboard *pBoard = mGame->GetChessboard();
    for (size_t i = 0; i < mGame->GetCountryCount(); ++i){
    //得先模拟走后的结果，例如这个问题, 炮吃马本来应该会被旁边的车吃，但现在的马还没有被吃，所以车肯定走不到马的位置
        if(country != i){
            const Chess *pChess = pBoard->Check(i, row, column);
            //炮目前没法判断, 先跳过
            if(pChess && pChess->GetChess() != Chess::Type::Pao_Chess){
                return true;
            }
        }
    }
    return false;
}
const Chess *Ai::Check(uint32_t country) const{
    const Chess *pChess = nullptr;
    auto pBoard = mGame->GetChessboard();
    for (size_t i = 0; i < mGame->GetCountryCount(); ++i){
        if(i != country){
            const Chess *pJiang = pBoard->GetChess(country)[Chess::Type::Jiang_Chess];
            if(!pJiang)break;
            pChess = pBoard->Check(i, pJiang->GetRow(), pJiang->GetColumn());
            if(pChess){
                break;
            }    
        }
    }
    return pChess;
}
Chess *Ai::GetSelect(uint32_t country)const{
    //计谋篇
    //驱虎吞狼:强制和棋多的一方结盟
    //二虎竞食:强制和棋少的一方结盟
    //离间之计:强制破坏另外两方盟约
    Chess *pSelect = nullptr;
    std::vector<glm::vec2>canplays;
    auto pBoard = mGame->GetChessboard();
    const Chess *pCheck = Check(country);
    auto pCountryChess = pBoard->GetChess(country);
    //优先走能被吃的
    //还需要考虑该棋子走开后，将会不会被吃掉....不过...如果不自己搞一个chessboard类模拟的话，似乎没法判断啊
    if(pCheck){
        //被将了。优先把将的棋子吃掉，不行则走将
        pSelect = GetSelect(country, pCheck);
        if(!pSelect){
            // if(pCheck->GetChess() == PAO_CHESS_INDEX){
            //     //找出炮是通过哪个棋子吃将的
            // }
            // //马不能通过蹩马脚解将
            // else if(pCheck->GetChess() != MA_CHESS_INDEX){
            //     //找一颗能挡住的棋子

            // }
            Chess *pShi = pBoard->GetChess(country)[Chess::Type::Shi_Chess];
            if(!pShi){
                pShi = pBoard->GetChess(country)[Chess::Type::Shi_Chess + 1];
            }
            if(!pShi){
                pShi = pBoard->GetChess(country)[Chess::Type::Xiang_Chess];
            }
            if(!pShi){
                pShi = pBoard->GetChess(country)[Chess::Type::Xiang_Chess + 1];
            }
            if(pShi){
                std::vector<glm::vec2>cp;
                pShi->Select(pBoard, cp);
                if(!cp.empty())pSelect = pShi;
            }
        }
        if(!pSelect){
            Chess *pJiang = pBoard->GetChess(country)[Chess::Type::Jiang_Chess];
            pJiang->Select(pBoard, canplays);
            for (auto&it:canplays){
                //虽然不能检查吃棋后是否被吃，但能检查走的地方是否被吃
                if(!Check(country, it.y, it.x)){
                    pSelect = pJiang;
                    break;
                }
            }
        }
    }
    if(!pSelect){
        for (size_t i = 0; i < DRAW_CHESS_COUNT; ++i){
            Chess *pChess = pCountryChess[i];
            if(pChess){
                canplays.clear();
                pChess->Select(pBoard, canplays);
                const Chess *pTarget = GetTarget(pChess, canplays);
                if(pTarget){
                    pSelect = pChess;
                    break;
                }
            }
        }    
    }
    if(!pSelect){
        for (size_t i = 0; i < DRAW_CHESS_COUNT; ++i){
            Chess *pChess = pCountryChess[i];
            if(pChess && Check(country, pChess->GetRow(), pChess->GetColumn())){
                canplays.clear();
                pChess->Select(pBoard, canplays);
                if(!canplays.empty()){
                    pSelect = pChess;
                    break;
                }    
            }
        }
    }
    if(!pSelect){
        pSelect = RandChess(country);
    }
    return pSelect;
}
const Chess *Ai::GetTarget(const Chess *pSelect, uint32_t *row, uint32_t *column) const{
    const Chess *pTarget = nullptr;
    std::vector<glm::vec2>canplays;
    auto pBoard = mGame->GetChessboard();
    const uint32_t country = pSelect->GetCountry();
    if(pSelect->GetChess() == Chess::Type::Jiang_Chess){
        //如果走的是将，那么极有可能是因为被将了，并且必须通过走将解将
        //即使不是因为被将才走，也需要检查走后是否被吃
        Chess *pJiang = pBoard->GetChess(country)[Chess::Type::Jiang_Chess];
        pJiang->Select(pBoard, canplays);
        for (auto&it:canplays){
            //虽然不能检查吃棋后是否被吃，但能检查走的地方是否被吃
            if(!Check(country, it.y, it.x)){
                *row = it.y;
                *column = it.x;
                return pTarget;
            }
        }
    }
    pSelect->Select(pBoard, canplays);
    mGame->RemoveInvalidTarget(pSelect, canplays);
    if(!pTarget){
        for (auto it:canplays){
            Chess *pChess = pBoard->GetChess(it.y, it.x);
            if(pChess && (mGame->IsControllable() || pChess->GetCountry() != HAN_COUNTRY_INDEX || pChess->GetChess() == Chess::Type::Jiang_Chess)){
                pTarget = pChess;
                *row = pTarget->GetRow();
                *column = pTarget->GetColumn();
                break;
            }
        }    
    }
    if(!pTarget)pTarget = RandTarget(country, canplays, row, column);
    return pTarget;
}