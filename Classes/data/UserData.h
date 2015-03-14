//
//  UserData.h
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/26.
//
//

#ifndef __DrinkXiyouji__UserData__
#define __DrinkXiyouji__UserData__

#include "cocos2d.h"

class Card;
class NetGamePlayerStatus;

class UserData : public cocos2d::Ref
{
public:
    UserData();

    //创建一个用户数据对象
    static UserData* create(void);

    //设置用户uid
    bool setUid(int uid);
    //查询用户uid
    int getUid(void);

    //设置用户名
    bool setUname(std::string &name);
    //查询用户名
    std::string getUname(void);

    //持有一张卡片
    bool cardCatch(int cid);
    //查询卡片数量
    int getCardNum(void);
    //设置卡片数量
    bool setCardNum(int numCard);

    //查看所有卡片
    std::vector<int>& getOwnCards(void);
    //更新用户信息
    bool updateInfo(NetGamePlayerStatus* ps);

    //设置惩罚累计数量
    bool setTotalPunish(int pn);
    //查看惩罚累计数量
    int getTotalPunish(void);

private:
    //用户uid
    int _uid;
    //用户名
    std::string _uname;

    //持有的卡片
    std::vector<int> _cards;
    //持有的卡片数量
    int _num_card;

    //累计的惩罚数量
    int _total_punish;
};

#endif /* defined(__DrinkXiyouji__UserData__) */
