//
//  RoomScene.h
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/28.
//
//

#ifndef __DrinkXiyouji__RoomScene__
#define __DrinkXiyouji__RoomScene__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "data/Struct.h"

class RoomScene : public cocos2d::Layer, public cocos2d::ui::EditBoxDelegate
{
public:
    RoomScene();
    static cocos2d::Scene* createScene();
    static RoomScene* getInstance();

    virtual bool init();

    CREATE_FUNC(RoomScene);

    //返回加载场景
    void menuReturnLoadScene(cocos2d::Ref* pSender);
    //创建房间
    void menuCreateRoomCallback(cocos2d::Ref* pSender);
    //查看房间列表
    void menuGetRoomListCallback(cocos2d::Ref* pSender);
    //按房间号加入
    void menuJoinRoomIdCallback(cocos2d::Ref* pSender);
    //按房间号加入输入确认
    void menuJoinRoomIdInputCallback(cocos2d::Ref* pSender);
    //加入房间
    void menuJoinRoomCallback(cocos2d::Ref* pSender);
    //房间内开始游戏
    void menuStartRoomCallback(cocos2d::Ref* pSender);
    //卡片倍数增加
    void menuMultiPlusCallback(cocos2d::Ref* pSender);
    //卡片倍数减少
    void menuMultiMinusCallback(cocos2d::Ref* pSender);

    //查看房间列表信息处理
    void netGetRoomListCallback(NetRoomInfo aRoomInfo[], int size);
    //加入房间信息处理
    void netJoinRoomCallback(bool success);
    //查看房间内信息处理
    void netGetRoomInfoCallback(NetRoomInner &roomInner);
    //设置卡片倍数信息处理
    void netSetCardMultiCallback(bool success, int multi);

    virtual void editBoxReturn(cocos2d::ui::EditBox* editBox);

private:
    //房间列表菜单
    cocos2d::Menu* _menu_room;

    //功能按钮菜单
    cocos2d::Menu* _menu_func;

    //加载房间内信息
    void loadRoomInfo(void);
    //房间主的uid
    int _room_owner;
    //房间内信息底板
    cocos2d::Sprite* _info_loadroom;
    //当前加入的房间
    int _room_current;
    //房间号输入信息底板
    cocos2d::Sprite* _input_room_id_broad;
    //房间号输入框
    cocos2d::ui::EditBox* _input_room_id;

    //定时发送加载房间内信息的请求
    void timeShowRoomInfo(float dt);

    //刷新房间内信息显示
    void refreshRoomInnerInfo(NetRoomInner &roomInner);
    //进入游戏过程, 人员齐全
    void enterGameStatus(void);
    //在房间信息中显示一个用户的信息
    void loadPlayerInfo(cocos2d::Sprite* broad, NetPlayerInfo* info);
    //承载可刷新信息的节点
    cocos2d::Node* _root_inner_info;

    //显示卡片倍数的标签
    cocos2d::Label* _label_multi;
    //当前房间的卡片倍数
    int _card_multi;
    //卡片倍数加减按键的菜单
    cocos2d::Menu* _menu_multi;
};

#endif /* defined(__DrinkXiyouji__RoomScene__) */
