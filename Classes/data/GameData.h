//
//  GameData.h
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/26.
//
//

#ifndef __DrinkXiyouji__GameData__
#define __DrinkXiyouji__GameData__

#include "cocos2d.h"

class UserData;
class Card;
class NetGameData;
class NetGameStatus;

class GameData : public cocos2d::Ref
{
public:
    GameData();
    //创建一个游戏数据对象
    static GameData* create(void);

    //生成游戏数据
    bool genData(NetGameData &gamedata);
    //更新游戏数据
    bool updateData(NetGameStatus &gamestatus);

    //设置游戏人数
    bool setPlayerNum(int n);

    //设置本地用户的uid
    bool setSelfUid(int uid);
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

    //重置,由新轮游戏引起的
    bool resetByNewRound(void);

private:
    //用户数据
    cocos2d::Map<int, UserData*> _users;
    //玩家人数
    int _num_player;

    //自己uid
    int _self_uid;

    //卡片集合
    cocos2d::Map<int, Card*> _cards;
};

#endif /* defined(__DrinkXiyouji__GameData__) */
