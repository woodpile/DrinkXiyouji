//
//  GameData.cpp
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/26.
//
//

#include "GameData.h"

#include "UserData.h"
#include "Card.h"
#include "Struct.h"

USING_NS_CC;

GameData::GameData()
: _num_player(0),
  _self_uid(0)
{
}

//创建一个游戏数据对象
GameData* GameData::create(void)
{
    auto g = new (std::nothrow) GameData();
    g->autorelease();
    return g;
}

//生成游戏数据
bool GameData::genData(NetGameData &gamedata)
{
    if (0 == _num_player) return false;

    for (int i = 0; i < _num_player; i++)
    {
        auto ud = UserData::create();
        auto p = gamedata.Players.at(i);
        ud->setUid(p->Uid);
        ud->setUname(p->Uname);
        ud->setCardNum(p->NumCard);
        _users.insert(p->Uid, ud);

        if (p->Uid == _self_uid)
        {
            for (auto& cid : p->Cards)
            {
                ud->cardCatch(cid);
            }
        }
    }

    for (auto& cd : gamedata.Cards)
    {
        auto c = Card::create();

        c->setId(cd->Cid);
        c->setType(cd->Type);
        c->setOrder(cd->Order);

        _cards.insert(cd->Cid, c);
    }

    return true;
}

//设置游戏人数
bool GameData::setPlayerNum(int n)
{
    _num_player = n;
    return true;
}

//设置本地用户的uid
bool GameData::setSelfUid(int uid)
{
    _self_uid = uid;
    return true;
}
//查询自己uid
int GameData::getSelfUid(void)
{
    return _self_uid;
}

//查询uid列表
std::vector<int> GameData::getAllUids(void)
{
    std::vector<int> uids;

    for (auto& e : _users)
    {
        uids.push_back(e.first);
    }

    return uids;
}

//查询一个用户的数据
UserData* GameData::getUserData(int uid)
{
    if (_users.end() != _users.find(uid))
    {
        return _users.at(uid);
    }
    return nullptr;
}

//查询一个卡片的数据
Card* GameData::getCard(int cid)
{
    if (_cards.end() == _cards.find(cid))
    {
        return nullptr;
    }

    return _cards.at(cid);
}
//查询一个卡片的数据, 由类型和顺位
Card* GameData::getCard(int type, int order)
{
    for (auto &e : _cards)
    {
        if (e.second->getType() == type &&
            e.second->getOrder() == order)
        {
            return e.second;
        }
    }
    return nullptr;
}

//更新游戏数据
bool GameData::updateData(NetGameStatus &gamestatus)
{
    for (auto& p : gamestatus.Players)
    {
        auto ud = _users.at(p->Uid);
        if (nullptr == ud)
        {
            continue;
        }

        ud->updateInfo(p);
    }

    if (0 != (gamestatus.JockerTo.size() % 2))
    {
        return false;
    }

    for (int i = 0; i < gamestatus.JockerTo.size(); i += 2)
    {
        auto j_id = gamestatus.JockerTo.at(i);
        auto to_id = gamestatus.JockerTo.at(i+1);

        auto card = _cards.at(j_id);
        if (nullptr == card)
        {
            continue;
        }
        card->setJockerChanged(to_id);
    }

    return true;
}

//重置,由新轮游戏引起的
bool GameData::resetByNewRound(void)
{
    for (auto& e : _cards)
    {
        e.second->resetCard();
    }
    return true;
}

