//
//  GameStage.h
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/25.
//
//

#ifndef __DrinkXiyouji__GameStage__
#define __DrinkXiyouji__GameStage__

#include "cocos2d.h"

class GameData;
class UserData;
class NetGameData;
class NetGameStatus;
class Card;

class GameStage : public cocos2d::Ref
{
public:
    GameStage();
    //创建一个游戏状态管理对象
    static GameStage* create(void);

    //生成游戏状态
    bool genStage(NetGameData &gamedata);
    //更新游戏状态
    bool updateStage(NetGameStatus &gamestatus);

    //查询自己uid
    int getSelfUid(void);

    //查询uid列表
    std::vector<int> getAllUids(void);

    //查询一个用户的数据
    UserData* getUserData(int uid);
    //查询一个卡片的数据
    Card* getCard(int cid);
    //查询一个卡片的数据, 由类型和顺位
    Card* getCard(int type, int order);

    //查询当前出手的玩家的uid
    int getCurrentTurn(void);

    //查询当前游戏的轮次
    int getRound(void);

private:
    //生成游戏数据对象
    bool genGameData(NetGameData &gamedata);
    //游戏数据对象
    GameData* _data;

    //当前出手的玩家
    int _current_uid;

    //当前轮次
    int _round;
};

#endif /* defined(__DrinkXiyouji__GameStage__) */
