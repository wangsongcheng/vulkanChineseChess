--[[
    #define WU_COUNTRY_INDEX 1
    #define WEI_COUNTRY_INDEX 2
    #define SHU_COUNTRY_INDEX 0
    #define HAN_COUNTRY_INDEX 3
    #define INVALID_COUNTRY_INDEX -1
    //如果汉能操作, 那么下面2个宏用不到
    #define HAN_PAO_CHESS_INDEX 1
    #define HAN_CHE_CHESS_INDEX (HAN_PAO_CHESS_INDEX + HAN_PAO_CHESS_COUNT)

    #define JIANG_CHESS_INDEX 0
    #define MA_CHESS_INDEX (JIANG_CHESS_INDEX + JIANG_CHESS_COUNT)
    //下一只炮的索引是PAO_CHESS_INDEX + 1//其他同理
    #define PAO_CHESS_INDEX (MA_CHESS_INDEX + MA_CHESS_COUNT)
    #define CHE_CHESS_INDEX (PAO_CHESS_INDEX + PAO_CHESS_COUNT)
    #define SHI_CHESS_INDEX (CHE_CHESS_INDEX + CHE_CHESS_COUNT)
    #define XIANG_CHESS_INDEX (SHI_CHESS_INDEX + SHI_CHESS_COUNT)
    #define BING_CHESS_INDEX (XIANG_CHESS_INDEX + XIANG_CHESS_COUNT)
    
    --can play 主要指的是，某个棋子能走的位置
    GetCurrentCountry()
    IsChess(country, chess)
    GetRow(country, chess)
    GetColumn(country, chess)
    GetChess(chess row, chess column)
    GetCountry(chess row, chess column)
    GetCanPlayCount(country index, chess index)
    GetCanPlayRow(country index, chess index, can play index)
    GetCanPlayColumn(country index, chess index, can play index)
--]]
function GetCanPlayRowColumn(country, chess, index)
    local row = GetCanPlayRow(country, chess, index)
    local column = GetCanPlayColumn(country, chess, index)
    return row, column
end
function GetRowColumn(country, chess)
    local row = -1
    local column = -1
    row = GetRow(country, chess)
    column = GetColumn(country, chess)
    return row, column
end
function GetTargetChess(currentCountry, chess)
    local row = -1
    local column = -1
    local canplayCount = GetCanPlayCount(currentCountry, chess)
    for i = 0, canplayCount do
        local canplayRow, canplayColumn = GetCanPlayRowColumn(currentCountry, chess, i);
        local country = GetCountry(canplayRow, canplayColumn)
        local targetChess = GetChess(canplayRow, canplayColumn);
        if IsChess(country, targetChess) and currentCountry ~= country then
            row = canplayRow
            column = canplayColumn
            io.write("have target:row  = ", row, ", column = ", column, "\n");
            break
        end
    end
    return row, column
end
--若chess有可下的位置, 则返回true
function CanPlay(currentCountry, chess)
    local bCanplay = false;
    local canplayCount = GetCanPlayCount(currentCountry, chess)
    for i = 0, canplayCount - 1 do
        local canplayRow, canplayColumn = GetCanPlayRowColumn(currentCountry, chess, i)
        if IsChess(currentCountry, chess) == false or currentCountry ~= GetCountry(canplayRow, canplayColumn) then
            io.write("this chess(", chess, ") can play, can play row = ", canplayRow, ", column = ", canplayColumn, "\n")
            bCanplay = true
            break
        end
    end
    return bCanplay
end
function GetSelectAndTarget(currentCountry, chessCount)
    -- --会被多次调用，返回合法的棋子行列
    -- --随机可用的棋子
    local chess = -1
    local selectRow = -1
    local targetRow = -1
    local selectColumn = -1;
    local targetColumn = -1;
    io.write("------------in lua source file, function GetSelectAndTarget:currentCountry = ", currentCountry, ", chessCount = ", chessCount, "-------------\n")
    while  true do
        chess = math.random(0, chessCount)
        if CanPlay(currentCountry, chess) then
            selectRow, selectColumn = GetRowColumn(currentCountry, chess)
            break
        end
    end
    io.write("chess = ", chess, "\n")
    targetRow, targetColumn = GetTargetChess(currentCountry, chess)
    io.write("targetRow = ", targetRow, ", targetColumn = ", targetColumn, "\n")
    if targetRow == -1 and targetColumn == -1 then
        local canplayCount = GetCanPlayCount(currentCountry, chess)
        io.write("canplayCount = ", canplayCount, "\n")
        for i = 0, canplayCount do
            local canplayRow, canplayColumn = GetCanPlayRowColumn(currentCountry, chess, i)
            io.write("canplayRow = ", canplayRow, ", canplayColumn = ", canplayColumn, "\n")
            if IsChess(currentCountry, chess) then
                targetRow = canplayRow
                targetColumn = canplayColumn
                io.write("targetRow = ", targetRow, ", targetColumn = ", targetColumn, "\n")
                break
            end
        end
    end
    return selectRow, selectColumn, targetRow, targetColumn
end