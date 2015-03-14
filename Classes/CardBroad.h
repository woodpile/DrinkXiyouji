//
//  CardBroad.h
//  DrinkXiyouji
//
//  Created by woodpile on 15/3/4.
//
//

#ifndef __DrinkXiyouji__CardBroad__
#define __DrinkXiyouji__CardBroad__

#include "cocos2d.h"

class CardBroad : public cocos2d::Sprite
{
public:
    CardBroad();

    //创建一个玩家信息面板对象
    static CardBroad* create(void);

    //引擎框架 初始化个体 在create中被调用
    virtual bool init() override;
    //引擎框架 在加载是调用
    virtual void onEnter();

    //设置卡片id
    bool setCid(int cid);
    //查看卡片id
    int getCid(void);
    
    //引擎 触摸行为注册函数
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    //恢复位置
    void moveBack(void);

    //Jocker类型选择
    void menuJockerTypeCallback(cocos2d::Ref* pSender);
    //Jocker顺位选择
    void menuJockerOrderCallback(cocos2d::Ref* pSender);

private:
    //卡片id
    int _cid;

    //生成卡片属性标记
    void genAttrTag(void);

    //选中状态
    bool _bSelect;
    //原位置坐标
    cocos2d::Vec2 _orig_pos;

    //显示Jocker选择界面
    void showJockerSelectUI(void);
    //类型选择根节点
    cocos2d::Menu* _menu_type_select;
    //顺位选择根节点
    cocos2d::Menu* _menu_order_select;
    //Jocker选择的类型
    int _jocker_select_type;
    //Jocker选择的顺位
    int _jocker_select_order;
    //jocker选择顺位后处理
    void selectOrderAfterCallback(cocos2d::Node* sender);
};

#endif /* defined(__DrinkXiyouji__CardBroad__) */
