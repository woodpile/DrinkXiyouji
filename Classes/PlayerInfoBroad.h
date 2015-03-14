//
//  PlayerInfoBroad.h
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/26.
//
//

#ifndef __DrinkXiyouji__PlayerInfoBroad__
#define __DrinkXiyouji__PlayerInfoBroad__

#include "cocos2d.h"

class PlayerInfoBroad : public cocos2d::Sprite
{
public:
    PlayerInfoBroad();

    //创建一个玩家信息面板对象
    static PlayerInfoBroad* create(void);
    
    //引擎框架 初始化个体 在create中被调用
    virtual bool init() override;
    //引擎框架 在加载是调用
    virtual void onEnter();

    //设置一个玩家的uid
    bool setUid(int uid);

    //更新玩家信息显示
    bool updateInfo(void);

private:
    //玩家uid
    int _uid;

    //显示玩家名字
    void showName(void);
    //玩家名字标签
    cocos2d::Label* _label_name;

    //显示玩家持有牌数
    void showCardNum(void);
    //玩家持有牌数标签
    cocos2d::Label* _label_cardnum;

    //显示玩家累计惩罚数量
    void showTotalPunish(void);
    //玩家累计惩罚数量标签
    cocos2d::Label* _label_totalpunish;

    //显示当前出手标志
    void showTurnTag(void);
    //出手标志
    cocos2d::Sprite* _tag_turn;
};

#endif /* defined(__DrinkXiyouji__PlayerInfoBroad__) */
