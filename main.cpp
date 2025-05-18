#include <array>
#include <thread>
#include <chrono>
#include <iostream>
#ifdef __linux__
#include <dirent.h>
#endif
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "Ai.h"
#include "Game.h"
#include "OnLine.h"
#include "Pipeline.hpp"
#include "VulkanImgui.h"
#include "VulkanWindow.h"
#include "imgui_impl_glfw.h"
enum IMGUI_BUTTON_IDENTIFIER{
    IMGUI_BUTTON_UNKNOW = 0,
    IMGUI_BUTTON_OK,
    IMGUI_BUTTON_CANCEL,
};

VulkanPool g_VulkanPool;
VulkanQueue g_VulkanQueue;
VulkanDevice g_VulkanDevice;
VulkanWindow g_VulkanWindow;
VulkanSynchronize g_VulkanSynchronize;

uint32_t g_WindowWidth = (CHESSBOARD_RECT_COUNT + 2) * CHESSBOARD_RECT_SIZE + MAX_CHESSBOARD_LINE * CHESSBOARD_LINE_WIDTH, g_WindowHeight = g_WindowWidth;

VulkanImGui g_VulkanImGui;
VkCommandBuffer g_CommandBuffers;

void (*g_OpenFileFunCall)(const std::string&file);
std::vector<const char *>g_FileTypeItem;

VkPipelineCache g_PipelineCache;
VkPipelineLayout g_PipelineLayout;
VkDescriptorSetLayout g_SetLayout;
VkPipeline g_Fill, g_Wireframe, g_Font;

Ai g_Ai;
Game g_Game;
OnLine g_OnLine;
ImGuiInput g_ImGuiInput;
glm::vec2 g_JiangPos[MAX_COUNTRY_INDEX];//目前只用于RotateChess
std::array<Player, MAX_COUNTRY_INDEX>g_Players;
//用数组是为了在联机模式下显示和控制其他玩家的国籍;目前只能选择ai的国籍
std::array<std::vector<std::string>, MAX_COUNTRY_INDEX>g_CountryItems;
void ResetCountryItem(std::vector<std::string>&countryItems){
    std::vector<std::string>country = { "?", "魏", "蜀", "吴" };
    if(g_Game.IsControllable()){
        country.push_back("汉");
    }
    if(countryItems.size() != country.size())countryItems.resize(country.size());
    for (size_t i = 0; i < countryItems.size(); ++i){
        countryItems[i] = country[i];
    }
}
void NewGame(Country playerCountry = Invald_Country, Country currentCountry = Invald_Country){
    // g_Game.EndGame();
    if(g_ImGuiInput.enableAi){
        g_Ai.End();
        g_Ai.WaitThread();
    }
    g_Game.NewGame(playerCountry, currentCountry);
    g_Ai.InitializeChessboard();
    if(g_ImGuiInput.enableAi){
        g_Ai.CreatePthread();
    }
}
void ShowPlayerCountryCombo(){
    static std::string currentCountryItem = g_CountryItems[0][0];
    if(ImGui::BeginCombo("势力", currentCountryItem.c_str())){
        for (auto countryIt = g_CountryItems[0].begin(); countryIt != g_CountryItems[0].end(); ++countryIt){
            bool is_selected = currentCountryItem == *countryIt;
            if (ImGui::Selectable(countryIt->c_str(), is_selected)){
                currentCountryItem = *countryIt;
                if(currentCountryItem == "?"){
                    NewGame();
                }
                else if(currentCountryItem == "魏"){
                    NewGame(Wei_Country);
                }
                else if(currentCountryItem == "蜀"){
                    NewGame(Shu_Country);
                }
                else if(currentCountryItem == "吴"){
                    NewGame(Wu_Country);
                }
                else if(currentCountryItem == "汉"){
                    NewGame(Han_Country);
                }
                break;
            }
        }
        ImGui::EndCombo();
    }
}
void RandomNumber(uint32_t max, uint32_t count, Player *result, uint32_t min = 0){
#if _MSVC_LANG >= 202002L || __cplusplus >= 202002L
    std::vector<Player> v(count);
    std::random_device rd;
    std::iota(v.begin(), v.end(), 0);
    std::ranges::shuffle(v, rd);
    memcpy(result, v.data(), sizeof(Player) * count);
#else
    for (size_t i = 0; i < count; ++i){
        if(result[i].random)result[i].uCountry = (Country)(rand() % max + min);
        for (size_t j = 0; j < i; ++j){
            if(result[i].uCountry == result[j].uCountry){
                --i;
                continue;
            }
        }
    }
#endif
}
Country countryToindex(const char *country){
    if(!strcmp(country, "魏")){
        return Wei_Country;
    }
    else if(!strcmp(country, "蜀")){
        return Shu_Country;
    }
    else if(!strcmp(country, "吴")){
        return Wu_Country;
    }
    else if(!strcmp(country, "汉")){
        return Han_Country;
    }
    return Invald_Country;
}
void RandomCountry(){
    Player countryIndex[MAX_COUNTRY_INDEX];
    const char *countryName[MAX_COUNTRY_INDEX];
    countryName[Wu_Country] = "吴";
    countryName[Wei_Country] = "魏";
    countryName[Shu_Country] = "蜀";
    countryName[Han_Country] = "汉";
    uint32_t playerIndex = 0;
    for (auto it = g_Players.begin(); it != g_Players.end(); ++it, ++playerIndex){
        if(strcmp(it->country, "?")){
            countryIndex[playerIndex].random = false;
            countryIndex[playerIndex].uCountry = countryToindex(it->country);
        }
        else{
            countryIndex[playerIndex].random = true;
        }
    }
    const uint32_t countryCount = g_Game.GetCountryCount();
    RandomNumber(countryCount, countryCount, countryIndex);
    playerIndex = 0;
    for (auto it = g_Players.begin(); it != g_Players.end() && playerIndex < g_Game.GetCountryCount(); ++playerIndex, ++it){
        strcpy(it->country, countryName[countryIndex[playerIndex].uCountry]);
        it->uCountry = countryIndex[playerIndex].uCountry;    
    }
}
void SendPlayersInfoation(){
    for (uint32_t uiPlayer = 0; uiPlayer < g_Players.size(); ++uiPlayer){
        if(g_Players[uiPlayer].uCountry != Invald_Country){
            g_OnLine.SendPlayersInfoation(uiPlayer, &g_Players[uiPlayer]);
        }
    }
}
auto CreateThread(void *(*__start_routine)(void *), void *__arg){
    #ifdef WIN32
    DWORD  threadId;
    HANDLE pthreadId;
    pthreadId = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)__start_routine, __arg, 0, &threadId);
#endif
#ifdef __linux
    pthread_t pthreadId;
    pthread_create(&pthreadId, nullptr, __start_routine, __arg);
#endif
    return pthreadId;
}
//不知道为什么，玩家离开事件中，发送游戏结束事件，该函数一直收不到
//但process_client函数能收到, 我怀疑是直接绕过该函数发到process_client函数, 但为什么呢?
void *process_server(void *userData){
    GameMessage message;
    uint32_t socketindex = *(SOCKET *)userData;
    printf("function %s start, socketindex is %d\n", __FUNCTION__, socketindex);
    do{
        g_OnLine.RecvFromClient(socketindex, &message, sizeof(message));
        if(message.event == PLAYER_EXIT_GAME_EVENT && socketindex == message.clientIndex){
            g_OnLine.ShutdownClient(socketindex);
        }
        g_OnLine.SendToAllClient(&message, sizeof(message));
    } while (message.event != GAME_OVER_GAME_EVENT && (message.event != PLAYER_EXIT_GAME_EVENT || socketindex != message.clientIndex));
    printf("function %s end, socketindex is %d\n", __FUNCTION__, socketindex);
    return nullptr;
}
void *server_start(void *userData){
    GameMessage message;
    static uint32_t socketIndex[] = { 0, 1, 2, 3 };
    for (uint32_t i = 0; i < g_Game.GetCountryCount(); ++i){
        if(INVALID_SOCKET != g_OnLine.AcceptClient(i, &message, sizeof(message))){
            if(message.event == AI_JOIN_GAME_GAME_EVENT){
                g_Players[i].ai = true;
                g_OnLine.ShutdownClient(i);
                strcpy(g_Players[i].name, "ai");
            }
            else{
                sprintf(g_Players[i].name, "player%d", i + 1);
            }
            //调用这个函数，期望的是能告诉所有客户端，有新客户端加入，并且会把加入的客户端信息发送给所有客户端
            //然后单独给刚加入的客户端发送所有玩家信息
            g_OnLine.SendClientInfoation(i, &g_Players[i]);
            for (uint32_t uiClientIndex = 0; uiClientIndex < i; ++uiClientIndex){
                g_OnLine.SendClientInfoation(uiClientIndex, &g_Players[uiClientIndex]);
            }
            if(message.event != AI_JOIN_GAME_GAME_EVENT)CreateThread(process_server, &socketIndex[i]);
        }
    }
    if(g_Game.IsControllable()){
        message.event = ENABLE_HANG_GAME_EVENT;
        g_OnLine.SendToAllClient(&message, sizeof(GameMessage));
    }
    RandomCountry();
    SendPlayersInfoation();
    g_OnLine.SendCurrentCountry(Wu_Country);
    //游戏开始, 发消息告诉所有客户端游戏开始
    message.event = GAME_START_GAME_EVENT;
    g_OnLine.SendToAllClient(&message, sizeof(message));
    return nullptr;
}
void InitJiangPos(){
    const Chessboard *pBoard = g_Game.GetChessboard();
    const Chess *pChess = pBoard->GetChess(Wei_Country)[Chess::Type::Jiang_Chess];
    g_JiangPos[Wei_Country] = glm::vec2(pChess->GetColumn(), pChess->GetRow());
    pChess = pBoard->GetChess(Shu_Country)[Chess::Type::Jiang_Chess];
    g_JiangPos[Shu_Country] = glm::vec2(pChess->GetColumn(), pChess->GetRow());
    pChess = pBoard->GetChess(Wu_Country)[Chess::Type::Jiang_Chess];
    g_JiangPos[Wu_Country] = glm::vec2(pChess->GetColumn(), pChess->GetRow());
    pChess = pBoard->GetChess(Han_Country)[Chess::Type::Jiang_Chess];
    g_JiangPos[Han_Country] = glm::vec2(pChess->GetColumn(), pChess->GetRow());
}
void Invert(uint32_t row, uint32_t column, uint32_t&newRow, uint32_t&newColumn){
    newRow = abs((float)CHESSBOARD_ROW - row);
    newColumn = abs((float)CHESSBOARD_COLUMN - column);
}
//country为下棋的国家
void RotateChess(uint32_t country, uint32_t row, uint32_t column, uint32_t&newRow, uint32_t&newColumn){
    uint32_t uiCountry = country;
    const uint32_t clientIndex = g_OnLine.GetClientIndex();
    for (size_t i = 0; i < g_Game.GetCountryCount(); ++i){
        if(g_Players[i].ai && g_Players[i].uCountry == country){
            uiCountry = g_Players[0].uCountry;
            break;
        }
    }
    if(uiCountry != g_Players[clientIndex].uCountry){
        //因为ai下棋的位置和服务端一样，所以必须用服务端的国家索引
        if(g_JiangPos[uiCountry].x == g_JiangPos[g_Players[clientIndex].uCountry].x){
            Invert(row, column, newRow, newColumn);
        }
        else{
            newRow = column;
            newColumn = row;
            //要获得country在哪个territory, 但不知道行列
            const uint32_t territory = GET_TERRITORY_INDEX(uiCountry, g_Players[clientIndex].uCountry);
            if(Han_Country == territory){
                newColumn = abs((float)newColumn - CHESSBOARD_COLUMN);
            }
            else{
                newRow = abs((float)newRow - CHESSBOARD_ROW);
            }
        }
    }
}
void DeleteCountryItem(const char *country, std::vector<std::string>&countryItems){
    const std::string newCountry[] = { "?", "魏", "蜀", "吴", "汉" };
    uint32_t count = IM_ARRAYSIZE(newCountry) - 1;
    if(g_Game.IsControllable()){
        ++count;
    }
    for (size_t i = 0; i < count; ++i){
        if(newCountry[i] == country){
            countryItems[i] =countryItems[0];
            break;
        }
    }
}
void PlayerChangeCountry(uint32_t clientIndex, const char *country){
    strcpy(g_Players[clientIndex].country, country);
    // g_Players[clientIndex].random = !strcmp(country, "?");
    const uint32_t client = g_OnLine.GetClientIndex();
    ResetCountryItem(g_CountryItems[client]);
    for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
        if(strcmp(it->country, "?")){
            DeleteCountryItem(it->country, g_CountryItems[client]);
        }
    }
    if(g_OnLine.IsServer()){
        auto aiClientIndex = g_OnLine.GetAiClientIndex();
        for (size_t i = 0; i < 2; ++i){
            if(aiClientIndex[i] != INVALID_CLIENT_INDEX)
                DeleteCountryItem(country, g_CountryItems[aiClientIndex[i]]);
        }
    }
}
void *process_client(void *userData){
    GameMessage message;
    do{
        g_OnLine.ClientRecvFrom(&message, sizeof(GameMessage));
        if(message.event == GAME_START_GAME_EVENT){
            g_Game.EnableOnline();
            uint32_t aiIndex = 1, index = 0;
            for (auto it = g_Players.begin() + 1; it != g_Players.end(); ++it, ++aiIndex){
                if(it->ai){
                    g_OnLine.SetAiClientIndex(index, aiIndex);
                    ++index;
                }
            }
            g_Game.StartGame();
            const uint32_t clientIndex = g_OnLine.GetClientIndex();
            printf("game start, current country is %d, you country index is %d\n", g_Game.GetCurrentCountry(), g_Players[clientIndex].uCountry);
            NewGame(g_Players[clientIndex].uCountry, g_Game.GetCurrentCountry());
            InitJiangPos();
        }
        else if(message.event == ENABLE_HANG_GAME_EVENT){
            g_Game.EnableHan();
        }
        else if(message.event == JOINS_GAME_GAME_EVENT){
            const uint32_t clientIndex = message.clientIndex;
            memcpy(&g_Players[clientIndex], &message.player, sizeof(Player));
            PlayerChangeCountry(clientIndex, message.player.country);
            printf("player joins game:name %s, country %s\n", g_Players[clientIndex].name, g_Players[clientIndex].country);
        }
        else if(message.event == CURRENT_COUNTRY_GAME_EVENT){
            g_Game.SetCurrentCountry(message.player.uCountry);
            printf("current country %d\n", message.player.uCountry);
        }
        else if(message.event == CHANGE_PLAYER_COUNTRY_GAME_EVENT){
            PlayerChangeCountry(message.clientIndex, message.player.country);
        }
        else if(message.event == SELF_CLIENT_INFORMATION_GAME_EVENT){
            g_OnLine.SetClientIndex(message.clientIndex);
        }
        else if(message.event == CHANGE_PLAYER_INFORMATION_GAME_EVENT){
            g_Players[message.clientIndex].ai = message.player.ai;
            strcpy(g_Players[message.clientIndex].name, message.player.name);
            g_Players[message.clientIndex].uCountry = message.player.uCountry;
            strcpy(g_Players[message.clientIndex].country, message.player.country);
        }
        else if(message.event == PLAY_CHESS_GAME_EVENT){
            auto pBoard = g_Game.GetChessboard();
            auto pChess = pBoard->GetChess(message.select.GetCountry());
            uint32_t dstRow = message.target.GetRow(), dstColumn = message.target.GetColumn();
            Chess *pStart = pChess[message.select.GetChessOffset()], *pTarget = nullptr;
            if(message.target.GetCountry() >= MAX_COUNTRY_INDEX){
                RotateChess(g_Game.GetCurrentCountry(), dstRow, dstColumn,dstRow, dstColumn);
            }
            else{
                pChess = pBoard->GetChess(message.target.GetCountry());
                pTarget = pChess[message.target.GetChessOffset()];
                dstRow = pTarget->GetRow();
                dstColumn = pTarget->GetColumn();
            }
            g_Game.PlayChess(pStart, dstRow, dstColumn);
            g_Ai.SyncBoard(pStart, dstRow, dstColumn);
            if(!g_Ai.IsEnd()){
                if(g_Game.GameOver()){
                    g_Ai.End();
                }
                g_Ai.EnableNextCountry(g_ImGuiInput.enableAutoPlay);
            }
        }
        else if(message.event == PLAYER_EXIT_GAME_EVENT){
            printf("player exit, client index %d\n", message.clientIndex);
            if(g_OnLine.IsServer()){
                g_OnLine.ShutdownClient(message.clientIndex);
                const uint32_t playerCount = g_OnLine.GetPlayerCount();
                if(playerCount > 1){//因为服务端本身还创建了一个客户端
                    message.event = CHANGE_PLAYER_INFORMATION_GAME_EVENT;
                    message.player.ai = true;
                    strcpy(message.player.name, "ai");
                    message.player.uCountry = g_Players[message.clientIndex].uCountry;
                    strcpy(message.player.country, g_Players[message.clientIndex].country);
                }
                else{
                    message.event = GAME_OVER_GAME_EVENT;
                }
                g_OnLine.SendToAllClient(&message, sizeof(message));
                message.event = INVALID_GAMER_EVENT;
            }
            else if(message.clientIndex == g_OnLine.GetClientIndex()){
                g_Ai.End();
                g_OnLine.Cleanup();
                g_ImGuiInput.Init();
                g_Game.EndGame();
                for (uint32_t i = 0; i < g_Players.size(); ++i){
                    g_Players[i].ai = false;
                    memset(g_Players[i].name, 0, strlen(g_Players[i].name));
                    memset(g_Players[i].country, 0, strlen(g_Players[i].country));
                }        
                break;
            }
        }
        else if(message.event == GAME_OVER_GAME_EVENT){
            printf("game over, client indec is %d\n", message.clientIndex);
            g_Ai.End();
            g_OnLine.Cleanup();
            g_ImGuiInput.Init();
            g_Game.EndGame();
            for (uint32_t i = 0; i < g_Players.size(); ++i){
                g_Players[i].ai = false;
                memset(g_Players[i].name, 0, strlen(g_Players[i].name));
                memset(g_Players[i].country, 0, strlen(g_Players[i].country));
            }
        }
        else{
            printf("invalid event:%d\n", message.event);
        }
    }while(message.event != GAME_OVER_GAME_EVENT);
    printf("function %s end\n", __FUNCTION__);
    return nullptr;
}
void splicingDirectory(const std::vector<std::string>&subDir, std::string&path){
    std::string newPath;
    for (size_t i = 0; i < subDir.size(); ++i){
        newPath += subDir[i];
    }
    path = newPath;
}
void getSubdirectory(const std::string&path, std::vector<std::string>&subDir){
    //得到从0到'/'的字符串。并将字符移动到‘/’之外。然后重复
    // int index = 0;
#ifdef __linux__
    char cSeparator = '/';
#endif
#ifdef WIN32
    char cSeparator = '\\';
#endif
    std::string sPath = path;
    if(sPath[sPath.size() - 1] != cSeparator){
            sPath += cSeparator;
    }
    const char *ptrStart = sPath.c_str();
    const char *ptrEnd = nullptr;
    while(ptrEnd = strchr(ptrStart, cSeparator)){
            int size = ptrEnd - ptrStart;
            subDir.push_back(std::string(ptrStart, size + 1));
            ptrStart += size + 1;
    }
}
void getFileFromDirectory(const std::string&path, std::vector<std::string>&folder, std::vector<std::string>&file){
#ifdef __linux
    DIR *d;
    dirent *dFile = NULL;
    if(!(d = opendir(path.c_str()))){
        perror("opendir error");
        printf("path is %s\n", path.c_str());
        return;
    }
    while((dFile = readdir(d))){
        if(strcmp(dFile->d_name, ".") && strcmp(dFile->d_name, "..")){
            if(dFile->d_type == DT_DIR){
                folder.push_back(dFile->d_name);
            }
            else{
                file.push_back(dFile->d_name);
            }
        }
    }
    closedir(d);
#endif
#ifdef WIN32
    HANDLE hListFile;
    CHAR szFilePath[MAX_PATH];
    WIN32_FIND_DATA FindFileData;

    lstrcpyA(szFilePath, path.c_str());

    lstrcatA(szFilePath, "\\*");
    hListFile = FindFirstFile(szFilePath, &FindFileData);
    //判断句柄
    if (hListFile == INVALID_HANDLE_VALUE){
        printf("错误： %d\n", GetLastError());
        return;
    }
    else{
        do{
            if(lstrcmp(FindFileData.cFileName,TEXT("."))==0 || lstrcmp(FindFileData.cFileName,TEXT(".."))==0){
                continue;
            }
            //打印文件名、目录名
            //printf("%s\t\t", FindFileData.cFileName);
            //判断文件属性，是否为目录
            if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
                folder.push_back(FindFileData.cFileName);
            }
            else {
                file.push_back(FindFileData.cFileName);
            }
        } while (FindNextFile(hListFile, &FindFileData));
    }
#endif // WIN32
}
std::string GetCurrentDirectory(){
#ifdef __linux
    std::string currentDir = get_current_dir_name();
#endif
#ifdef WIN32
    char directory[MAX_PATH] = { 0 };
    GetCurrentDirectory(sizeof(directory), directory);
    std::string currentDir = directory;
#endif
return currentDir;
}
bool ShowHomeDirectory(std::string&currentDir){
    if(ImGui::BeginTable("常用路径", 1)){
        ImGui::TableSetupColumn("主目录");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        // ImGui::TextUnformatted(r);
        // ImGui::Button(r);
#ifdef __linux
        std::string homePaht = getenv("HOME");
        std::vector<std::string>recentPath = { homePaht, homePaht + "/Desktop", homePaht + "/Documents", homePaht + "/Downloads", homePaht + "/Music", homePaht + "/Pictures", homePaht + "/Videos" };
#endif
#ifdef WIN32
        std::string homePaht = getenv("USERPROFILE");
        std::vector<std::string>recentPath = { homePaht, homePaht + "\\Desktop", homePaht + "\\Documents", homePaht + "\\Downloads", homePaht + "\\Music", homePaht + "\\Pictures", homePaht + "\\Videos" };
#endif
        for (size_t i = 0; i < recentPath.size(); ++i){
            if(ImGui::Selectable(recentPath[i].c_str())){
                currentDir = recentPath[i];

                ImGui::EndTable();
                return true;
            }
        }
        ImGui::EndTable();
    }
    return false;
}
void ShowFolderDirectory(const std::string&currentDir, std::vector<std::string>&subDir){
    getSubdirectory(currentDir, subDir);
    if(ImGui::BeginTable("各个文件路径名", subDir.size())){
        for (size_t i = 0; i < subDir.size(); ++i){
            ImGui::TableSetupColumn(subDir[i].c_str());
        }
        ImGui::TableHeadersRow();
        ImGui::EndTable();
    }
}
bool ShowFolderAndFile(const char *const *items, int fileTypeIndex, std::string&currentDir, std::vector<std::string>&subDir, std::string&selectedFile){
    if(ImGui::Begin("文件名窗口", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_ChildWindow)){
        ImGui::SetWindowSize(ImVec2(400, 200));
        if(ImGui::BeginTable("文件名", 1)){
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            std::vector<std::string>file;
            std::vector<std::string>folder;
            getFileFromDirectory(currentDir, folder, file);
            // ImGui::TextUnformatted(r);
            for (size_t i = 0; i < folder.size(); ++i){
#ifdef __linux
                if(ImGui::Selectable((folder[i] + '/').c_str())){
#endif
#ifdef WIN32
                if (ImGui::Selectable((folder[i] + '\\').c_str())) {
#endif // WIN32

                    subDir.push_back(folder[i]);
                    splicingDirectory(subDir, currentDir);

                    ImGui::EndTable();
                    ImGui::EndChild();
                    return true;
                }
            }
            if(fileTypeIndex){//指定栏类型
                for (size_t i = 0; i < file.size(); ++i){
                    std::size_t pos = file[i].find('.');
                    std::size_t tPos = file[i].find('.', pos + 1);
                    while(tPos < file[i].length()){
                        pos = tPos;
                        tPos = file[i].find('.', tPos + 1);
                    }
                    if(pos < file[i].length()){
                        std::string s = file[i].substr(pos);
                        if(s == items[fileTypeIndex] + 1){
                            if(ImGui::Selectable(file[i].c_str())){
                                selectedFile = currentDir + '/' + file[i];
                            }
                        }
                    }
                }
            }
            else{
                for (size_t i = 0; i < file.size(); ++i){
                    if(ImGui::Selectable(file[i].c_str())){
#ifdef __linux                        
                        selectedFile = currentDir + '/' + file[i];
#endif
#ifdef WIN32
                        selectedFile = currentDir + '\\' + file[i];
#endif
                    }
                }
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
    }
    return false;
}
IMGUI_BUTTON_IDENTIFIER ShowImguiGetFileButton(const char *ok_button, const char *cancel_button = "取消"){
    ImVec2 windowSize = ImGui::GetWindowSize();    
    ImGui::SetCursorPos(ImVec2(0, windowSize.y - 40));
    if(ImGui::Button(cancel_button)){
        return IMGUI_BUTTON_CANCEL;
    }
    ImGui::SetCursorPos(ImVec2(40, windowSize.y - 40));
    if(ImGui::Button(ok_button)){
        return IMGUI_BUTTON_OK;
    }
    return IMGUI_BUTTON_UNKNOW;
}
bool ShowFileInfomation(const char *const *items, int32_t fileTypeIndex, std::vector<std::string>&subDir, std::string&currentDir, std::string&selectedFile){
    if(ImGui::BeginTable("表_对齐", 2)){
        ImGui::TableNextColumn();
        if(ShowHomeDirectory(currentDir)){
            ImGui::EndTable();
            return true;
        }
        ImGui::TableNextColumn();
        ShowFolderDirectory(currentDir, subDir);
        if(ShowFolderAndFile(items, fileTypeIndex, currentDir, subDir, selectedFile)){
            ImGui::EndTable();
            return true;
        }
        ImGui::EndTable();
    }
    return true;
}
void ShowFileType(const char *const *items, int32_t items_count, int32_t *fileTypeIndex, std::vector<std::string>&subDir, std::string&currentDir){
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImGui::SetCursorPos(ImVec2(0, windowSize.y - 70));
    if(ImGui::BeginTable("返回上级_类型_表", 2)){
        ImGui::TableNextColumn();
        if(ImGui::Button("返回上级")){
            if(subDir.size() > 1){
                subDir.pop_back();
                splicingDirectory(subDir, currentDir);
            }
        }
        ImGui::TableNextColumn();ImGui::Combo("类型", fileTypeIndex, items, items_count);
        ImGui::EndTable();
    }
}
bool ShowOpenFileUI(const char *const *items, int items_count, std::string&result){
    // bool continueShow = true;
    if(!ImGui::Begin("打开"/*, nullptr, ImGuiWindowFlags_NoResize*/)){
        ImGui::End();
        return true;
    }
    static int fileTypeIndex = items_count - 1;
    std::vector<std::string> subDir;
    static std::string selectedFile;
    static std::string currentDir = GetCurrentDirectory();

    ShowFileInfomation(items, fileTypeIndex, subDir, currentDir, selectedFile);
    
    ImGui::Text(selectedFile.c_str());

    ShowFileType(items, items_count, &fileTypeIndex, subDir, currentDir);

    IMGUI_BUTTON_IDENTIFIER buttonID = ShowImguiGetFileButton("打开");
    if(buttonID == IMGUI_BUTTON_CANCEL){
        selectedFile = "";
        ImGui::End();
        return false;
    }
    else if(buttonID == IMGUI_BUTTON_OK){
        if(selectedFile != ""){
            result = selectedFile;
            selectedFile = "";
            ImGui::End();
            return false;
        }
        else{
            result = currentDir;
            selectedFile = "";
            ImGui::End();
            return false;
        }
    }
    ImGui::End();
    return true;
}
bool ShowOpenFolderUI(const char *const *items, int items_count, std::string&result){
    if(!ImGui::Begin("打开"/*, nullptr, ImGuiWindowFlags_NoResize*/)){
        ImGui::End();
        return true;
    }
    static int fileTypeIndex = items_count - 1;
    std::vector<std::string> subDir;
    static std::string selectedFile;
    static std::string currentDir = GetCurrentDirectory();

    ShowFileInfomation(items, fileTypeIndex, subDir, currentDir, selectedFile);

    static char fileName[1000] = "/";
    if((selectedFile == "" || selectedFile.c_str() != currentDir) && !strchr(selectedFile.c_str(), '.'))
        selectedFile = currentDir;
    memset(fileName, 0, sizeof(fileName));
    memcpy(fileName, selectedFile.c_str(), sizeof(char) * selectedFile.size());
    if(ImGui::InputText("选择保存的文件或文件夹", fileName, 999)){
        selectedFile = fileName;
    }
    
    ShowFileType(items, items_count, &fileTypeIndex, subDir, currentDir);

    IMGUI_BUTTON_IDENTIFIER buttonID = ShowImguiGetFileButton("保存");
    if(buttonID == IMGUI_BUTTON_CANCEL){
        memset(fileName, 0, sizeof(fileName));
        selectedFile = "";
        ImGui::End();
        return false;
    }
    else if(buttonID == IMGUI_BUTTON_OK){
        if(selectedFile != ""){
            result = selectedFile;
            memset(fileName, 0, sizeof(fileName));
            selectedFile = "";
            ImGui::End();
            return false;
        }
        else{
            result = currentDir;
            memset(fileName, 0, sizeof(fileName));
            selectedFile = "";
            ImGui::End();
            return false;
        }
    }
    ImGui::End();
    return true;
}
void CreateClient(const char *serverIp){
    if(g_OnLine.CreateClient(serverIp)){
        CreateThread(process_client, nullptr);
        g_OnLine.JoinsGame();
    }
}
void CreateServer(){
    static const uint32_t countryCount = g_Game.GetCountryCount();
    if(g_OnLine.CeateServer(countryCount)){
        //必须用静态变量, 局部变量在函数结束后就销毁, 会导致传餐错误
        CreateThread(server_start, nullptr);
        CreateClient(g_OnLine.GetLocalIp().c_str());
    }
}
void catspace(char *dst, uint32_t count){
    for (size_t i = 0; i < count; ++i){
        strcat(dst, " ");
    }
}
void ShowPlayerCountryCombo(uint32_t comboIndex, const char *country){
    char comboName[MAX_BYTE] = " ";
    const uint32_t clientIndex = g_OnLine.GetClientIndex();
    static const auto aiClientIndex = g_OnLine.GetAiClientIndex();
    catspace(comboName, comboIndex);
    if(!g_Game.IsGameStart() && (comboIndex == clientIndex || (g_OnLine.IsServer() && (comboIndex == aiClientIndex[0] || comboIndex == aiClientIndex[1])))){
        static std::string currentCountryItem = g_CountryItems[comboIndex][0];
        if(ImGui::BeginCombo(comboName, currentCountryItem.c_str())){
            for (auto countryIt = g_CountryItems[comboIndex].begin(); countryIt != g_CountryItems[comboIndex].end(); ++countryIt){
                bool is_selected = currentCountryItem == *countryIt;
                if (ImGui::Selectable(countryIt->c_str(), is_selected)){
                    currentCountryItem = *countryIt;
                    printf("玩家更改势力, 新势力为%s\n", currentCountryItem.c_str());
                    g_OnLine.SendCountryInfomation(comboIndex, currentCountryItem.c_str());
                    break;
                }
            }
            ImGui::EndCombo();
        }
    }
    else{
        //这里只给看不给改
        if(ImGui::BeginCombo(comboName, country)){
            ImGui::EndCombo();
        }
    }
}
void *ReplayFun(void *userData){
    g_Game.EnableReplay();
    std::vector<ChessMove>record = *(std::vector<ChessMove> *)userData;
    auto pBoard = g_Game.GetChessboard();
    for (auto&it:record){
        Chess *pStart = pBoard->GetChess(it.chess.GetRow(), it.chess.GetColumn());
        if(!pBoard->IsBoundary(it.chess.GetRow(), it.chess.GetColumn())){
            g_Game.PlayChess(pStart, it.captured.GetRow(), it.captured.GetColumn(), g_ImGuiInput.skipReplay);
            g_Ai.SyncBoard(pStart, it.captured.GetRow(), it.captured.GetColumn());
            // auto move = g_Game.GetSaveStep(pBoard, pStart, it.captured.GetRow(), it.captured.GetColumn());
            // pStart->SetPos(it.captured.GetRow(), it.captured.GetColumn());
            // pBoard->SaveStep(move);
        }
    }
    g_Game.UpdateUniform(g_VulkanDevice.device, g_WindowWidth);
    g_ImGuiInput.skipReplay = false;
    g_Game.EndReplay();
    return nullptr;
}
void ReadChessFromFile(FILE *fp, Chess&chess){
    Chess::Type c;
    Country country;
    uint32_t row, column, chessOffset;
    fread(&c, sizeof(c), 1, fp);
    fread(&row, sizeof(row), 1, fp);
    fread(&column, sizeof(column), 1, fp);
    fread(&country, sizeof(country), 1, fp);
    fread(&chessOffset, sizeof(chessOffset), 1, fp);
    chess.SetChess(c);
    chess.SetCountry(country);
    chess.SetPos(row, column);
    chess.SetChessOffset(chessOffset);
}
void LoadReplay(const std::string &file){
    //还需要实现ImportRecord
    FILE *fp = fopen(file.c_str(), "rb");
    if(!fp){
        perror("read file error");
        printf("file is %s\n", file.c_str());
        return;
    }
    Country player;
    ChessMove move;
    uint32_t recordCount;
    static std::vector<ChessMove>record;
    record.clear();
    fread(&player, sizeof(player), 1, fp);
    fread(&recordCount, sizeof(recordCount), 1, fp);
    for (size_t i = 0; i < recordCount; i++){
        fread(&move.is_facing, sizeof(move.is_facing), 1, fp);
        fread(&move.is_death, sizeof(move.is_death), 1, fp);
        fread(&move.is_check, sizeof(move.is_check), 1, fp);
        fread(&move.is_capture, sizeof(move.is_capture), 1, fp);
        fread(move.notation, sizeof(move.notation), 1, fp);
        fread(&move.number, sizeof(move.number), 1, fp);
        ReadChessFromFile(fp, move.chess);
        ReadChessFromFile(fp, move.captured);
        if(move.is_death){
            fread(&move.death.country, sizeof(move.death.country), 1, fp);
            for (auto&chess:move.death.chess){
                ReadChessFromFile(fp, chess);
            }
        }
        if(move.is_facing){
            for (size_t i = 0; i < move.facing.size(); ++i){
                fread(&move.facing[i].country, sizeof(move.facing[i].country), 1, fp);
                for (auto&chess:move.facing[i].chess){
                    ReadChessFromFile(fp, chess);
                }
            } 
        }
        record.push_back(move);
    }
    fclose(fp);
    NewGame(player);
    CreateThread(ReplayFun, &record);
}
void WriteChessToFile(FILE *fp, const Chess&chess){
    Chess::Type c = chess.GetChess();
    uint32_t row = chess.GetRow();
    uint32_t column = chess.GetColumn();
    Country country = chess.GetCountry();
    uint32_t chessOffset = chess.GetChessOffset();
    fwrite(&c, sizeof(c), 1, fp);
    fwrite(&row, sizeof(row), 1, fp);
    fwrite(&column, sizeof(column), 1, fp);
    fwrite(&country, sizeof(country), 1, fp);
    fwrite(&chessOffset, sizeof(chessOffset), 1, fp);
}
void SaveReplay(const std::string &file){
  FILE *fp = fopen(file.c_str(), "wb");
  if(!fp){
    perror("write file error");
    printf("file is %s\n", file.c_str());
    return;
  }
  Country country = g_Game.GetPlayer();
  fwrite(&country, sizeof(country), 1, fp);
   auto pBoard = g_Game.GetChessboard();
   uint32_t recordCount = pBoard->GetRecordSize();
  fwrite(&recordCount, sizeof(recordCount), 1, fp);
   for (auto it = pBoard->RecordBegin(); it != pBoard->RecordEnd(); ++it){
        fwrite(&it->is_facing, sizeof(it->is_facing), 1, fp);
        fwrite(&it->is_death, sizeof(it->is_death), 1, fp);
        fwrite(&it->is_check, sizeof(it->is_check), 1, fp);
        fwrite(&it->is_capture, sizeof(it->is_capture), 1, fp);
        fwrite(it->notation, sizeof(it->notation), 1, fp);
        fwrite(&it->number, sizeof(it->number), 1, fp);
        WriteChessToFile(fp, it->chess);
        WriteChessToFile(fp, it->captured);
        if(it->is_death){
            fwrite(&it->death.country, sizeof(it->death.country), 1, fp);
            for (auto&chess:it->death.chess){
                WriteChessToFile(fp, chess);
            }            
        }
        if(it->is_facing){
            for (size_t i = 0; i < it->facing.size(); ++i){
                fwrite(&it->facing[i].country, sizeof(it->facing[i].country), 1, fp);
                for (auto&chess:it->facing[i].chess){
                    WriteChessToFile(fp, chess);
                }
            } 
        }
   }
   fclose(fp);
}
void ShowGameWidget(uint32_t player){
    const char *countryName[MAX_COUNTRY_INDEX];
    countryName[Wu_Country] = "吴";
    countryName[Wei_Country] = "魏";
    countryName[Shu_Country] = "蜀";
    countryName[Han_Country] = "汉";
    const uint32_t currentCountry = g_Game.GetCurrentCountry();
    if(currentCountry != Invald_Country && player != Invald_Country){
        ImGui::Text("你是%s, 该%s下棋\n", countryName[player], player == currentCountry?"你":countryName[currentCountry]);
    }
    if(ImGui::Checkbox("四国", &g_ImGuiInput.enableHan)){
        if(g_ImGuiInput.enableHan){
            g_Game.EnableHan();
        }
        else{
            g_Game.DiscardHan();
        }
        ResetCountryItem(g_CountryItems[0]);
    }
    if(g_Game.IsGameStart()){
        if(g_Game.IsReplay()){
            if(ImGui::Button("跳过推演")){
                g_ImGuiInput.skipReplay = true;
            }
        }
        else{
            if(ImGui::BeginTable("检查框列表", 2)){
                ImGui::TableNextColumn();
                if(ImGui::Checkbox("启用AI", &g_ImGuiInput.enableAi)){
                    if(g_ImGuiInput.enableAi){
                        if(!g_Ai.IsEnd()){
                            g_Ai.End();
                            g_Ai.WaitThread();
                        }
                        g_Ai.CreatePthread();
                    }
                    else{
                        g_Ai.End();
                    }
                }
                ImGui::TableNextColumn();
                if(ImGui::Checkbox("托管", &g_ImGuiInput.enableAutoPlay)){
                    if(g_ImGuiInput.enableAutoPlay && currentCountry == player){
                        g_Ai.Enable();
                    }
                }
                ImGui::EndTable();
            }
            if(ImGui::Button("保存对局")){
                g_ImGuiInput.interface.bOpenFolder = true;
                g_OpenFileFunCall = SaveReplay;
                g_FileTypeItem.push_back("*.*");
                g_FileTypeItem.push_back(REPLAY_FILE_TYP);
            }
        }
    }
    if(ImGui::Button("详细步骤")){
        g_ImGuiInput.interface.bReplayInfo = true;
    }
    if(g_Game.IsOnline()){
        if(ImGui::Button("返回主菜单")){
            g_OnLine.ExitGame();
        }    
    }
}
void ShowClientWidget(){
    if(g_Game.IsGameStart()){
        ShowGameWidget(g_OnLine.GetClientIndex());
    }
    if(ImGui::BeginTable("player information", 2)){
        ImGui::TableNextColumn();
        ImGui::Text("名字");
        for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
            if (strcmp(it->name, ""))
                ImGui::Text(it->name);
        }
        ImGui::TableNextColumn();
        ImGui::Text("势力");
        uint32_t comboIndex = 0;
        //combo(或许其他也是)需要靠label查找。所以label不能重复
        for (auto it = g_Players.begin(); it != g_Players.end(); ++it, ++comboIndex){
            if (strcmp(it->name, "")){
                ShowPlayerCountryCombo(comboIndex, it->country);
            }
        }
        ImGui::EndTable();
    }    
}
uint32_t GetAiCount(){
    uint32_t aiCount = 0;
    for (auto it = g_Players.begin(); it != g_Players.end(); ++it){
        if(it->ai){
            ++aiCount;
            break;
        }
    }
    return aiCount;
}
void ShowServerWidget(){
    //在非广播模式下，直接显示本机ip。(或许广播模式下也可以显示)
    if(!g_Game.IsGameStart()){
        static std::string localIp = g_OnLine.GetLocalIp();
        ImGui::Text("本机ip:%s", localIp.c_str());    
            if(ImGui::Button("添加电脑")){
            const uint32_t aiCount = GetAiCount();
            //减掉一个服务端玩家和一个客户端玩家
            if(aiCount < g_Game.GetCountryCount() - 2)g_OnLine.AddAi(localIp.c_str());
        }
    }
    ShowClientWidget();
}
bool ShowConnectServerInterface(){
    bool show = true;
    if(ImGui::Begin("连接服务端")){
        static char serverIp[MAX_BYTE] = {0};
        ImGui::InputText("服务端ip", serverIp, sizeof(serverIp));
        if(ImGui::BeginTable("确定取消按钮", 3)){
            ImGui::TableNextColumn();
            if(ImGui::Button("确定")){
                show = false;
                CreateClient(serverIp);
            }
            ImGui::TableNextColumn();
            if(ImGui::Button("使用本机ip连接")){
                strcpy(serverIp, g_OnLine.GetLocalIp().c_str());
                // show = false;
                // CreateClient(g_ServerIp);
            }
            ImGui::TableNextColumn();
            if(ImGui::Button("取消")){
                show = false;
            }
            ImGui::EndTable();
        }
        ImGui::End();
    }
    return show;
}
bool ShowOnlineModeMainInterface(bool *mainInterface){
    bool ret = false;
    static bool bShowConnectServer;
    if(ImGui::Begin("局域网联机")){
        if(g_OnLine.IsServer()){
            ShowServerWidget();
        }
        else{
            ShowClientWidget();
        }
        if(!g_OnLine.IsServer()){
            static bool enableHan;
            if(ImGui::Checkbox("四国", &enableHan)){
                if(enableHan){
                    g_Game.EnableHan();
                }
                else{
                    g_Game.DiscardHan();
                }
            }
            //如果创建好房间或连接了客户端后, 不应该继续显示下面的按钮
            if(ImGui::BeginTable("客户端信息", 2)){
                ImGui::TableNextColumn();
                if(ImGui::Button("创建服务端")){
                    CreateServer();
                }
                ImGui::TableNextColumn();
                if(ImGui::Button("连接服务端")){
                    bShowConnectServer = true;
                }
                ImGui::EndTable();
            }
        }
        if(ImGui::Button("返回")){
            ret = true;
            *mainInterface = true;
            if(g_Game.IsGameStart())g_Game.EndGame();
        }
    }
    ImGui::End();
    if(bShowConnectServer){
        bShowConnectServer = ShowConnectServerInterface();
    }
    return !ret;
}
bool ShowSinglePlayerModeMainInterface(bool *mainInterface){
    bool ret = false;
    const Country player = g_Game.GetPlayer();
    if(ImGui::Begin("单人模式")){
        if(ImGui::Button("新游戏")){
            NewGame();
        }
        ShowGameWidget(player);
        ShowPlayerCountryCombo();
        if(ImGui::Button("返回")){
            *mainInterface = true;
            ret = true;
            g_Ai.End();
            g_ImGuiInput.Init();
            if(g_Game.IsGameStart())g_Game.EndGame();
        }
    }
    ImGui::End();
    return !ret;
}
bool ShowRecordInterface(bool *mainInterface, bool *bSinglePlayer){
    bool bshow = true;
    if(!g_Game.IsGameStart()){
        if(ImGui::Begin("回放")){
            if(ImGui::BeginTable("回放界面按钮", 1)){
                ImGui::TableNextColumn();
                if(ImGui::Button("加载棋谱")){
                    *bSinglePlayer = true;
                    g_ImGuiInput.interface.bOpenFile = true;
                    g_OpenFileFunCall = LoadReplay;
                    g_FileTypeItem.push_back("*.*");
                    g_FileTypeItem.push_back(REPLAY_FILE_TYP);
                }
                ImGui::EndTable();
            }
            if(ImGui::Button("返回")){
                bshow = false;
                *mainInterface = true;
            }
        }
        ImGui::End();
    }
    return bshow;
}
bool MainInterface(bool *bSinglePlayer, bool *bOnline, bool *bReplay){
    if(ImGui::Begin("主界面")){
        if(ImGui::BeginTable("主界面-按钮表格", 2)){
            ImGui::TableNextColumn();
            if(ImGui::Button("单人")){
                *bSinglePlayer = true;
            }
            ImGui::TableNextColumn();
            if(ImGui::Button("局域网联机")){
                *bOnline = true;
            }
            ImGui::EndTable();
        }
        if(ImGui::Button("回放")){
            *bReplay = true;
        }
    }
    ImGui::End();
    return !*bSinglePlayer && !*bOnline && !*bReplay;
}
bool ShowReplayInof(bool *mainInterface){
    bool bshow = true;
    if(ImGui::Begin("详细步骤")){
        int32_t stepNumber = -1;
        auto pBoard = g_Game.GetChessboard();
        if(ImGui::BeginTable("检查框列表", g_Game.GetCountryCount() + 2)){
            ImGui::TableNextColumn();
            ImGui::Text("回合");
            uint32_t round = 1;
            for (auto it = pBoard->RecordBegin(); it != pBoard->RecordEnd(); it += g_Game.GetCountryCount()) {
                ImGui::Text("%d", round++);
                if(round > (pBoard->GetRecordSize() / g_Game.GetCountryCount()) + 1)break;
            }
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "吴");
            for (auto it = pBoard->RecordBegin(); it != pBoard->RecordEnd(); ++it) {
                if (it->chess.GetCountry() == Wu_Country) {
                    if (ImGui::Selectable(it->step)) {
                        const char *str = strstr(it->step, "_") ;
                        g_Game.SetCurrentCountry(g_Game.GetNextCountry(Wu_Country));
                        if(str)stepNumber = atoi(str + 1);
                        break;
                    }
                }
            }
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 0, 1, 1), "魏");
            for (auto it = pBoard->RecordBegin(); it != pBoard->RecordEnd(); ++it){
                if(it->chess.GetCountry() == Wei_Country){
                    if(ImGui::Selectable(it->step)){
                        const char *str = strstr(it->step, "_") ;
                        g_Game.SetCurrentCountry(g_Game.GetNextCountry(Wei_Country));
                        if(str)stepNumber = atoi(str + 1);
                        break;
                    }
                }
            }
            if(g_Game.IsControllable()){
                ImGui::TableNextColumn();
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "汉");
                for (auto it = pBoard->RecordBegin(); it != pBoard->RecordEnd(); ++it) {
                    if (it->chess.GetCountry() == Han_Country) {
                        if (ImGui::Selectable(it->step)) {
                            const char *str = strstr(it->step, "_") ;
                            g_Game.SetCurrentCountry(g_Game.GetNextCountry(Han_Country));
                            if(str)stepNumber = atoi(str + 1);
                            break;
                        }
                    }
                }    
            }
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "蜀");
            for (auto it = pBoard->RecordBegin(); it != pBoard->RecordEnd(); ++it){
                if(it->chess.GetCountry() == Shu_Country){
                    if(ImGui::Selectable(it->step)){
                        const char *str = strstr(it->step, "_") ;
                        g_Game.SetCurrentCountry(g_Game.GetNextCountry(Shu_Country));
                        if(str)stepNumber = atoi(str + 1);
                        break;
                    }
                }
            }
            ImGui::TableNextColumn();
            ImGui::Text("备注");
            for (auto it = pBoard->RecordBegin(); it != pBoard->RecordEnd();){
                auto temp = it; 
                char notation[300] = {0};
                for (size_t i = 0; i < g_Game.GetCountryCount() && temp != pBoard->RecordEnd(); i++, ++temp){
                    if(strcmp(temp->notation, "")){
                        strcat(strcat(notation, temp->notation),";");
                    }
                }
                ImGui::Text(notation);
                for (size_t i = 1; i < g_Game.GetCountryCount() + 1; i++){
                    if(it + i == pBoard->RecordEnd()){
                        it = pBoard->RecordEnd();
                        break;
                    }
                }
                if(it != pBoard->RecordEnd())it += g_Game.GetCountryCount();
            }
            ImGui::EndTable();
        }
        if(ImGui::Button("关闭")){
            bshow = false;
            *mainInterface = true;
        }
        if(stepNumber != -1){            
            g_Game.SetStepNumber(stepNumber);
            g_Game.UpdateUniform(g_VulkanDevice.device, g_WindowWidth);
        }
    }
    ImGui::End();
    return bshow;
}
void UpdateImgui(VkCommandBuffer command){
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    if(g_ImGuiInput.interface.bMain){
        g_ImGuiInput.interface.bMain = MainInterface(&g_ImGuiInput.interface.bSinglePlayer, &g_ImGuiInput.interface.bOnline, &g_ImGuiInput.interface.bReplay);
    }
    else if(g_ImGuiInput.interface.bSinglePlayer || g_Game.IsGameStart()){
        g_ImGuiInput.interface.bSinglePlayer = ShowSinglePlayerModeMainInterface(&g_ImGuiInput.interface.bMain);
    }
    else if(g_ImGuiInput.interface.bOnline){
        g_ImGuiInput.interface.bOnline = ShowOnlineModeMainInterface(&g_ImGuiInput.interface.bMain);
    }
    else if (g_ImGuiInput.interface.bReplay){
        g_ImGuiInput.interface.bReplay = ShowRecordInterface(&g_ImGuiInput.interface.bMain, &g_ImGuiInput.interface.bSinglePlayer);
    }

    if(g_ImGuiInput.interface.bReplayInfo){
        g_ImGuiInput.interface.bReplayInfo = ShowReplayInof(&g_ImGuiInput.interface.bMain);
    }
    if(g_ImGuiInput.interface.bOpenFile){
        std::string file = "";
        g_ImGuiInput.interface.bOpenFile = ShowOpenFileUI(g_FileTypeItem.data(), g_FileTypeItem.size(), file);
        if(!g_ImGuiInput.interface.bOpenFile){
            g_FileTypeItem.clear();
        }
        if(file != "" && g_OpenFileFunCall){
            g_OpenFileFunCall(file);
            g_FileTypeItem.clear();
        }
    }
    if(g_ImGuiInput.interface.bOpenFolder){
        std::string file = "";
        g_ImGuiInput.interface.bOpenFolder = ShowOpenFolderUI(g_FileTypeItem.data(), g_FileTypeItem.size(), file);
        if(!g_ImGuiInput.interface.bOpenFolder){
            g_FileTypeItem.clear();
        }
        if(file != "" && g_OpenFileFunCall){
            g_OpenFileFunCall(file);
            g_FileTypeItem.clear();
        }        
    }

    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    const bool isMinimized = (draw_data->DisplaySize.x <=.0f || draw_data->DisplaySize.y <= .0f);
    if(!isMinimized)g_VulkanImGui.RenderDrawData(command, draw_data);
}
void RecordCommand(VkCommandBuffer command, VkFramebuffer frame){
    vulkanFrame::BeginCommands(command, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
    vulkanFrame::BeginRenderPass(command, frame, g_VulkanWindow.renderPass, g_WindowWidth, g_WindowHeight);

    const glm::mat4 projection = glm::ortho(0.0f, (float)g_WindowWidth, 0.0f, (float)g_WindowHeight, -1.0f, 1.0f);
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, g_Fill);
    vkCmdPushConstants(command, g_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &projection);

    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, g_Fill);
    g_Game.Draw(command, g_PipelineLayout);
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, g_Wireframe);
    g_Game.DrawWireframe(command, g_PipelineLayout);
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, g_Font);
    g_Game.DrawFont(command, g_PipelineLayout);

    UpdateImgui(command);

    vkCmdEndRenderPass(command);
    vkEndCommandBuffer(command);
}
void keybutton(GLFWwindow *window, int key, int scancode, int action, int mods){
    // 必须是玩家回合才能撤销。不然就要处理很多同步问题//撤销功能使用后，需要注意当前下棋的玩家,也就是说，撤回后，需要正确修改当前下棋的国籍
    if(action == GLFW_RELEASE){
        if(key == GLFW_KEY_Z){
            if(GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) || GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)){
                if(g_Game.IsOnline()){
                    //联机模式需要所有人类投票决定
                }
                // //为了测试
                // else if(g_Game.GetCurrentCountry() == g_Game.GetPlayer() || !g_ImGuiInput.enableAi){
                //     auto pBoard = g_Game.GetChessboard();
                //     const uint32_t step = 1;
                //     g_Ai.UndoStep(step);
                //     pBoard->UndoStep(step);
                //     for (size_t i = 0; i < step; i++){
                //         g_Game.LastCountry();//每退一步，下棋的玩家也需要跟着退一步
                //     }
                //     g_Game.UpdateUniform(g_VulkanDevice.device, g_WindowWidth);
                // }
            }
        }
    }
}
void *PlayChessFun(void *userData){
    glm::vec4 info = *(glm::vec4 *)userData;
    auto pBoard = g_Game.GetChessboard();
    Chess *pStart = pBoard->GetChess(info.y, info.x);
    g_Game.PlayChess(pStart, info.w, info.z);
    //之所以没开ai也要同步的原因, 就是怕玩家突然开ai
    g_Ai.SyncBoard(pStart, info.w, info.z);
    if(g_ImGuiInput.enableAi){
        if(g_Game.GameOver()){
            g_Ai.End();
        }
        g_Ai.EnableNextCountry(g_ImGuiInput.enableAutoPlay);
    }
    return nullptr;
}
const Chess *g_Select;
bool SelectChess(const glm::vec2 &mousePos){
    auto pBoard = g_Game.GetChessboard();
    const Country currentCountry = g_Game.GetCurrentCountry();
    if((!g_Ai.IsEnd() || g_Game.IsOnline()) && currentCountry != g_Game.GetPlayer())return false;
    if(g_Select){
        g_Game.UnSelectChess();
        static glm::vec4 info;
        info = g_Game.PrepareChess(g_Select, mousePos);
        if(info.x == 0 && info.y == 0){
            g_Select = nullptr;
            return false;
        }
        if(g_Game.IsOnline()){
            //单机和局域网联机差不多。无非就是要接收和发送消息
            Chess target;
            const Chess *pTarget = pBoard->GetChess(info.w, info.z);
            if(pTarget){
                target = *pTarget;
            }
            else{
                target.SetPos(info.w, info.z);
                target.SetCountry(Invald_Country);
            }
            const uint32_t clientIndex = g_OnLine.GetClientIndex();
            g_OnLine.SendPlayChessMessage(g_Players[clientIndex], g_Select, &target);
        }
        else{
            g_Game.UnSelectChess();
            //得另开线程才看到棋子移动效果
            CreateThread(PlayChessFun, &info);
        }
        g_Select = nullptr;
    }
    else{
        g_Select = pBoard->GetChess(currentCountry, mousePos);
    }
    return true;
}
void mousebutton(GLFWwindow *window,int button,int action,int mods){
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    const glm::vec2 mousePos = glm::vec2(xpos, ypos);
    if (g_Game.IsGameStart() && action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT){
        SelectChess(mousePos);
        g_Game.SelectChess(g_Select);
    }
}
VkVertexInputBindingDescription inputBindingDescription(uint32_t binding) {
    return VkVertexInputBindingDescription({ binding, sizeof(glm::vec3) * 2, VK_VERTEX_INPUT_RATE_VERTEX });
}
VkVertexInputAttributeDescription inputAttributeDescription(uint32_t location, VkFormat format, uint32_t offset, uint32_t binding = 0){
    VkVertexInputAttributeDescription info;
    info.offset = offset;
    info.format = format;
    info.binding = binding;
    info.location = location;
    return info;
}
static std::array<VkVertexInputAttributeDescription, 2> inputAttributeDescriptions(uint32_t binding) {
    return { inputAttributeDescription(0, VK_FORMAT_R32G32B32_SFLOAT, 0, binding), inputAttributeDescription(1, VK_FORMAT_R32G32B32_SFLOAT, sizeof(glm::vec3), binding) };
}
void CreatePipelineLayout(VkDevice device, VkDescriptorSetLayout layout){
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = Pipeline::initializers::pipelineLayoutCreateInfo(1, 1);
    VkPushConstantRange pushConstantRange = Pipeline::initializers::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), 0);
    pipelineLayoutInfo.pSetLayouts = &layout;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &g_PipelineLayout));
}
void DestroyGraphicsPipeline(VkDevice device){
    vkDestroyPipeline(device, g_Fill, VK_NULL_HANDLE);
    vkDestroyPipeline(device, g_Font, VK_NULL_HANDLE);
    vkDestroyPipeline(device, g_Wireframe, VK_NULL_HANDLE);
    vkDestroyPipelineLayout(device, g_PipelineLayout, VK_NULL_HANDLE);
}
void CreateGraphicsPipeline(VkDevice device, VkPipelineLayout layout){
    VkPipelineVertexInputStateCreateInfo inputState{};
    std::array<VkPipelineShaderStageCreateInfo,2> shaderStages;
    VkVertexInputBindingDescription bindingDescriptions = inputBindingDescription(0);
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = inputAttributeDescriptions(0);

    VkPipelineViewportStateCreateInfo viewportState = Pipeline::initializers::pipelineViewportStateCreateInfo(1, 1, 0);
    VkGraphicsPipelineCreateInfo pipelineCreateInfo = Pipeline::initializers::pipelineCreateInfo(layout, g_VulkanWindow.renderPass, 0);

    VkPipelineColorBlendAttachmentState blendAttachmentState = Pipeline::initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
    VkPipelineColorBlendStateCreateInfo colorBlendState = Pipeline::initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
    VkPipelineMultisampleStateCreateInfo multisampleState = Pipeline::initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
    VkPipelineDepthStencilStateCreateInfo depthStencilState = Pipeline::initializers::pipelineDepthStencilStateCreateInfo(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = Pipeline::initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
    VkPipelineRasterizationStateCreateInfo rasterizationState = Pipeline::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, 0);
    // std::array<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    // VkPipelineDynamicStateCreateInfo dynamicState = Pipeline::initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables.data(), static_cast<uint32_t>(dynamicStateEnables.size()), 0);
    VkRect2D scissors = Pipeline::Rect2D(g_WindowWidth, g_WindowHeight);
    VkViewport viewport = Pipeline::Viewport(g_WindowWidth, g_WindowHeight);
    viewportState.pScissors = &scissors;
    viewportState.pViewports = &viewport;

    inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    inputState.vertexBindingDescriptionCount = 1;
    inputState.pVertexBindingDescriptions = &bindingDescriptions;
    inputState.pVertexAttributeDescriptions = attributeDescriptions.data();
    inputState.vertexAttributeDescriptionCount = attributeDescriptions.size();

    shaderStages[0] = Pipeline::LoadShader(device, "shaders/color.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = Pipeline::LoadShader(device, "shaders/color.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    // pipelineCreateInfo.pTessellationState = ;
    pipelineCreateInfo.layout = layout;
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.stageCount = shaderStages.size();

    pipelineCreateInfo.pVertexInputState = &inputState;
    pipelineCreateInfo.pViewportState = &viewportState;
    // pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    VK_CHECK(vkCreateGraphicsPipelines(device, g_PipelineCache, 1, &pipelineCreateInfo, nullptr, &g_Fill));
    rasterizationState = Pipeline::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_LINE, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, 0);
    VK_CHECK(vkCreateGraphicsPipelines(device, g_PipelineCache, 1, &pipelineCreateInfo, nullptr, &g_Wireframe));
    vkDestroyShaderModule(device, shaderStages[0].module, VK_NULL_HANDLE);
    vkDestroyShaderModule(device, shaderStages[1].module, VK_NULL_HANDLE);
    rasterizationState = Pipeline::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, 0);
    shaderStages[0] = Pipeline::LoadShader(device, "shaders/font.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = Pipeline::LoadShader(device, "shaders/font.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
    VK_CHECK(vkCreateGraphicsPipelines(device, g_PipelineCache, 1, &pipelineCreateInfo, nullptr, &g_Font));
    vkDestroyShaderModule(device, shaderStages[0].module, VK_NULL_HANDLE);
    vkDestroyShaderModule(device, shaderStages[1].module, VK_NULL_HANDLE);
}
void SetupDescriptorSetLayout(VkDevice device){
    VkDescriptorSetLayoutBinding setlayoutBindings[2] = {};
    // setlayoutBinding.binding = 0;
    setlayoutBindings[0].descriptorCount = 1;
    setlayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    setlayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    setlayoutBindings[1].binding = 1;
    setlayoutBindings[1].descriptorCount = 1;
    setlayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    setlayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    vulkanFrame::CreateDescriptorSetLayout(device, setlayoutBindings, 2, &g_SetLayout);
}
void Setup(GLFWwindow *window){
    glfwSetKeyCallback(window, keybutton);
    glfwSetMouseButtonCallback(window, mousebutton);

    SetupDescriptorSetLayout(g_VulkanDevice.device);
    
    Pipeline::CreatePipelineCache(g_VulkanDevice.device, "GraphicsPipelineCache", g_PipelineCache);
    CreatePipelineLayout(g_VulkanDevice.device, g_SetLayout);
    CreateGraphicsPipeline(g_VulkanDevice.device, g_PipelineLayout);

    g_Game.Setup(g_VulkanDevice, g_SetLayout, g_VulkanQueue.graphics, g_VulkanPool);
    g_Game.UpdateUniform(g_VulkanDevice.device, g_WindowWidth);

    g_VulkanPool.AllocateCommandBuffers(g_VulkanDevice.device, 1, &g_CommandBuffers);

    //imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;//启用手柄
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window, true);
    g_VulkanImGui.Setup(g_VulkanDevice, g_VulkanPool);
    g_VulkanImGui.CreatePipeline(g_VulkanDevice.device, g_VulkanWindow.renderPass, g_PipelineCache);
    g_VulkanImGui.CreateFont(g_VulkanDevice, "fonts/ukai.ttc", g_VulkanQueue.graphics, g_VulkanPool);
    g_OnLine.Init();
    for (size_t i = 0; i < MAX_COUNTRY_INDEX - 1; i++){
        ResetCountryItem(g_CountryItems[i]);
    }

    g_Ai.InitializeAi(&g_Game, &g_OnLine);
}
void Cleanup(VkDevice device){
    ImGui_ImplGlfw_Shutdown();
    vkDestroyDescriptorSetLayout(device, g_SetLayout, VK_NULL_HANDLE);
    DestroyGraphicsPipeline(device);
    Pipeline::DestroyPipelineCache(device, "GraphicsPipelineCache", g_PipelineCache);
    g_VulkanImGui.Cleanup(device);
    g_Game.Cleanup(device);
    g_OnLine.ExitGame();
    g_OnLine.Cleanup();
}
void RecreateSwapchain(void *userData){
    GLFWwindow* window = (GLFWwindow*)userData;
    int32_t width, height;
    while(width == 0 || height == 0){
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(g_VulkanDevice.device);
    //想不调用vkDeviceWaitIdle重建交换链也行, 在重新创建交换链时, VkSwapchainCreateInfoKHR的结构体中oldSwapChain设置为原来的交换链
    g_VulkanWindow.CleanupSwapchain(g_VulkanDevice.device);
   const VkExtent2D swapchainExtent = g_VulkanWindow.swapchain.extent;
   g_VulkanWindow.swapchain.CreateSwapchain(g_VulkanDevice, g_VulkanWindow.surface);
    g_WindowWidth = g_VulkanWindow.swapchain.extent.width;
    g_WindowHeight = g_VulkanWindow.swapchain.extent.height;
    g_VulkanWindow.CreateRenderPass(g_VulkanDevice.device);
    g_VulkanWindow.CreateFrameBuffer(g_VulkanDevice);    
    if(swapchainExtent.width != g_WindowWidth || swapchainExtent.height != g_WindowHeight){
        DestroyGraphicsPipeline(g_VulkanDevice.device);
        CreatePipelineLayout(g_VulkanDevice.device, g_SetLayout);
        CreateGraphicsPipeline(g_VulkanDevice.device, g_PipelineLayout);

        //等到能正确创建交换链后, 再重建帧缓冲
        // g_PickingTexture.CreateOffscreen(g_VulkanDevice, g_WindowWidth, g_WindowHeight);
        // g_PickingTexture.DestroyGraphicsPipeline(g_VulkanDevice.device);
        // auto attributeDescriptions = Vertex::inputAttributeDescriptions(0);
        // g_PickingTexture.CreateGraphicsPipeline(g_VulkanDevice.device, g_Pipeline.layout, g_Pipeline.cache, Vertex::inputBindingDescription(0), attributeDescriptions.data(), attributeDescriptions.size());
    }
}
void display(GLFWwindow* window){
    static size_t currentFrame;
    vkDeviceWaitIdle(g_VulkanDevice.device);
    RecordCommand(g_CommandBuffers, g_VulkanWindow.framebuffers[currentFrame]);
    vulkanFrame::Render(g_VulkanDevice.device, currentFrame, g_CommandBuffers, g_VulkanWindow.swapchain.swapchain, g_VulkanQueue, g_VulkanSynchronize, RecreateSwapchain, window);

    currentFrame = (currentFrame + 1) % g_VulkanWindow.framebuffers.size();
}
VkBool32 VKAPI_PTR debugUtilsMessenger(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData){
    const char* strMessageSeverity = nullptr;//, *strMessageTypes = nullptr;
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        strMessageSeverity = "VERBOSE";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        strMessageSeverity = "INFO";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        strMessageSeverity = "WARNING";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        strMessageSeverity = "ERROR";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT) {
        strMessageSeverity = "FLAG";
    }
    printf("[VULKAN VALIDATION LAYER]\nSEVERITY:%s\nMESSAGE:%s\n", strMessageSeverity, pCallbackData->pMessage);
    return VK_FALSE;
}
bool GetPhysicalDevices(VkPhysicalDevice physicalDevice){
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    return physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
}
void SetupVulkan(GLFWwindow *window){
    uint32_t count;
    const char** instanceExtension = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char*> extensions(instanceExtension, instanceExtension + count);
    VK_CHECK(g_VulkanDevice.CreateInstance(extensions));
    g_VulkanDevice.GetPhysicalDevices(GetPhysicalDevices);
    VK_CHECK(g_VulkanDevice.CreateDebugUtilsMessenger(debugUtilsMessenger));
    glfwCreateWindowSurface(g_VulkanDevice.instance, window, nullptr, &g_VulkanWindow.surface);
    VK_CHECK(g_VulkanDevice.CreateDevice(g_VulkanWindow.surface));
    g_VulkanQueue.CreateQueue(g_VulkanDevice, g_VulkanWindow.surface);
    g_VulkanWindow.swapchain.CreateSwapchain(g_VulkanDevice, g_VulkanWindow.surface);
    g_VulkanWindow.CreateRenderPass(g_VulkanDevice.device);
    g_VulkanWindow.CreateFrameBuffer(g_VulkanDevice);
    g_VulkanPool.CreatePool(g_VulkanDevice, 20);
    g_VulkanSynchronize.CreateSynchronize(g_VulkanDevice.device, g_VulkanWindow.swapchain.images.size());
    //显示设备信息
    const char *deviceType;
    VkPhysicalDeviceProperties physicalDeviceProperties = g_VulkanDevice.GetPhysicalDeviceProperties();
    switch (physicalDeviceProperties.deviceType){
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        deviceType = "CPU";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        deviceType = "DISCRETE GPU";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        deviceType = "INTEGRATED GPU";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        deviceType = "VIRTUAL GPU";
        break;
    default:
        deviceType = "OTHER";
        break;
    }
	printf("gpu name:%s, gpu type:%s\n", physicalDeviceProperties.deviceName, deviceType);
    // PrintVulkanInfo(&physicalDeviceProperties);
}
void CleanupVulkan(){
    g_VulkanSynchronize.Cleanup(g_VulkanDevice.device);

    g_VulkanPool.Cleanup(g_VulkanDevice.device);

    g_VulkanWindow.Cleanup(g_VulkanDevice);

    g_VulkanDevice.Cleanup();
}
int main(){
    // srandom(time(nullptr));
    if (GLFW_FALSE == glfwInit()) {
        printf("initialize glfw error");
        return 1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(g_WindowWidth, g_WindowHeight, "demo", NULL, NULL);
    // Setup Vulkan
    if (!glfwVulkanSupported()){
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }
    SetupVulkan(window);
    Setup(window);

    const double targetFrameTime = 1.0 / 60.0; // 目标帧时间（例如60 FPS）
    auto previousTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(currentTime - previousTime).count();

        if (elapsedTime < targetFrameTime) {
            double sleepTime = (targetFrameTime - elapsedTime) * 1000;
            std::this_thread::sleep_for(std::chrono::milliseconds((int)sleepTime));
        }

        previousTime = std::chrono::high_resolution_clock::now();

        display(window);

        glfwPollEvents();
    }
	glfwTerminate();

    vkDeviceWaitIdle(g_VulkanDevice.device);
    Cleanup(g_VulkanDevice.device);
    CleanupVulkan();
    return 0;
}
