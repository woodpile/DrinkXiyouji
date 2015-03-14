//
//  GameStage.cpp
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/25.
//
//

#include "GameStage.h"

#include "../data/GameData.h"
#include "../data/Struct.h"
#include "../data/MyData.h"
#include "../data/Card.h"

USING_NS_CC;

GameStage::GameStage()
: _data(nullptr),
  _current_uid(0), _round(0)
{
}

GameStage* GameStage::create(void)
{
    auto g = new (std::nothrow) GameStage();
    g->autorelease();
    return g;
}

//生成游戏数据对象
bool GameStage::genGameData(NetGameData &gamedata)
{
    //创建游戏数据对象
    _data = GameData::create();
    _data->retain();

    //设置游戏
    _data->setPlayerNum((int)gamedata.Players.size());
    //设置游戏的本地用户uid
    _data->setSelfUid(MyData::getInstance()->getMyUid());

    //生成游戏数据
    _data->genData(gamedata);

    return true;
}

//生成游戏状态
bool GameStage::genStage(NetGameData &gamedata)
{
    return this->genGameData(gamedata);
}

//查询自己uid
int GameStage::getSelfUid(void)
{
    return _data->getSelfUid();
}

//查询uid列表
std::vector<int> GameStage::getAllUids(void)
{
    return _data->getAllUids();
}

//查询一个用户的数据
UserData* GameStage::getUserData(int uid)
{
    return _data->getUserData(uid);
}

//查询一个卡片的数据
Card* GameStage::getCard(int cid)
{
    return _data->getCard(cid);
}
//查询一个卡片的数据, 由类型和顺位
Card* GameStage::getCard(int type, int order)
{
    return _data->getCard(type, order);
}

//查询当前出手的玩家的uid
int GameStage::getCurrentTurn(void)
{
    return _current_uid;
}

//查询当前游戏的轮次
int GameStage::getRound(void)
{
    return _round;
}

//更新游戏状态
bool GameStage::updateStage(NetGameStatus &gamestatus)
{
    _current_uid = gamestatus.CurrentTurn;

    _data->updateData(gamestatus);

    if (_round != gamestatus.Round)
    {
        _data->resetByNewRound();
        _round = gamestatus.Round;
    }
    return true;
}



