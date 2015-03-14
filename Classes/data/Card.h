//
//  Card.h
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/27.
//
//

#ifndef __DrinkXiyouji__Card__
#define __DrinkXiyouji__Card__

#include "cocos2d.h"

class Card : public cocos2d::Ref
{
public:
    Card();

    //创建一个用户数据对象
    static Card* create(void);

    //设置卡片id
    bool setId(int id);
    //查询卡片id
    int getId(void);

    //查询卡片类型
    int getType(void);
    //设置卡片类型
    bool setType(int type);

    //查询卡片顺位
    int getOrder(void);
    //设置卡片顺位
    bool setOrder(int order);

    //设置Jocker卡片的变化
    bool setJockerChanged(int to_id);
    //查询Jocker变化的卡片
    int getJockerChanged(void);

    //重置卡片动态数据
    bool resetCard(void);

    static const int TYPE_SHIFU;
    static const int TYPE_WUKONG;
    static const int TYPE_BAJIE;
    static const int TYPE_SHASENG;
    static const int TYPE_LAOYAOJING;
    static const int TYPE_DAYAOJING;
    static const int TYPE_ZHONGYAOJING;
    static const int TYPE_XIAOYAOJING;
    static const int TYPE_JOCKER;

private:
    //卡片ID
    int _id;
    //卡片类型
    int _type;
    //卡片顺位
    int _order;

    //Jocker变化的cid
    int _jocker_toid;
};

#endif /* defined(__DrinkXiyouji__Card__) */
