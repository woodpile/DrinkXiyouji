#include "LoadScene.h"
#include "GameScene.h"
#include "RoomScene.h"

#include "data/MyData.h"
#include "game/Net.h"

USING_NS_CC;

LoadScene::LoadScene()
: _input_name(nullptr), _menu_main(nullptr), _label_uname(nullptr)
{
}

LoadScene* _gLoadScene = nullptr;

LoadScene* LoadScene::getInstance()
{
    return _gLoadScene;
}

Scene* LoadScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = LoadScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    _gLoadScene = layer;

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool LoadScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create("Exit_N.png",
                                           "Exit_S.png",
                                           CC_CALLBACK_1(LoadScene::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + visibleSize.height - closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto closemenu = Menu::create(closeItem, NULL);
    closemenu->setPosition(Vec2::ZERO);
    this->addChild(closemenu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = Label::createWithTTF("XiYouJi", "fonts/heiti.ttf", 72);
    label->setColor(Color3B::BLACK);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("LoadSceneBack.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);

    _menu_main = Menu::create();
    _menu_main->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
    this->addChild(_menu_main);

    if (0 >= MyData::getInstance()->getMyUid())
    {
        this->genMenuBeforeLogin();
    }
    else
    {
        this->genMenuAfterLogin();
    }

    return true;
}

//生成登录前菜单
void LoadScene::genMenuBeforeLogin(void)
{
    auto loginItem = MenuItemImage::create("Login_N.png",
                                           "Login_S.png",
                                           CC_CALLBACK_1(LoadScene::menuLoginCallback, this));
    loginItem->setPosition(Vec2(0,300));
    _menu_main->addChild(loginItem);
}
//生成登录后菜单
void LoadScene::genMenuAfterLogin(void)
{
    _menu_main->removeAllChildren();

    auto setNameItem = MenuItemImage::create("SetName_N.png",
                                             "SetName_S.png",
                                             CC_CALLBACK_1(LoadScene::menuSetNameCallback, this));
    setNameItem->setPosition(Vec2(0,0));
    _menu_main->addChild(setNameItem);

    auto startItem = MenuItemImage::create("Start_N.png",
                                           "Start_S.png",
                                           CC_CALLBACK_1(LoadScene::menuStartCallback, this));
    startItem->setPosition(Vec2(0,-600));
    _menu_main->addChild(startItem);

    if (MyData::getInstance()->getMyUname().size() != 0)
    {
        Size visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();

        _label_uname = Label::createWithTTF(MyData::getInstance()->getMyUname(),
                                            "fonts/heiti.ttf", 144);
        _label_uname->setTextColor(Color4B::BLACK);
        _label_uname->setPosition(origin.x + 0.5 * visibleSize.width, origin.y + visibleSize.height - 500);
        this->addChild(_label_uname);
    }
}

void LoadScene::menuLoginCallback(Ref* pSender)
{
    //登录到服务器
    UtilNet::sendUserLogin();
}

void LoadScene::menuStartCallback(Ref* pSender)
{
    if (nullptr != _input_name && _uname.size() != 0 &&
        MyData::getInstance()->getMyUname().size() == 0)
    {
        UtilNet::sendUserSetName(_uname);
        MyData::getInstance()->setMyUname(_uname);
    }
    Director::getInstance()->replaceScene(RoomScene::createScene());
}
//设置用户名菜单键
void LoadScene::menuSetNameCallback(cocos2d::Ref* pSender)
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    if (nullptr != _input_name)
    {
        _input_name->removeFromParent();
        _input_name = nullptr;
    }

    if (nullptr != _label_uname)
    {
        _label_uname->removeFromParent();
        _label_uname = nullptr;
    }

    _input_name = ui::EditBox::create(Size(1152, 144), "EditBoxBroad.png");
    _input_name->setPosition(Vec2(origin.x + 0.5 * visibleSize.width, origin.y + visibleSize.height - 500));
    _input_name->setFont("fonts/heiti.ttf", 120);
    _input_name->setFontColor(Color3B::BLACK);
    _input_name->setPlaceHolder("玩家名:");
    _input_name->setPlaceholderFontColor(Color3B::GRAY);
    _input_name->setMaxLength(8);
    _input_name->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
    _input_name->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    _input_name->setDelegate(this);

    this->addChild(_input_name);
}
void LoadScene::editBoxReturn(cocos2d::ui::EditBox* editBox)
{
    _uname = std::string(editBox->getText());
}

void LoadScene::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

//登录返回信息处理
void LoadScene::netLoginCallback(int uid)
{
    MyData::getInstance()->setMyUid(uid);

    this->genMenuAfterLogin();
}