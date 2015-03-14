//
//  MyData.h
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/28.
//
//

#ifndef __DrinkXiyouji__MyData__
#define __DrinkXiyouji__MyData__

#include "cocos2d.h"

class MyData : public cocos2d::Ref
{
public:
    MyData();
    //创建一个游戏数据对象
    static MyData* create(void);
    static MyData* getInstance(void);

    //查询用户的uid
    int getMyUid(void);
    //设置用户的uid
    bool setMyUid(int uid);

    //查询用户的uname
    std::string getMyUname(void);
    //设置用户的uname
    bool setMyUname(std::string uname);

    //设置当前的房间号
    bool setCurrentRoomId(int room_id);
    //查询当前的房间号
    int getCurrentRoomId(void);

private:
    //本地用户的uid
    int _uid;
    //当前游戏的房间号
    int _room_current;
    //本地用户名
    std::string _local_uname;
};

#endif /* defined(__DrinkXiyouji__MyData__) */
