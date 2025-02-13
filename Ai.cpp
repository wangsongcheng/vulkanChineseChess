#include "Ai.h"
#include "LuaFrame.h"
void PlayChess(Chess *pChess, uint32_t dstRow, uint32_t dstColumn);
#ifdef INTERNET_MODE
#include "SocketFrame.h"
extern std::array<Player, g_DefaultCountryCount>g_Players;
void SendPlayChessMessage(const Player&game, const Chess *pSelect, const Chess *pTarget);
#endif
lua_State *InitLua(){
    lua_State *luaState = luaL_newstate();
    luaL_openlibs(luaState);
    registration_function(luaState);
    return luaState;
}
void aiPlay(const Ai *pAi, lua_State *pL){
    uint32_t dstRow, dstColumn;
    Chess *pSelect = nullptr, *pTarget = nullptr;
    if(pL){
        GetSelectAndTarget(pL, &pSelect, &pTarget, &dstRow, &dstColumn);
    }
    else{
        pAi->GetPlayChess(pAi->GetCurrentCountry(), &pSelect, &pTarget, &dstRow, &dstColumn);
    }
    Chess target(dstRow, dstColumn);
    // SelectChess(g_VulkanDevice.device, pSelect);
#ifdef INTERNET_MODE
    if(pTarget){
        SendPlayChessMessage(g_Players[pSelect->GetCountry()], pSelect, pTarget);
    }
    else{
        target.SetCountry(MAX_COUNTRY_INDEX);
        SendPlayChessMessage(g_Players[pSelect->GetCountry()], pSelect, &target);
    }
#else
    PlayChess(pSelect, dstRow, dstColumn);
#endif
    // UnSelectChess(g_VulkanDevice.device);
}
void *AiPlayChess(void *userData){
    Ai *pAi = (Ai *)userData;
    lua_State *luaState = InitLua();
    if(luaL_loadfile(luaState, "ai.lua") || lua_pcall(luaState, 0, 0, 0)){
        luaError(luaState, "load luad file error:%s\n", lua_tostring(luaState, -1));
        lua_close(luaState);
        luaState = nullptr;
    }
#ifdef __linux
    if(pAi->GetPlayer() != pAi->GetCurrentCountry()){
        //因为创建的时候固定为0,所以如果此时应该ai出牌，那么应该先调用下面的函数一次
        pAi->Enable();
    }
#endif
    printf("function %s start\n", __FUNCTION__);
    while(!pAi->GameOver() || pAi->IsEnd()){
        pAi->Wait();
        if(pAi->IsPause()){
            printf("function %s pause\n", __FUNCTION__);
           pAi->Wait(); 
        }
        aiPlay(pAi, luaState);
#ifdef INTERNET_MODE
        if(g_ServerAppaction){
            for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
                if(it->ai && it->index == g_CurrentCountry){
                    pAi->Enable();
                    break;
                }
            }
        }
#else
        pAi->NextCountry();
        if(pAi->GetCurrentCountry() != pAi->GetPlayer()){
            pAi->Enable();
        }
#endif
    }
    printf("function %s end\n", __FUNCTION__);
    if(luaState){
        lua_close(luaState);
    }
    return nullptr;
}
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
//     if((*pDangerous)->chess == SHI_CHESS_INDEX_1 || (*pDangerous)->chess == SHI_CHESS_INDEX_2){
//         *pDangerous = nullptr;
//         *pRival = nullptr;
//         return;
//     }
//     // g_Chessboard.SetSelected(*pDangerous);
//     // g_Chessboard.Selected(VK_NULL_HANDLE);
//     //找一个解围的棋子
//     const ChessInfo *pChess = GetRival((*pRival)->country, (*pDangerous)->country, (*pRival)->chess);
//     if(pChess){
//         *pDangerous = pChess;
//     }
//     else{
//         if((*pDangerous)->chess != JIANG_CHESS_INDEX){
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
int32_t Ai::CanPlay(uint32_t country, const std::vector<Chess>&canplays)const{
    int32_t index = -1;
    for (size_t i = 0; i < canplays.size(); ++i){
        const Chess *pInfo = mGame->GetChess(canplays[i].GetRow(), canplays[i].GetColumn());
        if(!pInfo || pInfo->GetCountry() != country){
            index = i;
            break;
        }
    }
    return index;
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
void Ai::CreatePthread(Game *pGame){
    if(mGame != nullptr)return;
    mEnd = false;
    mPause = false;
    mGame = pGame;
#ifdef WIN32
    DWORD  pthreadId;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AiPlayChess, this, 0, &pthreadId);
#endif
#ifdef __linux
    pthread_t pthreadId;
    pthread_create(&pthreadId, nullptr, AiPlayChess, this);
#endif
}
void Ai::GetPlayChess(uint32_t country, Chess **pSelect, Chess **pTarget, uint32_t *row, uint32_t *column)const{
    std::vector<Chess>canplays;
    // const ChessInfo *pSelect = nullptr, *pRival = nullptr;
    // const uint32_t nextCountry = GetNextCountry(g_CurrentCountry);
    // const ChessInfo * = GetRival(g_CurrentCountry, (uint32_t)JIANG_CHESS_INDEX);
    // if(pRival){
    //     pSelect = g_Chessboard.GetChessInfo(g_CurrentCountry, JIANG_CHESS_INDEX);
    //     Dangerous(&pSelect, &pRival);
    // }
    // else{
    //     pRival = GetRival(g_CurrentCountry, &pSelect);
    //     if(!pRival){
    //         pSelect = nullptr;
    //         //如果某个棋子面临被吃问题, 如果有棋子可以解则解，无再逃走
    //         for (uint32_t uiChess = 1; uiChess < COUNTRY_CHESS_COUNT; ++uiChess){
    //             pRival = GetRival(g_CurrentCountry, nextCountry, uiChess);
    //             if(pRival){
    //                 pSelect = g_Chessboard.GetChessInfo(g_CurrentCountry, uiChess);
    //                 Dangerous(&pSelect, &pRival);
    //                 break;
    //             }
    //         }
    //     }
    // }
    auto pChess = mGame->GetChess();
    if(!*pSelect){
        do{
            *pSelect = pChess[ROW_COLUMN_CHESS_TO_INDEX(country, rand() % DRAW_COUNTRY_CHESS_COUNT)];
            if(*pSelect){
                canplays.clear();
                pChess[ROW_COLUMN_CHESS_TO_INDEX(country, (*pSelect)->GetChess())]->Selected(mGame->GetChess(), canplays);
            }
        }while(!*pSelect || -1 == CanPlay(country, canplays));//不用担心判断*player后死循环,因为"将"肯定会存在,否则就输了
    }
    if(!*pTarget){
        //这个分支也有吃子的可能
        canplays.clear();
        pChess[ROW_COLUMN_CHESS_TO_INDEX(country, (*pSelect)->GetChess())]->Selected(mGame->GetChess(), canplays);
        uint32_t index = 0;
        do{
            index = rand() % canplays.size();
            *pTarget = mGame->GetChess(canplays[index].GetRow(), canplays[index].GetColumn());
        } while (*pTarget && (*pTarget)->GetCountry() == country);
        *row = canplays[index].GetRow();
        *column = canplays[index].GetColumn();
    }
}
