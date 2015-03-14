//
//  Struct.h
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/28.
//
//

#ifndef __DrinkXiyouji__Struct__
#define __DrinkXiyouji__Struct__

#include "cocos2d.h"

struct NetRoomInfo
{
    int Room_id;
};

struct NetPlayerInfo : public cocos2d::Ref
{
    int Uid;
    std::string Uname;
};

struct NetRoomInner : public cocos2d::Ref
{
    cocos2d::Vector<NetPlayerInfo*> Players;
    int Status;
    int MaxPlayer;
    int CardMulti;
    int Creator;
};

struct NetGameCardStatus : public cocos2d::Ref
{
    int Cid;
    int Type;
    int Order;
};
struct NetGamePlayerStatus : public cocos2d::Ref
{
    int Uid;
    std::string Uname;
    int NumCard;
    int TotalPunish;
    std::vector<int> Cards;
};
struct NetGameStatus : public cocos2d::Ref
{
    int CurrentTurn;
    int Round;
    cocos2d::Vector<NetGamePlayerStatus*> Players;
    std::vector<int> LastStep;
    std::vector<int> JockerTo;
    int LastStepOwn;
};
struct NetGameData : public cocos2d::Ref
{
    cocos2d::Vector<NetGamePlayerStatus*> Players;
    cocos2d::Vector<NetGameCardStatus*> Cards;
};

#endif /* defined(__DrinkXiyouji__Struct__) */
