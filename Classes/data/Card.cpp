//
//  Card.cpp
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/27.
//
//

#include "Card.h"

USING_NS_CC;

const int Card::TYPE_SHIFU          = 1;
const int Card::TYPE_WUKONG         = 2;
const int Card::TYPE_BAJIE          = 3;
const int Card::TYPE_SHASENG        = 4;
const int Card::TYPE_LAOYAOJING     = 5;
const int Card::TYPE_DAYAOJING      = 6;
const int Card::TYPE_ZHONGYAOJING   = 7;
const int Card::TYPE_XIAOYAOJING    = 8;
const int Card::TYPE_JOCKER         = 9;

Card::Card()
: _id(-1), _type(0), _order(0), _jocker_toid(0)
{
}

//创建一个游戏数据对象
Card* Card::create(void)
{
    auto c = new (std::nothrow) Card();
    c->autorelease();
    return c;
}

//设置卡片id
bool Card::setId(int id)
{
    _id = id;
    return true;
}
//查询卡片id
int Card::getId(void)
{
    return _id;
}

//查询卡片类型
int Card::getType(void)
{
    return _type;
}
//设置卡片类型
bool Card::setType(int type)
{
    _type = type;
    return true;
}

//查询卡片顺位
int Card::getOrder(void)
{
    return _order;
}
//设置卡片顺位
bool Card::setOrder(int order)
{
    _order = order;
    return true;
}

//设置Jocker卡片的变化
bool Card::setJockerChanged(int to_id)
{
    if (_type != Card::TYPE_JOCKER)
    {
        return false;
    }

    _jocker_toid = to_id;
    return true;
}
//查询Jocker变化的卡片
int Card::getJockerChanged(void)
{
    return _jocker_toid;
}

//重置卡片动态数据
bool Card::resetCard(void)
{
    _jocker_toid = 0;
    return true;
}
