//
//  CardBroad.cpp
//  DrinkXiyouji
//
//  Created by woodpile on 15/3/4.
//
//

#include "CardBroad.h"

#include "GameScene.h"
#include "game/GameStage.h"

#include "data/Card.h"

USING_NS_CC;

CardBroad::CardBroad()
: _cid(0),
  _bSelect(false), _orig_pos(Vec2(0,0)),
  _menu_type_select(nullptr), _menu_order_select(nullptr),
  _jocker_select_type(0), _jocker_select_order(0)
{
}

//引擎框架 初始化个体 在create中被调用
bool CardBroad::init()
{
    if ( !Sprite::init() )
    {
        return false;
    }

    //触摸监听
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    //引擎 向触摸监听注册响应函数
    listener->onTouchBegan = CC_CALLBACK_2(CardBroad::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CardBroad::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CardBroad::onTouchEnded, this);
    //激活触摸监听
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

//创建一个玩家信息面板对象
CardBroad* CardBroad::create(void)
{
    CardBroad *pib = new (std::nothrow) CardBroad();
    if (pib)
    {
        if (pib->init() && pib->initWithFile("CardBroad.png"))
        {
            pib->autorelease();
            return pib;
        }
    }
    CC_SAFE_DELETE(pib);
    return nullptr;
}

//引擎框架 在加载时调用
void CardBroad::onEnter()
{
    Sprite::onEnter();

    this->genAttrTag();
}

//设置卡片id
bool CardBroad::setCid(int cid)
{
    _cid = cid;
    return true;
}
//查看卡片id
int CardBroad::getCid(void)
{
    return _cid;
}

const std::string g_cardname[10] = {
    "",
    "CardTagShifu.png",
    "CardTagWukong.png",
    "CardTagBajie.png",
    "CardTagShaseng.png",
    "CardTagLaoYaojing.png",
    "CardTagDaYaojing.png",
    "CardTagZhongYaojing.png",
    "CardTagXiaoYaojing.png",
    "CardTagJocker.png",

};

//生成卡片属性标记
void CardBroad::genAttrTag(void)
{
    auto card = GameScene::getInstance()->getStage()->getCard(_cid);
    if (nullptr == card)
    {
        return;
    }

    auto tag = Sprite::create(g_cardname[card->getType()]);
    tag->setPosition(Vec2(35, this->getContentSize().height - 35));
    this->addChild(tag);

    auto str = StringUtils::format("%d", card->getOrder());
    auto label = Label::createWithTTF(str, "fonts/heiti.ttf", 45);
    label->setTextColor(Color4B::BLACK);
    label->setPosition(Vec2(35, this->getContentSize().height - 85));
    this->addChild(label);

    if (card->getType() == Card::TYPE_JOCKER)
    {
        auto to_card = GameScene::getInstance()->getStage()->getCard(card->getJockerChanged());
        if (nullptr != to_card)
        {
            auto tag = Sprite::create(g_cardname[to_card->getType()]);
            tag->setPosition(Vec2(35, this->getContentSize().height - 135));
            this->addChild(tag);

            auto str = StringUtils::format("%d", to_card->getOrder());
            auto label = Label::createWithTTF(str, "fonts/heiti.ttf", 45);
            label->setTextColor(Color4B::BLACK);
            label->setPosition(Vec2(35, this->getContentSize().height - 185));
            this->addChild(label);
        }
    }
}

//引擎 触摸行为注册函数
bool CardBroad::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    //检查触摸点
    Point point = this->convertTouchToNodeSpaceAR(touch);
    auto s = this->getContentSize();
    bool ret = Rect(-s.width / 2, -s.height / 2, s.width, s.height).containsPoint(point);
    return ret;
}
void CardBroad::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
}
void CardBroad::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    auto card = GameScene::getInstance()->getStage()->getCard(_cid);
    if (nullptr == card)
    {
        return;
    }

    if (false == _bSelect)
    {
        _orig_pos = this->getPosition();
        this->setPositionY(_orig_pos.y + 80);
        _bSelect = true;
        GameScene::getInstance()->selectCard(_cid);

        if (card->getType() == Card::TYPE_JOCKER)
        {
            this->showJockerSelectUI();
        }
    }
    else
    {
        this->setPositionY(_orig_pos.y);
        _bSelect = false;
        GameScene::getInstance()->canselSelectCard(_cid);
    }
}

//恢复位置
void CardBroad::moveBack(void)
{
    this->setPositionY(_orig_pos.y);
    _bSelect = false;
}

//显示Jocker选择界面
void CardBroad::showJockerSelectUI(void)
{
    if (nullptr != _menu_type_select)
    {
        _menu_type_select->removeFromParent();
        _menu_type_select = nullptr;
    }

    _menu_type_select = Menu::create();
    _menu_type_select->setPosition(Vec2(this->getContentSize() / 2));
    this->addChild(_menu_type_select);

    Vec2 pos(0,0);
    for (auto i = Card::TYPE_SHIFU; i <= Card::TYPE_XIAOYAOJING; i++)
    {
        auto cItem = MenuItemImage::create(g_cardname[i],
                                           g_cardname[i],
                                           CC_CALLBACK_1(CardBroad::menuJockerTypeCallback, this));
        cItem->setPosition(pos);
        cItem->setScale(3);
        cItem->setTag(i);
        cItem->setZOrder(i);
        _menu_type_select->addChild(cItem);

        pos.x += cItem->getContentSize().width * 3;
    }
}

//Jocker类型选择
void CardBroad::menuJockerTypeCallback(cocos2d::Ref* pSender)
{
    static_cast<MenuItemImage*>(pSender)->setZOrder(100);

    _jocker_select_type = static_cast<MenuItemImage*>(pSender)->getTag();

    auto children = _menu_type_select->getChildren();
    for (auto it = children.begin(); it != children.end(); it++)
    {
        (*it)->runAction(MoveTo::create(0.2, Vec2(0,0)));
    }

    if (nullptr != _menu_order_select)
    {
        _menu_order_select->removeFromParent();
        _menu_order_select = nullptr;
    }

    _menu_order_select = Menu::create();
    _menu_order_select->setPosition(_menu_type_select->getPosition() +
                                    Vec2(static_cast<MenuItemImage*>(pSender)->getContentSize().width * 3,0));
    this->addChild(_menu_order_select);

    Vec2 pos(0,0);
    for (auto i = 1; i <= 4; i++)
    {
        auto oItem = MenuItemImage::create("CardTagBlank.png",
                                           "CardTagBlank.png",
                                           CC_CALLBACK_1(CardBroad::menuJockerOrderCallback, this));
        oItem->setScale(3);
        oItem->setTag(i);

        auto ln = Label::createWithTTF(StringUtils::format("%d", i), "fonts/heiti.ttf", 45);
        ln->setTextColor(Color4B::BLACK);
        ln->setPosition(Vec2(oItem->getContentSize() / 2));
        oItem->addChild(ln);
        oItem->setZOrder(101 + i);

        oItem->setPosition(pos);
        pos.x += oItem->getContentSize().width * 3;

        _menu_order_select->addChild(oItem);
    }
}
//Jocker顺位选择
void CardBroad::menuJockerOrderCallback(cocos2d::Ref* pSender)
{
    static_cast<MenuItemImage*>(pSender)->setZOrder(200);

    _jocker_select_order = static_cast<MenuItemImage*>(pSender)->getTag();

    auto children = _menu_order_select->getChildren();
    for (auto it = children.begin(); it != children.end(); it++)
    {
        if ((*it)->getTag() == _jocker_select_order)
        {
            (*it)->runAction(Sequence::create(MoveTo::create(0.2, Vec2(0,0)),
                                              CallFuncN::create(CC_CALLBACK_1(CardBroad::selectOrderAfterCallback, this)),
                                              nullptr));
        }
        else
        {
            (*it)->runAction(MoveTo::create(0.2, Vec2(0,0)));
        }
    }
}

//jocker选择顺位后处理
void CardBroad::selectOrderAfterCallback(cocos2d::Node* sender)
{
    auto to_card = GameScene::getInstance()->getStage()->getCard(_jocker_select_type, _jocker_select_order);
    if (nullptr == to_card)
    {
        return;
    }

    GameScene::getInstance()->selectJocker(_cid, to_card->getId());

    _menu_type_select->removeFromParent();
    _menu_type_select = nullptr;
    _menu_order_select->removeFromParent();
    _menu_order_select = nullptr;

    auto tag = Sprite::create(g_cardname[to_card->getType()]);
    tag->setPosition(Vec2(35, this->getContentSize().height - 135));
    this->addChild(tag);

    auto str = StringUtils::format("%d", to_card->getOrder());
    auto label = Label::createWithTTF(str, "fonts/heiti.ttf", 45);
    label->setTextColor(Color4B::BLACK);
    label->setPosition(Vec2(35, this->getContentSize().height - 185));
    this->addChild(label);
}

