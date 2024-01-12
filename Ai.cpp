#include "Ai.h"
extern uint32_t g_PlayerIndex;
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
int32_t Ai::CanPlay(uint32_t country, const std::vector<ChessInfo>&canplays, const Chessboard *pChessboard){
    int32_t index = -1;
    for (size_t i = 0; i < canplays.size(); ++i){
        const ChessInfo *pInfo = pChessboard->GetChessInfos(canplays[i].row, canplays[i].column);
        if(!pInfo || pInfo->country != country){
            index = i;
            break;
        }
    }
    return index;
}
Ai::Ai(/* args */){
}

Ai::~Ai(){
}
int Ai::CreatePthread(void *(*__start_routine)(void *), void *__arg){
    //注意windows和linux两部分都返回了
#ifdef WIN32
    DWORD  threadId;
    mHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)__start_routine, nullptr, 0, &threadId);
    return threadId;
#endif
#ifdef __linux
    return pthread_create(&mPthread, nullptr, __start_routine, __arg);
#endif
}
void Ai::GetPlayChessInfo(uint32_t country, const ChessInfo **pPlayer, const ChessInfo **pTarget, glm::vec2 *mousePos, const Chessboard *pChessboard){
    const Chess *pChess = nullptr;
    std::vector<ChessInfo>canplays;
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
    if(!*pPlayer){
        do{
            *pPlayer = pChessboard->GetChessInfo(country, rand() % COUNTRY_CHESS_COUNT);
            if(*pPlayer){
                canplays.clear();
                pChess = pChessboard->GetChess(country, (*pPlayer)->chess);
                pChess->Selected((const Chess* (*)[COUNTRY_CHESS_COUNT])pChessboard->GetChess(), canplays);
            }
        }while(!*pPlayer || -1 == CanPlay(country, canplays, pChessboard));//不用担心判断*player后死循环,因为"将"肯定会存在,否则就输了
    }
    if(!*pTarget){
        //这个分支也有吃子的可能, 只不过是随机的
        canplays.clear();
        pChess = pChessboard->GetChess(country, (*pPlayer)->chess);
        pChess->Selected((const Chess* (*)[COUNTRY_CHESS_COUNT])pChessboard->GetChess(), canplays);
        uint32_t index = 0;
        do{
            index = rand() % canplays.size();
            *pTarget = pChessboard->GetChessInfos(canplays[index].row, canplays[index].column);
        } while (*pTarget && (*pTarget)->country == country);
        *mousePos = glm::vec2(COLUMN_TO_X(canplays[index].column), ROW_TO_Y(canplays[index].row));
    }
}
