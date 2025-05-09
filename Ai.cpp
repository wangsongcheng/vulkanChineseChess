#include "Ai.h"
extern ImGuiInput g_ImGuiInput;
extern std::array<Player, MAX_COUNTRY_INDEX>g_Players;
void aiPlay(Ai *pAi){
    glm::vec2 dst;
    Chess *pSelect = pAi->GetSelect(pAi->GetCurrentCountry(), dst);
    const Chess *pTarget = pAi->GetChess(dst.y, dst.x);
    if(pAi->IsOnline()){
        if(pTarget){
            pAi->SendPlayChessMessage(g_Players[pAi->GetCurrentCountry()], pSelect, pTarget);
        }
        else{
            Chess target;
            target.SetPos(dst.y, dst.x);
            target.SetCountry(MAX_COUNTRY_INDEX);
            pAi->SendPlayChessMessage(g_Players[pAi->GetCurrentCountry()], pSelect, &target);
        }
    }
    else{
        // pAi->SelectChess(pSelect);
        pAi->PlayChess(pSelect, dst.y, dst.x);
        // pAi->SyncBoardCopy(pSelect, dst.y, dst.x);
        // pAi->UnSelectChess();
        //下完棋要调用下面的函数
        pAi->EnableNextCountry(g_ImGuiInput.enableAutoPlay);
    }
}
void *AiPlayChess(void *userData){
    Ai *pAi = (Ai *)userData;
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
    printf("function %s start\n", __FUNCTION__);
    while(!pAi->GameOver() && !pAi->IsEnd()){
        pAi->Wait();
        // if(pAi->IsPause()){
        //     printf("function %s pause\n", __FUNCTION__);
        //    pAi->Wait();
        // }
        if(!pAi->IsEnd())aiPlay(pAi);
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
int32_t Ai::CanPlay(uint32_t country, const std::vector<glm::vec2>&canplays)const{
    int32_t index = 0;
    auto pBoard = mGame->GetChessboard();
    for (auto&it:canplays){
        if(Check(country, it))continue;
        const Chess *pInfo = pBoard->GetChess(it.y, it.x);
        if((!pInfo || pInfo->GetCountry() != country) && !Check(country, it)){
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
        if(IsEnd())return;
        const uint32_t currentCountry = mGame->GetCurrentCountry();
        if(IsOnline() && IsServer()){
            for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
                if(it->ai && it->uCountry == currentCountry){
                    Enable();
                    break;
                }
            }
        }
        else{
            if(autoPlay || currentCountry != mGame->GetPlayerCountry()){
                Enable();
            }
        }
    }
}
//返回即将被吃的棋子
// Chess *Ai::GetCheck(uint32_t country, glm::vec2&pos)const{
//     Chess *pSelect = nullptr;
//     std::vector<glm::vec2>canplays;
//     auto pBoard = mGame->GetChessboard();
//     auto pCountryChess = pBoard->GetChess(country);
//     for (size_t i = 0; i < DRAW_CHESS_COUNT; ++i){
//         Chess *pChess = pCountryChess[i];
//         if(pChess && pChess->GetChess() != Chess::Type::Shi_Chess && pChess->GetChess() != Chess::Type::Xiang_Chess){
//             auto pc = Check(country, glm::vec2(pChess->GetColumn(), pChess->GetRow()));
//             if(pc){
//                 canplays.clear();
//                 pChess->Select(pBoard, canplays);
//                 if(!canplays.empty()){
//                     pSelect = pChess;
//                     pos.y = pc->GetRow();
//                     pos.x = pc->GetColumn();
//                     break;
//                 }    
//             }
//         }
//     }
//     return pSelect;
// }
Chess *Ai::GetTarget(uint32_t country)const{
    Chess *pSelect = nullptr;
    std::vector<glm::vec2>canplays;
    auto pBoard = mGame->GetChessboard();
    auto pCountryChess = pBoard->GetChess(country);
    for (size_t i = 0; i < DRAW_CHESS_COUNT; ++i){
        Chess *pChess = pCountryChess[i];
        if(pChess){
            canplays.clear();
            pChess->Select(pBoard, canplays);
            mGame->RemoveInvalidTarget(canplays);
            const Chess *pTarget = GetTarget(pChess, canplays);
            if(pTarget){
                pSelect = pChess;
                break;
            }
        }
    }
    return pSelect;
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
            mGame->RemoveInvalidTarget(canplays);
        }
    }while(!pSelect || -1 == CanPlay(country, canplays));
    return pSelect;
}

Chess *Ai::RandTarget(uint32_t country, const std::vector<glm::vec2> &canplays, glm::vec2&pos) const{
    int32_t index = 0;
    Chess *pTarget = nullptr;
    auto pBoard = mGame->GetChessboard();
    do{
        index = rand() % canplays.size();
        pTarget = pBoard->GetChess(canplays[index].y, canplays[index].x);
        if(canplays.size() <= 1)break;
    } while (Check(country, canplays[index]));
    pos = canplays[index];
    return pTarget;
}
// void Ai::SetNotAllianceCountry(uint32_t country, uint32_t row, uint32_t column){
//     const uint32_t mNotAllianceCountry = mGame->GetNotAllianceCountry(), playerCountry = mGame->GetPlayerCountry();
//     if(mNotAllianceCountry != INVALID_COUNTRY_INDEX)return;
//     glm::vec2 vPos[3];
//     const uint32_t territory = GET_TERRITORY_INDEX(HAN_COUNTRY_INDEX, playerCountry);
//     glm::vec2 center = mChessboard.GetPalacesCenter(territory);
//     for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
//         if((i != HAN_COUNTRY_INDEX) && mChessboard.IsDeath(i)){
//             //有势力阵亡，联盟就无效了
//             return;
//         }
//     }
//     //吴永远在中心点
//     vPos[WU_COUNTRY_INDEX] = center;
//     if(territory == HAN_TERRITORY_INDEX){
//         vPos[WEI_COUNTRY_INDEX] = glm::vec2(center.x - 1, center.y + 1);
//         vPos[SHU_COUNTRY_INDEX] = glm::vec2(center.x - 1, center.y - 1);
//     }
//     else if(territory == WEI_TERRITORY_INDEX){
//         vPos[WEI_COUNTRY_INDEX] = glm::vec2(center.x - 1, center.y - 1);
//         vPos[SHU_COUNTRY_INDEX] = glm::vec2(center.x + 1, center.y - 1);
//     }
//     else if(territory == SHU_TERRITORY_INDEX){
//         //理论上不可能，但算了，还是写上吧
//         vPos[WEI_COUNTRY_INDEX] = glm::vec2(center.x + 1, center.y - 1);
//         vPos[SHU_COUNTRY_INDEX] = glm::vec2(center.x - 1, center.y - 1);
//     }
//     else if(territory == WU_TERRITORY_INDEX){
//         vPos[WEI_COUNTRY_INDEX] = glm::vec2(center.x + 1, center.y - 1);
//         vPos[SHU_COUNTRY_INDEX] = glm::vec2(center.x + 1, center.y + 1);
//     }
//     uint32_t allianceCountry = INVALID_COUNTRY_INDEX;
//     for (size_t i = 0; i < MAX_COUNTRY_INDEX; ++i){
//         if(vPos[i].x == column && vPos[i].y == row && country != i){
//             allianceCountry = i;
//             break;
//         }
//     }
//     if(allianceCountry != INVALID_COUNTRY_INDEX && allianceCountry != country){
//         mChessboard.GetCountryChess(mNotAllianceCountry, HAN_COUNTRY_INDEX);
//         mChessboard.DestroyCountry(HAN_COUNTRY_INDEX);
//     }
// }
// void Ai::SyncBoardCopy(Chess *pChess, uint32_t dstRow, uint32_t dstColumn){
//     Chess *pSelect = mChessboard.GetChess(pChess->GetCountry())[pChess->GetChessOffset()];
//     mChessboard.SaveStep(pSelect->GetRow(), pSelect->GetColumn(), dstRow, dstColumn);
//     const Chess *pTarget = mChessboard.GetChess(dstRow, dstColumn);
//     if(pTarget){
//         uint32_t targetCountry = pTarget->GetCountry();
//         mChessboard.CaptureChess(pSelect, pTarget);
//         if(mChessboard.IsDeath(targetCountry)){
//             mChessboard.DestroyCountry(targetCountry);
//         }
//     }
//     pSelect->SetPos(dstRow, dstColumn);
//     for (uint32_t srcCountry = 0; srcCountry < mGame->GetCurrentCountry(); srcCountry++){
//         for (uint32_t dstountry = 0; dstountry < mGame->GetCurrentCountry(); dstountry++){
//             if(srcCountry != dstountry){
//                 if(mChessboard.areKingsFacing(srcCountry, dstountry)){
//                     mChessboard.DestroyCountry(srcCountry);
//                     mChessboard.DestroyCountry(dstountry);
//                     srcCountry = mGame->GetCurrentCountry();
//                     break;
//                 }
//             }
//         }
//     }
//     if(!mGame->IsControllable() && pSelect->GetChess() == Chess::Type::Ma_Chess){
//         //不用提示了。因为结盟后，汉的棋子会归另一方
//         SetNotAllianceCountry(pSelect->GetCountry(), pSelect->GetRow(), pSelect->GetColumn());
//     }
// }

Chess *Ai::GetSelect(uint32_t country, const Chess *pTarget) const{
    auto pBoard = mGame->GetChessboard();
    auto pAllChess = pBoard->GetChess(country);
    for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; uiChess++){
        Chess *pChess = pAllChess[uiChess];
        if(pChess){
            std::vector<glm::vec2>canplays;
            pChess->Select(pBoard, canplays);
            mGame->RemoveInvalidTarget(canplays);
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
const Chess *Ai::Check(uint32_t country, const glm::vec2&pos) const{
    auto pBoard = mGame->GetChessboard();
    for (size_t i = 0; i < mGame->GetCountryCount(); ++i){
    //得先模拟走后的结果，例如这个问题, 炮吃马本来应该会被旁边的车吃，但现在的马还没有被吃，所以车肯定走不到马的位置
        if(country != i){
            const Chess *pChess = pBoard->Check(i, pos.y, pos.x);
            if(pChess){
                if(pChess->GetChess() != Chess::Type::Pao_Chess || pBoard->GetChess(pos.y, pos.x)){
                    return pChess;
                }
            }
        }
    }
    return nullptr;
}
const Chess *Ai::Check(uint32_t country) const{
    const Chess *pChess = nullptr;
    auto pBoard = mGame->GetChessboard();
    const Chess *pTaget = pBoard->GetChess(country, Chess::Type::Jiang_Chess);
    if(!pTaget)return pChess;
    for (size_t i = 0; i < mGame->GetCountryCount(); ++i){
        if(i != country){
            pChess = pBoard->Check(i, pTaget->GetRow(), pTaget->GetColumn());
            if(pChess){
                break;
            }    
        }
    }
    return pChess;
}
bool Ai::IsBoundary(int32_t row, int32_t column)const{
    if(row < 0 || column < 0)return true;
    if(row > CHESSBOARD_ROW || column > CHESSBOARD_COLUMN){
        // printf("到达边界, row:%u, column:%u\n", row, column);
        return true;
    }
    else if ((row < CHESSBOARD_BING_GRID_DENSITY || row > CHESSBOARD_BOUNDARY_CENTER_RECT_COUNT + CHESSBOARD_BING_GRID_DENSITY)
    && (column < CHESSBOARD_BING_GRID_DENSITY || column > CHESSBOARD_BOUNDARY_CENTER_RECT_COUNT + CHESSBOARD_BING_GRID_DENSITY)){
        // printf("到达边界, row:%u, column:%u\n", row, column);
        return true;
    }
    return false;
}
std::vector<glm::vec2>Ai::GetPathBetween(const Chess *pChess, const Chess *pTarget) const{
    if(!pTarget || !pChess)return {};
    const glm::vec2 che = glm::vec2(pChess->GetColumn(), pChess->GetRow()), target = glm::vec2(pTarget->GetColumn(), pTarget->GetRow());
    const glm::vec2 dir = glm::normalize(che - target) * -1.0f;
    glm::vec2 pos = glm::vec2(0);
    std::vector<glm::vec2>canplays;
    //目前这里是按车的逻辑来写的，能被车吃到，说明中间肯定没棋子
    do{
        pos += dir;
        canplays.push_back(pos);
    } while (IsBoundary(pos.y, pos.x) && pos.x != pTarget->GetColumn() && pTarget->GetRow() != pos.y);
    return canplays;
}
Chess *Ai::GetCannonScreenPiece(const Chess *pPao, const Chess *pTarget) const{
    if(!pPao || !pTarget)return nullptr;
    auto pBoard = mGame->GetChessboard();
    const glm::vec2 pao = glm::vec2(pPao->GetColumn(), pPao->GetRow()), jiang = glm::vec2(pTarget->GetColumn(), pTarget->GetRow());
    const glm::vec2 dir = glm::normalize(pao - jiang) * -1.0f;
    glm::vec2 pos = pao;
    Chess *pChess = nullptr;
    do{
        pos += dir;
        pChess = pBoard->GetChess(pos.y, pos.x);
    }while (!pChess && !IsBoundary(pos.y, pos.x));
    return pChess;
}
//返回走后能避免pTarget被吃的棋子
Chess *Ai::GetResolveCheck_Pao(const Chess *pCheck, const Chess *pTarget, glm::vec2&pos){
    Chess *pSelect = nullptr;
    auto pBoard = mGame->GetChessboard();
    //找出炮是通过哪个棋子吃将的
    Chess *pCannonScreen = GetCannonScreenPiece(pCheck, pTarget);
    //不需要判空，pCannonScreen不能为空，为空说明程序有问题
    std::vector<glm::vec2>canplays;
    pCannonScreen->Select(pBoard, canplays);
    mGame->RemoveInvalidTarget(canplays);
    //如果炮架是自己的棋子, 那么就移动炮架
    if(pCannonScreen->GetCountry() == pTarget->GetCountry() && !canplays.empty()){
        pSelect = pCannonScreen;
        pos = canplays[rand() % canplays.size()];
    }
    else{
        canplays.clear();
        pTarget->Select(pBoard, canplays);
        mGame->RemoveInvalidTarget(canplays);
        if(canplays.empty()){
            pSelect = GetResolveCheck_Che(pCheck, pCannonScreen, pos);
        }
        else{
            pSelect = pBoard->GetChess(pTarget->GetRow(), pTarget->GetColumn());
            pos = canplays[rand() % canplays.size()];
        }
    }
    return pSelect;
}
//被将的国家和造成将的棋子;返回能解将的棋子
Chess *Ai::ResolveCheck(const Chess *pCheck, const Chess *pTarget, glm::vec2&pos){
    auto pBoard = mGame->GetChessboard();
    //优先把'将'的棋子吃掉
    Chess *pSelect = GetSelect(pTarget->GetCountry(), pCheck);
    if(!pSelect){
        //就目前而言，能将的只有车、马、炮、兵。
        //特别是士，经常因为炮而移动到非法位置
        if(pCheck->GetChess() == Chess::Type::Pao_Chess){
            pSelect = GetResolveCheck_Pao(pCheck, pTarget, pos);
        }
        //马不能通过蹩马脚解将
        else if(pCheck->GetChess() == Chess::Type::Ma_Chess){
            //这时候只能走将
            pSelect = pBoard->GetChess(pTarget->GetRow(), pTarget->GetColumn());
            std::vector<glm::vec2>canplays;
            pSelect->Select(pBoard, canplays);
            mGame->RemoveInvalidTarget(canplays);
            pos = canplays[rand() % canplays.size()];
        }
        else{
            //兵一般情况下，都能直接吃掉,所以只处理车
            pSelect = GetResolveCheck_Che(pCheck, pTarget, pos);
        }
    }
    return pSelect;
}
glm::vec2 GetSamePos(const std::vector<glm::vec2>&canplays, const std::vector<glm::vec2>&targetcanplays){
    for (auto&it:canplays){
        for (auto&tit:targetcanplays){
            if(tit == it){
                return it;
            }
        }
    }
    return glm::vec2(0);
}
Chess *Ai::GetResolveCheck_Che(const Chess *pCheck, const Chess *pTarget, glm::vec2&pos){
    //只需要判断哪个棋子能走到车的路径上即可
    Chess *pSelect = nullptr;
    auto pBoard = mGame->GetChessboard();
    //优先走子
    std::vector<glm::vec2>canplays;
    pTarget->Select(pBoard, canplays);
    mGame->RemoveInvalidTarget(canplays);
    if(canplays.empty()){
        auto checanplays = GetPathBetween(pCheck,pTarget);
        auto pCountryChess = pBoard->GetChess(pTarget->GetCountry());
        Chess *pChess = nullptr;
        for (size_t i = 0; i < DRAW_CHESS_COUNT; i++){
            canplays.clear();
            pChess = pCountryChess[i];
            if(pChess){
                pChess->Select(pBoard,canplays);
                mGame->RemoveInvalidTarget(canplays);
                auto bpos = GetSamePos(checanplays, canplays);
                if(!IsBoundary(pos.y, pos.x)){
                    pos = bpos;
                    //走pChess来挡住车
                    pSelect = pChess;
                    break;
                }
            }
        }
    }
    else{
        pSelect = pBoard->GetChess(pTarget->GetRow(), pTarget->GetColumn());
        pos = canplays[rand()%canplays.size()];
    }
    return pSelect;
}
Chess *Ai::GetSelect(uint32_t country, glm::vec2&pos){
    //计谋篇
    //驱虎吞狼:强制和棋多的一方结盟
    //二虎竞食:强制和棋少的一方结盟
    //离间之计:强制破坏另外两方盟约
/*
    将
        面临被吃的危险
            1,有其他棋子(包括自身)可以解则解
            2.没路走就走其他棋子,不然要死循环了
            3.没路、没其他棋走则直接跳过。反正这种情况少见
    其他棋子:
        可以先找敌人
        找不到的话则说明没有保护的棋子
        面临被吃的危险
            1.有其他棋子(包括自身)可以解则解
            2.没有的话如果有保护的棋子，也可以不走
            3.都没有就而且没路的话就参考上面
    //除将外，其他棋子均可以在有保护的情况下不走
*/
    Chess *pSelect = nullptr;
    std::vector<glm::vec2>canplays;
    auto pBoard = mGame->GetChessboard();
    auto pCountryChess = pBoard->GetChess(country);
    const Chess *pCheck = Check(country);
    //现在可以同添加一个棋盘变量，然后模拟走过的情况，这样就能判断吃子后会不会被吃。可能涉及棋子分数的计算
    //还需要考虑该棋子走开后，将会不会被吃掉....不过...如果不自己搞一个chessboard类模拟的话，似乎没法判断啊
    if(pCheck){
        pSelect = ResolveCheck(pCheck, pCountryChess[JIANG_CHESS_OFFSET], pos);
    }
    //其实应该根据棋子的分数决定行为，例如，有更重要的棋子即将被吃，则应该先逃离。
    if(!pSelect){
        pSelect = GetTarget(country);
        if(pSelect){
            pSelect->Select(pBoard, canplays);
            mGame->RemoveInvalidTarget(canplays);
            auto pTarget = GetTarget(pSelect, canplays);
            if(pTarget){
                pos.y = pTarget->GetRow();
                pos.x = pTarget->GetColumn();    
            }
            else{
                pSelect = nullptr;
            }
        }
    }
    if(!pSelect){
        for (uint32_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
            auto pTarget = pCountryChess[uiChess];
            if(pTarget){
                pCheck = Check(country, glm::vec2(pTarget->GetColumn(), pTarget->GetRow()));
                if(pCheck){
                    pSelect = ResolveCheck(pCheck, pTarget, pos);
                    break;
                    }
                }                    
            }
    }
    if(!pSelect){
        canplays.clear();
        pSelect = RandChess(country);
        pSelect->Select(pBoard, canplays);
        mGame->RemoveInvalidTarget(canplays);
        RandTarget(country, canplays, pos);
    }
    return pSelect;
}
void Ai::WaitThread(){
#ifdef WIN32
    printf("in function %s:windows no wait thread\n", __FUNCTION__);
#endif
#ifdef __linux
    pthread_join(pthreadId, nullptr);
#endif

}
void Ai::CreatePthread(Game *pGame, OnLine *pOnline){
    if(!mEnd)return;
    mEnd = false;
    // mPause = false;
    mGame = pGame;
    mOnline = pOnline;
    // const uint32_t playerCountry = mGame->GetPlayerCountry();
    // mChessboard.InitializeChess(playerCountry, mGame->IsControllable(), mGame->GetCountryCount());
#ifdef WIN32
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AiPlayChess, this, 0, &pthreadId);
#endif
#ifdef __linux
    pthread_create(&pthreadId, nullptr, AiPlayChess, this);
#endif
}