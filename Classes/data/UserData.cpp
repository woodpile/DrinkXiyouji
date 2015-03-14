//
//  UserData.cpp
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/26.
//
//

#include "Struct.h"
#include "UserData.h"

#include "Card.h"

USING_NS_CC;

UserData::UserData()
: _uid(-1), _uname("UserNaMe"), _total_punish(0)
{
}

//创建一个游戏数据对象
UserData* UserData::create(void)
{
    auto u = new (std::nothrow) UserData();
    u->autorelease();
    return u;
}

//设置用户uid
bool UserData::setUid(int uid)
{
    _uid = uid;
    return true;
}
//查询用户uid
int UserData::getUid(void)
{
    return _uid;
}

//设置用户名
bool UserData::setUname(std::string &name)
{
    _uname = name;
    return true;
}
//查询用户的用户名
std::string UserData::getUname(void)
{
    return _uname;
}


//持有一张卡片
bool UserData::cardCatch(int cid)
{
    _cards.push_back(cid);
    return true;
}

//查询卡片数量
int UserData::getCardNum(void)
{
    auto n = (int)_cards.size();
    if (0 != n) {
        return n;
    }
    return _num_card;
}
//设置卡片数量
bool UserData::setCardNum(int numCard)
{
    _num_card = numCard;
    return true;
}

//查看所有卡片
std::vector<int>& UserData::getOwnCards(void)
{
    return _cards;
}

//设置惩罚累计数量
bool UserData::setTotalPunish(int pn)
{
    _total_punish = pn;
    return true;
}
//查看惩罚累计数量
int UserData::getTotalPunish(void)
{
    return _total_punish;
}

//更新用户信息
bool UserData::updateInfo(NetGamePlayerStatus* ps)
{
    this->setCardNum(ps->NumCard);

    _total_punish = ps->TotalPunish;

    _cards.clear();
    for (auto& cid : ps->Cards)
    {
        _cards.push_back(cid);
    }

    return true;
}


