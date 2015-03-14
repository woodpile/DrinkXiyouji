//
//  PlayerInfoBroad.cpp
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/26.
//
//

#include "PlayerInfoBroad.h"
#include "GameScene.h"
#include "GameStage.h"
#include "data/UserData.h"

USING_NS_CC;

PlayerInfoBroad::PlayerInfoBroad()
: _uid(-1),
  _label_name(nullptr), _label_cardnum(nullptr), _label_totalpunish(nullptr),
  _tag_turn(nullptr)
{
}

//引擎框架 初始化个体 在create中被调用
bool PlayerInfoBroad::init()
{
    if ( !Sprite::init() )
    {
        return false;
    }

    return true;
}

//创建一个玩家信息面板对象
PlayerInfoBroad* PlayerInfoBroad::create(void)
{
    PlayerInfoBroad *pib = new (std::nothrow) PlayerInfoBroad();
    if (pib)
    {
        if (pib->init() && pib->initWithFile("PlayerInfoBroad.png"))
        {
            pib->autorelease();
            return pib;
        }
    }
    CC_SAFE_DELETE(pib);
    return nullptr;
}

//引擎框架 在加载时调用
void PlayerInfoBroad::onEnter()
{
    Sprite::onEnter();

    this->showName();
    this->showCardNum();
    this->showTotalPunish();
    this->showTurnTag();
}

//设置一个玩家的uid
bool PlayerInfoBroad::setUid(int uid)
{
    _uid = uid;
    return true;
}

//显示玩家名字
void PlayerInfoBroad::showName(void)
{
    auto game = GameScene::getInstance()->getStage();
    auto data = game->getUserData(_uid);
    if (nullptr == data) {
        return;
    }

    if (nullptr != _label_name)
    {
        _label_name->removeFromParent();
        _label_name = nullptr;
    }

    _label_name = Label::createWithTTF(data->getUname(), "fonts/heiti.ttf", 64);
    _label_name->setColor(Color3B::BLACK);
    _label_name->setPosition(Vec2(230,150));
    this->addChild(_label_name);
}

//显示玩家持有牌数
void PlayerInfoBroad::showCardNum(void)
{
    auto game = GameScene::getInstance()->getStage();
    auto data = game->getUserData(_uid);
    if (nullptr == data) {
        return;
    }

    if (nullptr != _label_cardnum)
    {
        _label_cardnum->removeFromParent();
        _label_cardnum = nullptr;
    }

    auto str = StringUtils::format("卡%d", data->getCardNum());
    _label_cardnum = Label::createWithTTF(str, "fonts/heiti.ttf", 56);
    _label_cardnum->setColor(Color3B::BLACK);
    _label_cardnum->setPosition(Vec2(200,50));
    this->addChild(_label_cardnum);
}

//显示玩家累计惩罚数量
void PlayerInfoBroad::showTotalPunish(void)
{
    auto game = GameScene::getInstance()->getStage();
    auto data = game->getUserData(_uid);
    if (nullptr == data) {
        return;
    }

    if (nullptr != _label_totalpunish)
    {
        _label_totalpunish->removeFromParent();
        _label_totalpunish = nullptr;
    }

    auto str = StringUtils::format("罚%d", data->getTotalPunish());
    _label_totalpunish = Label::createWithTTF(str, "fonts/heiti.ttf", 56);
    _label_totalpunish->setColor(Color3B::BLACK);
    _label_totalpunish->setPosition(Vec2(300,50));
    this->addChild(_label_totalpunish);
}

//显示当前出手标志
void PlayerInfoBroad::showTurnTag(void)
{
    if (_uid == GameScene::getInstance()->getStage()->getCurrentTurn())
    {
        if (nullptr == _tag_turn)
        {
            _tag_turn = Sprite::create("CurrentTurn.png");
            _tag_turn->setPosition(50, 150);
            this->addChild(_tag_turn);
        }
    }
    else
    {
        if (nullptr != _tag_turn)
        {
            _tag_turn->removeFromParent();
            _tag_turn = nullptr;
        }
    }
}

//更新玩家信息显示
bool PlayerInfoBroad::updateInfo(void)
{
    auto game = GameScene::getInstance()->getStage();
    auto data = game->getUserData(_uid);
    if (nullptr == data) {
        return false;
    }

    auto str = StringUtils::format("卡\n%d", data->getCardNum());

    if (nullptr == _label_cardnum) {
        _label_cardnum = Label::createWithTTF(str, "fonts/heiti.ttf", 56);
        _label_cardnum->setColor(Color3B::BLACK);
        _label_cardnum->setPosition(Vec2(200,50));
        this->addChild(_label_cardnum);
    }
    else {
        _label_cardnum->setString(str);
    }

    auto str_punish = StringUtils::format("罚\n%d", data->getTotalPunish());

    if (nullptr == _label_totalpunish) {
        _label_totalpunish = Label::createWithTTF(str_punish, "fonts/heiti.ttf", 56);
        _label_totalpunish->setColor(Color3B::BLACK);
        _label_totalpunish->setPosition(Vec2(300,50));
        this->addChild(_label_totalpunish);
    }
    else {
        _label_totalpunish->setString(str_punish);
    }

    this->showTurnTag();

    return true;
}


