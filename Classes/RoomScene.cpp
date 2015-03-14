//
//  RoomScene.cpp
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/28.
//
//


#include "RoomScene.h"
#include "GameScene.h"
#include "LoadScene.h"

#include "game/Net.h"
#include "data/MyData.h"

USING_NS_CC;

RoomScene::RoomScene()
: _menu_room(nullptr), _menu_func(nullptr), _info_loadroom(nullptr),
  _room_current(0),
  _root_inner_info(nullptr), _label_multi(nullptr), _card_multi(1), _menu_multi(nullptr),
  _room_owner(0),
  _input_room_id_broad(nullptr), _input_room_id(nullptr)
{
}

RoomScene* _gRoomScene = nullptr;

RoomScene* RoomScene::getInstance()
{
    return _gRoomScene;
}

Scene* RoomScene::createScene()
{
    auto scene = Scene::create();

    auto layer = RoomScene::create();

    scene->addChild(layer);

    _gRoomScene = layer;

    return scene;
}

bool RoomScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto label = Label::createWithTTF("Rooms", "fonts/heiti.ttf", 72);
    label->setColor(Color3B::BLACK);

    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    this->addChild(label, 1);

    auto closeItem = MenuItemImage::create("Exit_N.png",
                                           "Exit_S.png",
                                           CC_CALLBACK_1(RoomScene::menuReturnLoadScene, this));

    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + visibleSize.height - closeItem->getContentSize().height/2));

    auto closemenu = Menu::create(closeItem, NULL);
    closemenu->setPosition(Vec2::ZERO);
    this->addChild(closemenu, 1);

    auto sprite = Sprite::create("LoadSceneBack.png");

    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    this->addChild(sprite, 0);

    auto createRoomItem = MenuItemImage::create("CreateRoom_N.png",
                                           "CreateRoom_S.png",
                                           CC_CALLBACK_1(RoomScene::menuCreateRoomCallback, this));
    createRoomItem->setPosition(Vec2(-40 - 0.5 * 640, 80));
    auto joinRoomIdItem = MenuItemImage::create("JoinRoom_N.png",
                                                "JoinRoom_S.png",
                                                CC_CALLBACK_1(RoomScene::menuJoinRoomIdCallback, this));
    joinRoomIdItem->setPosition(Vec2(-40 - 0.5 * 640, -80));
    auto getRoomsItem = MenuItemImage::create("GetRooms_N.png",
                                              "GetRooms_S.png",
                                              CC_CALLBACK_1(RoomScene::menuGetRoomListCallback, this));
    getRoomsItem->setPosition(Vec2(40 + 0.5 * 640, 0));

    _menu_func = Menu::create(createRoomItem, joinRoomIdItem, getRoomsItem, nullptr);
    _menu_func->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + 200 + 0.5 * 320));
    this->addChild(_menu_func);
    
    return true;
}

//返回加载场景
void RoomScene::menuReturnLoadScene(cocos2d::Ref* pSender)
{
    if (0 != _room_current)
    {
        UtilNet::sendLeaveRoom();
    }
    Director::getInstance()->replaceScene(LoadScene::createScene());
}
//创建房间
void RoomScene::menuCreateRoomCallback(cocos2d::Ref* pSender)
{
    UtilNet::sendCreateRoom();
}
//按房间号加入
void RoomScene::menuJoinRoomIdCallback(cocos2d::Ref* pSender)
{
    if (nullptr != _input_room_id)
    {
        _input_room_id->removeFromParent();
        _input_room_id = nullptr;
    }

    _input_room_id_broad = Sprite::create("RoomPlayerBroad.png");

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    _input_room_id_broad->setPosition(Vec2(origin.x + 0.5 * visibleSize.width,
                                     origin.y + 0.75 * visibleSize.height));

    this->addChild(_input_room_id_broad);

    _input_room_id = ui::EditBox::create(Size(860, 160), "EditBoxBroad.png");
    _input_room_id->setPosition(Vec2(500, 100));
    _input_room_id->setFont("fonts/heiti.ttf", 100);
    _input_room_id->setFontColor(Color3B::BLACK);
    _input_room_id->setPlaceHolder("Room ID");
    _input_room_id->setPlaceholderFontColor(Color3B::GRAY);
    _input_room_id->setMaxLength(8);
    _input_room_id->setInputMode(ui::EditBox::InputMode::NUMERIC);
    _input_room_id->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    _input_room_id->setDelegate(this);
    _input_room_id_broad->addChild(_input_room_id);

    auto joinItem = MenuItemImage::create("JoinInput_N.png",
                                          "JoinInput_S.png",
                                          CC_CALLBACK_1(RoomScene::menuJoinRoomIdInputCallback, this));
    auto joinmenu = Menu::create(joinItem, nullptr);
    joinmenu->setPosition(Vec2(1100, 100));
    _input_room_id_broad->addChild(joinmenu);
}
void RoomScene::editBoxReturn(ui::EditBox* editBox)
{
}
//按房间号加入输入确认
void RoomScene::menuJoinRoomIdInputCallback(cocos2d::Ref* pSender)
{
    auto c_str = _input_room_id->getText();
    int room_id = 0;
    sscanf(c_str, "%d", &room_id);
    if (0 == room_id)
    {
        return;
    }
    _room_current = room_id;
    UtilNet::sendJoinRoom(room_id);
}
//查看房间列表
void RoomScene::menuGetRoomListCallback(cocos2d::Ref* pSender)
{
    UtilNet::sendGetRoomList();
}
//加入房间
void RoomScene::menuJoinRoomCallback(cocos2d::Ref* pSender)
{
    if (0 != _room_current)
    {
        return;
    }

    auto join_id = static_cast<MenuItemImage*>(pSender)->getTag();
    UtilNet::sendJoinRoom(join_id);
    _room_current = join_id;
}

//登录返回信息处理
void RoomScene::netGetRoomListCallback(NetRoomInfo aRoomInfo[], int size)
{
    if (nullptr != _menu_room)
    {
        _menu_room->removeFromParent();
        _menu_room = nullptr;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    _menu_room = Menu::create();
    _menu_room->setPosition(Vec2(origin.x + 0.5 * visibleSize.width,
                                 origin.y + visibleSize.height - 300 - 0.5 * 160));
    this->addChild(_menu_room);

    for (int i = 0; i < size; i++)
    {
        for (int j = i+1; j < size; j++)
        {
            if (aRoomInfo[i].Room_id > aRoomInfo[j].Room_id)
            {
                auto t = aRoomInfo[j];
                aRoomInfo[j] = aRoomInfo[i];
                aRoomInfo[i] = t;
            }
        }
    }

    for (int i = 0; i < size; i++)
    {
        auto roomItem = MenuItemImage::create("Room_N.png",
                                              "Room_S.png",
                                              CC_CALLBACK_1(RoomScene::menuJoinRoomCallback, this));
        _menu_room->addChild(roomItem);
        roomItem->setPositionY(0 - i * (160 + 40));
        roomItem->setTag(aRoomInfo[i].Room_id);

        auto str = StringUtils::format("Room %d", aRoomInfo[i].Room_id);
        auto label = Label::createWithTTF(str, "fonts/heiti.ttf", 120);
        label->setPosition(roomItem->getContentSize().width * 0.5,
                           roomItem->getContentSize().height * 0.5);
        roomItem->addChild(label);
    }
}
//加入房间信息处理
void RoomScene::netJoinRoomCallback(bool success)
{
    if (false == success)
    {
        _room_current = 0;
        return;
    }

    if (nullptr != _input_room_id_broad)
    {
        _input_room_id = nullptr;
        _input_room_id_broad->removeFromParent();
        _input_room_id_broad = nullptr;
    }
    this->loadRoomInfo();
}

//加载房间内信息
void RoomScene::loadRoomInfo(void)
{
    log("loadRoomInfo");

    if (nullptr != _info_loadroom)
    {
        _info_loadroom->removeFromParent();
        _info_loadroom = nullptr;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    _info_loadroom = Sprite::create("RoomBroad.png");
    _info_loadroom->setPosition(Vec2(origin.x + visibleSize.width * 0.5,
                                     origin.y + visibleSize.height - 240 - 0.5 * 1300));
    this->addChild(_info_loadroom);

    _menu_room->removeFromParent();
    _menu_room = nullptr;

    this->timeShowRoomInfo(0);

    _menu_func->removeAllChildren();

    auto startItem = MenuItemImage::create("StartRoom_N.png",
                                           "StartRoom_S.png",
                                           CC_CALLBACK_1(RoomScene::menuStartRoomCallback, this));
    _menu_func->addChild(startItem);

    auto multiPlus = MenuItemImage::create("NumPlus.png",
                                           "NumPlus.png",
                                           CC_CALLBACK_1(RoomScene::menuMultiPlusCallback, this));
    auto multiMinus = MenuItemImage::create("NumMinuse.png",
                                            "NumMinuse.png",
                                            CC_CALLBACK_1(RoomScene::menuMultiMinusCallback, this));
    _menu_multi = Menu::create(multiPlus, multiMinus, nullptr);
    multiPlus->setPosition(Vec2(120,0));
    multiMinus->setPosition(Vec2(-120,0));

    _info_loadroom->addChild(_menu_multi);
    _menu_multi->setPosition(_info_loadroom->getContentSize().width * 0.75,
                            _info_loadroom->getContentSize().height - 80);

    _label_multi = Label::createWithTTF("1", "fonts/heiti.ttf", 120);
    _label_multi->setPosition(_menu_multi->getPosition());
    _label_multi->setColor(Color3B::BLACK);
    _info_loadroom->addChild(_label_multi);

    auto label_multi_name = Label::createWithTTF("卡片倍数", "fonts/heiti.ttf", 60);
    label_multi_name->setPosition(_menu_multi->getPosition() + Vec2(-350,0));
    label_multi_name->setTextColor(Color4B::BLACK);
    _info_loadroom->addChild(label_multi_name);

    _root_inner_info = Node::create();
    _info_loadroom->addChild(_root_inner_info);
}

//定时发送加载房间内信息的请求
void RoomScene::timeShowRoomInfo(float dt)
{
    this->unschedule(schedule_selector(RoomScene::timeShowRoomInfo));
    UtilNet::sendGetRoomInfo(_room_current);
}

//查看房间内信息处理
void RoomScene::netGetRoomInfoCallback(NetRoomInner &roomInner)
{
    if (1 == roomInner.Status)
    {
        refreshRoomInnerInfo(roomInner);
    }
    else if (2 == roomInner.Status)
    {
        enterGameStatus();
    }
}

//刷新房间内信息显示
void RoomScene::refreshRoomInnerInfo(NetRoomInner &roomInner)
{
    _room_owner = roomInner.Creator;
    _root_inner_info->removeAllChildren();

    Vec2 pos(_info_loadroom->getContentSize().width / 2, _info_loadroom->getContentSize().height - 60);

    Vector<NetPlayerInfo*>& players = roomInner.Players;
    for (int i = 0; i < players.size(); i++)
    {
        for (int j = i+1; j < players.size(); j++)
        {
            if (players.at(i)->Uid > players.at(j)->Uid)
            {
                players.swap(i, j);
            }
        }
    }

    for (auto& e : players)
    {
        log("uid %d uname %s", e->Uid, e->Uname.c_str());
        auto pi = Sprite::create("RoomPlayerBroad.png");

        pos.y = pos.y - pi->getContentSize().height;
        pi->setPosition(pos);

        _root_inner_info->addChild(pi);

        this->loadPlayerInfo(pi, e);
    }

    //显示房间人数
    auto str = StringUtils::format("人数: %ld/%d", players.size(), roomInner.MaxPlayer);
    auto label_player_num = Label::createWithTTF(str, "fonts/heiti.ttf", 60);
    label_player_num->setColor(Color3B::BLACK);
    label_player_num->setPosition(0 + 200,
                                  _info_loadroom->getContentSize().height - 80);
    _root_inner_info->addChild(label_player_num);

    //更新卡片倍数
    _card_multi = roomInner.CardMulti;
    _label_multi->setString(StringUtils::format("%d", _card_multi));

    if (_room_owner != MyData::getInstance()->getMyUid() && nullptr != _menu_multi)
    {
        _menu_multi->removeFromParent();
        _menu_multi = nullptr;
    }

    //启动定时器拉取下一次数据
    this->schedule(schedule_selector(RoomScene::timeShowRoomInfo), 1);
}

//在房间信息中显示一个用户的信息
void RoomScene::loadPlayerInfo(cocos2d::Sprite* broad, NetPlayerInfo* info)
{
    auto label = Label::createWithTTF(info->Uname, "fonts/heiti.ttf", 75);
    label->setColor(Color3B::BLACK);
    label->setPosition(Vec2(30 + label->getContentSize().width * 0.5,
                            broad->getContentSize().height - 30 - label->getContentSize().height * 0.5));
    broad->addChild(label);
}

//进入游戏过程, 人员齐全
void RoomScene::enterGameStatus(void)
{
    MyData::getInstance()->setCurrentRoomId(_room_current);
    Director::getInstance()->replaceScene(GameScene::createScene());
}

//房间内开始游戏
void RoomScene::menuStartRoomCallback(cocos2d::Ref* pSender)
{
    if (_room_owner != MyData::getInstance()->getMyUid())
    {
        return;
    }
    UtilNet::sendStartGame();
}

//卡片倍数增加
void RoomScene::menuMultiPlusCallback(cocos2d::Ref* pSender)
{
    if (_room_owner != MyData::getInstance()->getMyUid())
    {
        return;
    }
    _card_multi += 1;
    UtilNet::sendSetCardMulti(_card_multi);
}
//卡片倍数减少
void RoomScene::menuMultiMinusCallback(cocos2d::Ref* pSender)
{
    if (_room_owner != MyData::getInstance()->getMyUid())
    {
        return;
    }
    if (1 >= _card_multi)
    {
        return;
    }
    _card_multi -= 1;
    UtilNet::sendSetCardMulti(_card_multi);
}
//设置卡片倍数信息处理
void RoomScene::netSetCardMultiCallback(bool success, int multi)
{
    if (false == success)
    {
        return;
    }

    _card_multi = multi;
    _label_multi->setString(StringUtils::format("%d", _card_multi));
}
