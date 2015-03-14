//
//  GameScene.cpp
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/25.
//
//

#include "GameScene.h"
#include "RoomScene.h"

#include "CardBroad.h"

#include "game/GameStage.h"
#include "game/Net.h"

#include "data/MyData.h"
#include "data/Struct.h"
#include "data/UserData.h"
#include "data/Card.h"

USING_NS_CC;

GameScene::GameScene()
: _game(nullptr),
  _infobroad_self(nullptr),
  _bInited(false),
  _root_card(nullptr), _root_last(nullptr),
  _round(0)
{
}

GameScene* _gGameScene = nullptr;

GameScene* GameScene::getInstance()
{
    return _gGameScene;
}

Scene* GameScene::createScene()
{
    auto scene = Scene::create();

    auto layer = GameScene::create();

    scene->addChild(layer);

    _gGameScene = layer;

    return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto label = Label::createWithTTF("Game", "fonts/heiti.ttf", 72);
    label->setColor(Color3B::BLACK);

    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    auto closeItem = MenuItemImage::create("Exit_N.png",
                                           "Exit_S.png",
                                           CC_CALLBACK_1(GameScene::menuReturnRoomScene, this));

    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + visibleSize.height - closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto closemenu = Menu::create(closeItem, NULL);
    closemenu->setPosition(Vec2::ZERO);
    this->addChild(closemenu, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("LoadSceneBack.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);


    auto passItem = MenuItemImage::create("Pass_N.png",
                                          "Pass_S.png",
                                          CC_CALLBACK_1(GameScene::menuPassCallback, this));
    auto pushItem = MenuItemImage::create("Push_N.png",
                                          "Push_S.png",
                                          CC_CALLBACK_1(GameScene::menuPushCallback, this));
    passItem->setPosition(Vec2(0, 20+90));
    pushItem->setPosition(Vec2(0, -20-90));

    auto menu = Menu::create(passItem, pushItem, nullptr);
    menu->setPosition(Vec2(1300, 300));
    this->addChild(menu, 0);

    UtilNet::sendGetRoomGameData(MyData::getInstance()->getCurrentRoomId());

    return true;
}

//引擎框架 在加载时调用
void GameScene::onEnter()
{
    Layer::onEnter();
}

//返回房间场景
void GameScene::menuReturnRoomScene(cocos2d::Ref* pSender)
{
    UtilNet::sendLeaveRoom();
    Director::getInstance()->replaceScene(RoomScene::createScene());
}

//生成游戏状态
bool GameScene::genGameStage(NetGameData &gamedata)
{
    _game = GameStage::create();
    _game->retain();

    _game->genStage(gamedata);
    return true;
}

//查询当前的游戏状态
GameStage* GameScene::getStage(void)
{
    return _game;
}

//生成自身的信息面板
bool GameScene::genSelfInfoBroad(void)
{
    _infobroad_self = PlayerInfoBroad::create();

    _infobroad_self->setUid(_game->getSelfUid());

    this->addChild(_infobroad_self);
    _infobroad_self->setPosition(Vec2(750, 270));

    return true;
}
//更新自身的信息面板
bool GameScene::updateSelfInfoBroad(void)
{
    _infobroad_self->updateInfo();
    return true;
}

//生成其他玩家的信息面板
bool GameScene::genOtherInfoBroad(void)
{
    auto uids = _game->getAllUids();

    //计算圆形显示的间隔距离
    int num = (int)uids.size() - 1;
    double start_r = 20 * 3.14159 / 180;
    double step_r = 2 * 3.14159 / (double)num;

    for (auto& e : uids)
    {
        if (e == _game->getSelfUid())
        {
            continue;
        }

        auto infobroad = PlayerInfoBroad::create();
        _infobroad_others.insert(e, infobroad);

        infobroad->setUid(e);

        this->addChild(infobroad);

        auto pos = Vec2(400 * sin(start_r) + 750, 400 * cos(start_r) + 1968);

        infobroad->setPosition(pos);

        start_r += step_r;
    }

    return true;
}
//更新其他玩家的信息面板
bool GameScene::updateOtherInfoBroad(void)
{
    for (auto& other : _infobroad_others)
    {
        other.second->updateInfo();
    }
    return true;
}


//获取游戏数据信息处理
void GameScene::netGetGameDataCallback(NetGameData &gamedata)
{
    //生成游戏状态
    this->genGameStage(gamedata);

    //启动定时器拉取下一次数据
    this->schedule(schedule_selector(GameScene::timeGetGameStatus), 1);
}

//查看房间内信息处理
void GameScene::netGetGameStatusCallback(NetGameStatus &gamestatus)
{
    this->unschedule(schedule_selector(GameScene::timeGetGameStatus));

    if (false == _bInited)
    {
        this->initGame(gamestatus);
    }
    else {
        this->refreshGame(gamestatus);
    }

    //启动定时器拉取下一次数据
    this->schedule(schedule_selector(GameScene::timeGetGameStatus), 1);
}

//初始化游戏
bool GameScene::initGame(NetGameStatus &gamestatus)
{
    //生成玩家信息面板
    this->genSelfInfoBroad();
    //生成其他玩家信息面板
    this->genOtherInfoBroad();

    //生成本地玩家卡牌的面板
    this->genLocalCardBroad();

    _bInited = true;

    return true;
}
//刷新游戏状态
bool GameScene::refreshGame(NetGameStatus &gamestatus)
{
    //更新游戏状态
    _game->updateStage(gamestatus);

    this->updateSelfInfoBroad();
    this->updateOtherInfoBroad();

    this->refreshLastCards(gamestatus);

    if (_game->getRound() != _round)
    {
        _round = _game->getRound();
        this->genLocalCardBroad();
    }

    return true;
}

//生成本地玩家卡牌的面板
bool GameScene::genLocalCardBroad(void)
{
    auto cards = _game->getUserData(MyData::getInstance()->getMyUid())->getOwnCards();

    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    if (nullptr != _root_card)
    {
        _root_card->removeFromParent();
        _root_card = nullptr;
    }
    _root_card = Node::create();
    _root_card->setPosition(origin.x + 150, origin.y + 1200);
    this->addChild(_root_card);

    Vec2 pos[10] = {
        Vec2(0,0),
        Vec2(75, -125),
        Vec2(425, -125),
        Vec2(775, -125),
        Vec2(1125, -125),
        Vec2(75, -525),
        Vec2(425, -525),
        Vec2(775, -525),
        Vec2(1125, -525),
        Vec2(75, -925),
    };

    for (auto& cid : cards)
    {
        auto c = _game->getCard(cid);
        auto cb = CardBroad::create();
        cb->setTag(cid);
        cb->setPosition(pos[c->getType()]);

        cb->setCid(cid);

        pos[c->getType()].x += 50;

        _root_card->addChild(cb);
    }

    return true;
}

//定时发送获取游戏状态信息的请求
void GameScene::timeGetGameStatus(float dt)
{
    UtilNet::sendGetRoomGameStatus(MyData::getInstance()->getCurrentRoomId());
}


//卡片选中
bool GameScene::selectCard(int cid)
{
    _select_card.push_back(cid);
    return true;
}
//卡片取消选中
bool GameScene::canselSelectCard(int cid)
{
    for (auto it = _select_card.begin(); it != _select_card.end(); it++)
    {
        if (cid == *it)
        {
            _select_card.erase(it);
            return true;
        }
    }
    return false;
}
//Jocker选中
bool GameScene::selectJocker(int jocker_id, int to_cid)
{
    for (auto i = 0; i < _select_jocker_card.size(); i++)
    {
        if (_select_jocker_card.at(i) == jocker_id)
        {
            _select_jocker_card[i+1] = to_cid;
            return true;
        }
    }

    _select_jocker_card.push_back(jocker_id);
    _select_jocker_card.push_back(to_cid);
    return true;
}

//过牌按键处理
void GameScene::menuPassCallback(cocos2d::Ref* pSender)
{
    UtilNet::sendPassTurn(MyData::getInstance()->getCurrentRoomId());
}
//出牌按键处理
void GameScene::menuPushCallback(cocos2d::Ref* pSender)
{
    UtilNet::sendPushCards(MyData::getInstance()->getCurrentRoomId(), _select_card, _select_jocker_card);
}

//出牌信息处理
void GameScene::netPushCardsCallback(bool bSuccess)
{
    if (false == bSuccess)
    {
        for (auto& sc : _select_card)
        {
            static_cast<CardBroad*>(_root_card->getChildByTag(sc))->moveBack();
        }
        _select_card.clear();
        _select_jocker_card.clear();
    }
    else
    {
        for (auto& sc : _select_card)
        {
            _root_card->removeChildByTag(sc);
        }
        _select_card.clear();
        _select_jocker_card.clear();
    }
}

//刷新上次出的牌
bool GameScene::refreshLastCards(NetGameStatus &gamestatus)
{
    if (nullptr == _root_last)
    {
        _root_last = Node::create();
        _root_last->setPosition(750, 1968);
        this->addChild(_root_last);
    }

    _root_last->removeAllChildren();

    auto num = gamestatus.LastStep.size();
    if (0 == gamestatus.LastStepOwn || 0 == num)
    {
        return true;
    }

    Vec2 pos = Vec2(0,0);
    pos.x = (num - 1) * -0.5 * 50;

    for (auto& cid : gamestatus.LastStep) {
        auto cb = CardBroad::create();
        cb->setTag(cid);

        cb->setCid(cid);

        cb->setPosition(pos);

        _root_last->addChild(cb);

        pos.x += 50;
    }

    auto lastown = _game->getUserData(gamestatus.LastStepOwn);
    if (nullptr == lastown)
    {
        return true;
    }
    auto lastownlabel = Label::createWithTTF(lastown->getUname(), "fonts/heiti.ttf", 64);
    lastownlabel->setTextColor(Color4B::BLACK);

    auto ownBroad = Sprite::create("LastOwnBroad.png");
    ownBroad->setPosition(Vec2(0, -100));
    _root_last->addChild(ownBroad);

    lastownlabel->setPosition(Vec2(ownBroad->getContentSize() / 2));
    ownBroad->addChild(lastownlabel);

    return true;
}
