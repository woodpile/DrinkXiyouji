//
//  MyData.cpp
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/28.
//
//

#include "MyData.h"

USING_NS_CC;

MyData::MyData()
: _uid(-1), _room_current(-1), _local_uname("")
{
}

static MyData* gMyData = nullptr;
MyData* MyData::getInstance(void)
{
    if (nullptr == gMyData)
    {
        gMyData = MyData::create();
    }
    return gMyData;
}

//创建一个游戏数据对象
MyData* MyData::create(void)
{
    auto d = new (std::nothrow) MyData();
    return d;
}

//查询用户的uid
int MyData::getMyUid(void)
{
    return _uid;
}

//设置用户的uid
bool MyData::setMyUid(int uid)
{
    _uid = uid;
    return true;
}

//查询用户的uname
std::string MyData::getMyUname(void)
{
    return _local_uname;
}
//设置用户的uname
bool MyData::setMyUname(std::string uname)
{
    _local_uname = uname;
    return true;
}

//设置当前的房间号
bool MyData::setCurrentRoomId(int room_id)
{
    _room_current = room_id;
    return true;
}
//查询当前的房间号
int MyData::getCurrentRoomId(void)
{
    return _room_current;
}
