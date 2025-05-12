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
            target.SetCountry(Invald_Country);
            pAi->SendPlayChessMessage(g_Players[pAi->GetCurrentCountry()], pSelect, &target);
        }
    }
    else{
        // pAi->SelectChess(pSelect);
        pAi->PlayChess(pSelect, dst.y, dst.x);
        pAi->SyncBoard(pSelect, dst.y, dst.x);
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
            if(autoPlay || currentCountry != mGame->GetPlayer()){
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
Chess *Ai::GetSelect(Country country)const{
    Chess *pSelect = nullptr;
    std::vector<glm::vec2>canplays;
    // auto pBoard = mGame->GetChessboard();
    auto pCountryChess = mChessboard.GetChess(country);
    for (size_t i = 0; i < DRAW_CHESS_COUNT; ++i){
        Chess *pChess = pCountryChess[i];
        if(pChess){
            canplays.clear();
            pChess->Select(&mChessboard, canplays);
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
    // auto pBoard = mGame->GetChessboard();
    for (const auto&it:canplays){
        pTarget = mChessboard.GetChess(it.y, it.x);
        if(pTarget && pTarget->GetCountry() != pSelect->GetCountry())break;
    }
    return pTarget;
}
ChessMove Ai::GetSaveStep(Chess *pChess, uint32_t dstRow, uint32_t dstColumn){
    //注意:更新该函数后仍需要更新Game类的同名函数
   const Chess *pTarget = mChessboard.GetChess(dstRow, dstColumn);
   ChessMove move = {};
   move.chess = *pChess;
   move.is_capture = pTarget;
   if(move.is_capture){
       move.is_death = pTarget->GetChess() == Chess::Type::Jiang_Chess;
       if(move.is_death){
           move.death.country = pTarget->GetCountry();
           auto pCountryChess = mChessboard.GetChess(move.death.country);
           for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; uiChess++){
               if(pCountryChess[uiChess])move.death.chess.push_back(*pCountryChess[uiChess]);
           }
       }
       else{
           move.captured = *pTarget;
       }        
   }
   move.is_check = mGame->Check(nullptr);
   //为测试是否见面，需要一定的修改
   const glm::vec2 pos = glm::vec2(pChess->GetColumn(), pChess->GetRow());
   pChess->SetPos(dstRow, dstColumn);
   for (uint32_t srcCountry = 0; srcCountry < mGame->GetCountryCount(); srcCountry++){
       for (uint32_t dstountry = 0; dstountry < mGame->GetCountryCount(); dstountry++){
           if(srcCountry != dstountry){
               if(mChessboard.areKingsFacing((Country)srcCountry, (Country)dstountry)){
                   move.is_facing = true;
                   move.facing[1].country = (Country)dstountry;
                   move.facing[0].country = (Country)srcCountry;
                   srcCountry = mGame->GetCountryCount();
                   break;
               }
           }
       }
   }
   pChess->SetPos(pos.y, pos.x);
   if(move.is_facing){
       for (uint32_t i = 0; i < move.facing.size(); ++i){
           auto pCountryChess = mChessboard.GetChess(move.facing[i].country);
           for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; uiChess++){
               if(pCountryChess[uiChess]){
                   move.facing[i].chess.push_back(*pCountryChess[uiChess]);
               }   
           }        
       }
   }
   return move;
}
Chess *Ai::RandChess(Country country) const{
    Chess *pSelect = nullptr;
    uint32_t count = 0;//防止死循环
    std::vector<glm::vec2>canplays;
    // auto pBoard = mGame->GetChessboard();
    auto pChess = mChessboard.GetChess(country);
    const uint32_t chessCount = mChessboard.GetChessCount(count);
    do{
        pSelect = pChess[rand() % DRAW_CHESS_COUNT];
        if(pSelect){
            canplays.clear();
            pSelect->Select(&mChessboard, canplays);
            mGame->RemoveInvalidTarget(canplays);
            if(canplays.size() < 2 && count < chessCount)pSelect = nullptr;//优先走能走多格的
        }
    }while(!pSelect || canplays.empty());
    return pSelect;
}

glm::vec2 Ai::RandTarget(const Chess *pSelect, const std::vector<glm::vec2> &canplays){
    //先仔细
    glm::vec2 pos = glm::vec2(0);
    const auto country = pSelect->GetCountry();
    auto pJiang = mChessboard.GetChess(country, Chess::Type::Jiang_Chess);
    const glm::vec2 jiang = glm::vec2(pJiang->GetColumn(), pJiang->GetRow());
    std::vector<glm::vec2>can = canplays;
    while(!can.empty()){
        const auto target = can.begin() + rand() % can.size();
        SyncBoard(pSelect, target->y, target->x);
        if((Check(country, *target) && !mChessboard.Check(country, target->y, target->x)) || Check(country, jiang) || Invald_Country == areKingsFacing(country)){
            can.erase(target);
        }
        else{
            pos = *target;
        }
        UndoStep();
        if(!IsBoundary(pos.y, pos.x))break;
    }
    if(IsBoundary(pos.y, pos.x)){
        can = canplays;
        while(can.empty()){
            auto target = can.begin() + rand() % can.size();
            SyncBoard(pSelect, target->y, target->x);
            if(Check(country, jiang) || Invald_Country == areKingsFacing(country)){
                can.erase(target);
            }
            else{
                pos = *target;
            }
            UndoStep();
            if(!IsBoundary(pos.y, pos.x))break;
        }
    }
    //大部分送死情况都是因为只能走一格导致进入该条件随机了
    if(IsBoundary(pos.y, pos.x)){
        //没办法，随机一个位置吧
        pos = canplays[rand() % canplays.size()];
    }
    return pos;
}
void Ai::SyncBoard(const Chess *pChess, uint32_t dstRow, uint32_t dstColumn){
    Chess *pSelect = mChessboard.GetChess(pChess->GetCountry())[pChess->GetChessOffset()];
    mChessboard.SaveStep(GetSaveStep(pSelect, dstRow, dstColumn));
    const Chess *pTarget = mChessboard.GetChess(dstRow, dstColumn);
    if(pTarget){
        auto targetCountry = pTarget->GetCountry();
        mChessboard.CaptureChess(pSelect, pTarget);
        if(mChessboard.IsDeath(targetCountry)){
            mChessboard.DestroyCountry(targetCountry);
        }
    }
    pSelect->SetPos(dstRow, dstColumn);
    for (uint32_t srcCountry = 0; srcCountry < mGame->GetCurrentCountry(); srcCountry++){
        for (uint32_t dstountry = 0; dstountry < mGame->GetCurrentCountry(); dstountry++){
            if(srcCountry != dstountry){
                if(mChessboard.areKingsFacing((Country)srcCountry, (Country)dstountry)){
                    mChessboard.DestroyCountry((Country)srcCountry);
                    mChessboard.DestroyCountry((Country)dstountry);
                    srcCountry = mGame->GetCurrentCountry();
                    break;
                }
            }
        }
    }
}

Chess *Ai::GetSelect(Country country, const Chess *pTarget) const{
    // auto pBoard = mGame->GetChessboard();
    auto pAllChess = mChessboard.GetChess(country);
    for (size_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; uiChess++){
        Chess *pChess = pAllChess[uiChess];
        if(pChess){
            std::vector<glm::vec2>canplays;
            pChess->Select(&mChessboard, canplays);
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
// 判断是否有势力一下步会走到pos的地方, country是当前势力, 用来排除的
const Chess *Ai::Check(Country country, const glm::vec2&pos) const{
    // auto pBoard = mGame->GetChessboard();
    for (size_t i = 0; i < mGame->GetCountryCount(); ++i){
    //得先模拟走后的结果，例如这个问题, 炮吃马本来应该会被旁边的车吃，但现在的马还没有被吃，所以车肯定走不到马的位置
        if(country != i){
            const Chess *pChess = mChessboard.Check((Country)i, pos.y, pos.x);
            if(pChess){
                return pChess;
            }
        }
    }
    return nullptr;
}
Country Ai::areKingsFacing(Country country){
    Country cCountry = Invald_Country;
    for (uint32_t dstountry = 0; dstountry < mGame->GetCountryCount(); dstountry++){
        if(country != dstountry && (Invald_Country == mGame->GetNotAllianceCountry() || dstountry != GET_ALLIANCE_COUNTRY(country, mGame->GetNotAllianceCountry()))){
            if(mChessboard.areKingsFacing(country, (Country)dstountry)){
                cCountry = (Country)dstountry;
                break;
            }
        }
    }
    return cCountry;
}
const Chess *Ai::Check(Country country) const{
    const Chess *pChess = nullptr;
    // auto pBoard = mGame->GetChessboard();
    const Chess *pTaget = mChessboard.GetChess(country, Chess::Type::Jiang_Chess);
    for (size_t i = 0; i < mGame->GetCountryCount(); ++i){
        if(i != country){
            pChess = mChessboard.Check((Country)i, pTaget->GetRow(), pTaget->GetColumn());
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
    glm::vec2 pos = glm::vec2(pChess->GetColumn(), pChess->GetRow());
    std::vector<glm::vec2>canplays;
    //目前这里是按车的逻辑来写的，能被车吃到，说明中间肯定没棋子
    do{
        pos += dir;
        canplays.push_back(pos);
    } while (!IsBoundary(pos.y, pos.x) && (pos.x != pTarget->GetColumn() || pTarget->GetRow() != pos.y));
    return canplays;
}
Chess *Ai::GetCannonScreenPiece(const Chess *pPao, const Chess *pTarget) const{
    if(!pPao || !pTarget)return nullptr;
    // auto pBoard = mGame->GetChessboard();
    const glm::vec2 pao = glm::vec2(pPao->GetColumn(), pPao->GetRow()), jiang = glm::vec2(pTarget->GetColumn(), pTarget->GetRow());
    const glm::vec2 dir = glm::normalize(pao - jiang) * -1.0f;
    glm::vec2 pos = pao;
    Chess *pChess = nullptr;
    do{
        pos += dir;
        pChess = mChessboard.GetChess(pos.y, pos.x);
    }while (!pChess && !IsBoundary(pos.y, pos.x));
    return pChess;
}
//返回走后能避免pTarget被吃的棋子
Chess *Ai::GetResolveCheck_Pao(const Chess *pCheck, const Chess *pTarget, glm::vec2&pos){
    Chess *pSelect = nullptr;
    // auto pBoard = mGame->GetChessboard();
    Chess *pCannonScreen = GetCannonScreenPiece(pCheck, pTarget);
    //pCannonScreen为空说明程序有问题
    std::vector<glm::vec2>canplays;
    pCannonScreen->Select(&mChessboard, canplays);
    mGame->RemoveInvalidTarget(canplays);
    //如果炮架是自己的棋子, 那么就移动炮架
    if(pCannonScreen->GetCountry() == pTarget->GetCountry() && !canplays.empty()){
        pSelect = pCannonScreen;
        pos = RandTarget(pSelect, canplays);
    }
    if(IsBoundary(pos.y, pos.x)){
        canplays.clear();
        pTarget->Select(&mChessboard, canplays);
        mGame->RemoveInvalidTarget(canplays);
        if(canplays.empty()){
            pSelect = GetResolveCheck_Che(pCannonScreen, pTarget, pos);
        }
        else{
            //先考虑是否有其中棋子在走一步后能保护该棋子
            pSelect = GetNextDefender(pCheck, &pTarget, pos);
            if(pSelect){
                pSelect = mChessboard.GetChess(pTarget->GetCountry(), pSelect->GetRow(), pSelect->GetColumn());
            }
            else{
                pSelect = mChessboard.GetChess(pTarget->GetRow(), pTarget->GetColumn());
                pos = RandTarget(pSelect, canplays);    
            }            
        }
    }
    return pSelect;
}
Chess *Ai::GetNowDefender(const Chess *pCheck, const Chess **pTarget){
    Chess backup = *(*pTarget);//SyncBoard执行后, pTarget会因为被吃而销毁, 所以需要备份
    SyncBoard(pCheck, (*pTarget)->GetRow(), (*pTarget)->GetColumn());
    Chess *pDefender = (Chess *)mChessboard.Check(backup.GetCountry(), (*pTarget)->GetRow(), (*pTarget)->GetColumn());
    UndoStep();
    //因为被销毁了, 所以需要恢复
    *pTarget = mChessboard.GetChess(backup.GetCountry(), backup.GetRow(), backup.GetColumn());
    return pDefender;
}
Chess *Ai::GetNextDefender(const Chess *pCheck, const Chess **pTarget, glm::vec2 &pos){
    Chess *pDefender = nullptr;
    const Country country = (*pTarget)->GetCountry();
    //需要将所有棋子，所有能走的地方都走一遍, 然后调用GetNowDefender返回现在能包含的棋子
    auto pCountryChess = mChessboard.GetChess(country);
    for (uint32_t uiChess = 0; uiChess < DRAW_CHESS_COUNT; ++uiChess){
        auto pChess = pCountryChess[uiChess];
        if(pChess && uiChess != (*pTarget)->GetChessOffset()){
            std::vector<glm::vec2>canplays;
            pChess->Select(&mChessboard, canplays);
            mGame->RemoveInvalidTarget(canplays);
            for (auto&it:canplays){
                SyncBoard(pChess, it.y, it.x);
                pDefender = GetNowDefender(pCheck, pTarget);
                UndoStep();
                if(pDefender && pDefender->GetChessOffset() != pChess->GetChessOffset()){
                    pos = it;
                    pDefender = pChess;
                    uiChess = DRAW_CHESS_COUNT;
                    break;
                }
            }
        }
    }
    return pDefender;
}
// 返回能解将的棋子
Chess *Ai::GetResolveCheck(const Chess *pCheck, const Chess *pTarget, glm::vec2&pos){
    // auto pBoard = mGame->GetChessboard();
    const Chess *pDefender = nullptr;
    //优先把'将'的棋子吃掉
    Chess *pSelect = GetSelect(pTarget->GetCountry(), pCheck);
    if(pSelect)pos = glm::vec2(pCheck->GetColumn(), pCheck->GetRow());
    if(!pSelect && pTarget->GetChess() != Chess::Type::Jiang_Chess){
        //看看吃掉pTarget后，pCheck会不会被本势力其他棋子吃。但是不能拿老将赌
        pDefender = GetNowDefender(pCheck, &pTarget);
        // if(pDefender){
        //     //我赌你不敢吃
        // }
    }
    if(!pSelect && !pDefender){
        //就目前而言，能将的只有车、马、炮、兵。
        if(pCheck->GetChess() == Chess::Type::Pao_Chess){
            pSelect = GetResolveCheck_Pao(pCheck, pTarget, pos);
        }
        //马不能通过蹩马脚解将
        else if(pCheck->GetChess() == Chess::Type::Ma_Chess){
            //这时候只能走将
            pSelect = mChessboard.GetChess(pTarget->GetRow(), pTarget->GetColumn());
            std::vector<glm::vec2>canplays;
            pSelect->Select(&mChessboard, canplays);
            mGame->RemoveInvalidTarget(canplays);
            if(canplays.empty()){
                pSelect = nullptr;
            }
            else{
                pos = canplays[rand() % canplays.size()];
            }
        }
        else{
            //兵一般情况下，都能直接吃掉,所以只处理车
            pSelect = GetResolveCheck_Che(pCheck, pTarget, pos);
        }
    }
    return pSelect;
}
void RemoveSamePos(const std::vector<glm::vec2>&canplays, std::vector<glm::vec2>&targetcanplays){
    for (auto&it:canplays){
        for (auto tit = targetcanplays.begin(); tit != targetcanplays.end();){
            if(it == *tit){
                tit = targetcanplays.erase(tit);
            }
            else{
                ++tit;
            }
        }
    }
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
    // auto pBoard = mGame->GetChessboard();
    //优先走子
    std::vector<glm::vec2>canplays, targetcanplays;
    pCheck->Select(&mChessboard, canplays);
    pTarget->Select(&mChessboard, targetcanplays);
    mGame->RemoveInvalidTarget(canplays);
    mGame->RemoveInvalidTarget(targetcanplays);
    RemoveSamePos(canplays, targetcanplays);
    if(targetcanplays.empty()){
        pSelect = GetNextDefender(pCheck, &pTarget, pos);
        if(!pSelect){
            auto checanplays = GetPathBetween(pCheck, pTarget);
            auto pCountryChess = mChessboard.GetChess(pTarget->GetCountry());
            Chess *pChess = nullptr;
            for (size_t i = 0; i < DRAW_CHESS_COUNT; i++){
                targetcanplays.clear();
                pChess = pCountryChess[i];
                if(pChess && pChess->GetChessOffset() != pTarget->GetChessOffset()){
                    pChess->Select(&mChessboard,targetcanplays);
                    mGame->RemoveInvalidTarget(targetcanplays);
                    auto bpos = GetSamePos(checanplays, targetcanplays);
                    if(!IsBoundary(pos.y, pos.x)){
                        pos = bpos;
                        //走pChess来挡住车
                        pSelect = pChess;
                        break;
                    }
                }
            }    
        }
    }
    if(IsBoundary(pos.y, pos.x)){
        pSelect = mChessboard.GetChess(pTarget->GetRow(), pTarget->GetColumn());
        targetcanplays.clear();
        pSelect->Select(&mChessboard, targetcanplays);
        mGame->RemoveInvalidTarget(targetcanplays);
        pos = RandTarget(pSelect, targetcanplays);
    }
    return pSelect;
}
Chess *Ai::GetSelect(Country country, glm::vec2&pos){
    //计谋篇
    //驱虎吞狼:强制和棋多的一方结盟
    //二虎竞食:强制和棋少的一方结盟
    //离间之计:强制破坏另外两方盟约
    Chess *pSelect = nullptr;
    std::vector<glm::vec2>canplays;
    auto pBoard = mGame->GetChessboard();
    auto pCountryChess = mChessboard.GetChess(country);
    const Chess *pCheck = Check(country);
    printf("--------势力%d开始思考下棋-------\n", country);
    if(pCheck){
        pSelect = GetResolveCheck(pCheck, pCountryChess[JIANG_CHESS_OFFSET], pos);
    }
    //其实应该根据棋子的分数决定行为，例如，有更重要的棋子即将被吃，则应该先逃离。
    if(!pSelect){
        pSelect = GetSelect(country);
        if(pSelect){
            pSelect->Select(&mChessboard, canplays);
            mGame->RemoveInvalidTarget(canplays);
            auto pTarget = GetTarget(pSelect, canplays);
            if(pTarget){
                const glm::vec2 target = glm::vec2(pTarget->GetColumn(), pTarget->GetRow());
                SyncBoard(pSelect, target.y, target.x);
                if(Check(country, target)){
                    pSelect = nullptr;
                }
                else{
                    pos.y = pTarget->GetRow();
                    pos.x = pTarget->GetColumn();
                }
                UndoStep();
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
                    pSelect = GetResolveCheck(pCheck, pTarget, pos);
                    if(pSelect)break;
                }
            }
        }
    }
    if(!pSelect){
        canplays.clear();
        pSelect = RandChess(country);
        pSelect->Select(&mChessboard, canplays);
        mGame->RemoveInvalidTarget(canplays);
        pos = RandTarget(pSelect, canplays);
    }
    //调试用
    if(!pBoard->GetChess(country, pSelect->GetRow(), pSelect->GetColumn())){
        printf("......\n");
    }
    printf("--------势力%d结束的思考下棋-------\n", country);
    return pBoard->GetChess(country, pSelect->GetRow(), pSelect->GetColumn());
}
void Ai::WaitThread(){
#ifdef WIN32
    printf("in function %s:windows no wait thread\n", __FUNCTION__);
#endif
#ifdef __linux
    pthread_join(pthreadId, nullptr);
#endif

}
void Ai::CreatePthread(){
    if(!mEnd)return;
    mEnd = false;
#ifdef WIN32
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AiPlayChess, this, 0, &pthreadId);
#endif
#ifdef __linux
    pthread_create(&pthreadId, nullptr, AiPlayChess, this);
#endif
}