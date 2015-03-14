//
//  GameScene.h
//  DrinkXiyouji
//
//  Created by woodpile on 15/2/25.
//
//

#ifndef __DrinkXiyouji__GameScene__
#define __DrinkXiyouji__GameScene__

#include "cocos2d.h"
#include "PlayerInfoBroad.h"

class GameStage;
class NetGameData;
class NetGameStatus;

class GameScene : public cocos2d::Layer
{
public:
    GameScene();
    static cocos2d::Scene* createScene();
    static GameScene* getInstance();

    virtual bool init();
    //引擎框架 在加载是调用
    virtual void onEnter();

    CREATE_FUNC(GameScene);

    //查询当前的游戏状态
    GameStage* getStage(void);

    //返回房间场景
    void menuReturnRoomScene(cocos2d::Ref* pSender);
    //过牌按键处理
    void menuPassCallback(cocos2d::Ref* pSender);
    //出牌按键处理
    void menuPushCallback(cocos2d::Ref* pSender);

    //获取游戏数据信息处理
    void netGetGameDataCallback(NetGameData &gamedata);
    //查看房间内信息处理
    void netGetGameStatusCallback(NetGameStatus &gamestatus);
    //出牌信息处理
    void netPushCardsCallback(bool bSuccess);

    //卡片选中
    bool selectCard(int cid);
    //卡片取消选中
    bool canselSelectCard(int cid);
    //Jocker选中
    bool selectJocker(int jocker_id, int to_cid);
    
private:
    //生成游戏状态
    bool genGameStage(NetGameData &gamedata);
    //当前的游戏状态
    GameStage* _game;
    //是否已初始化状态
    bool _bInited;

    //初始化游戏
    bool initGame(NetGameStatus &gamestatus);
    //刷新游戏状态
    bool refreshGame(NetGameStatus &gamestatus);

    //生成自身的信息面板
    bool genSelfInfoBroad(void);
    //更新自身的信息面板
    bool updateSelfInfoBroad(void);
    //自身的信息面板
    PlayerInfoBroad* _infobroad_self;

    //生成其他玩家的信息面板
    bool genOtherInfoBroad(void);
    //更新其他玩家的信息面板
    bool updateOtherInfoBroad(void);
    //其他玩家的信息面板
    cocos2d::Map<int, PlayerInfoBroad*> _infobroad_others;

    //生成本地玩家卡片的面板
    bool genLocalCardBroad(void);
    //各卡片面板的根节点
    cocos2d::Node* _root_card;

    //定时发送获取游戏状态信息的请求
    void timeGetGameStatus(float dt);

    //选中的卡片
    std::vector<int> _select_card;
    //Jocker卡变化
    std::vector<int> _select_jocker_card;

    //刷新上次出的牌
    bool refreshLastCards(NetGameStatus &gamestatus);
    //上次出的牌的根节点
    cocos2d::Node* _root_last;

    //当前的轮次
    int _round;
};

#endif /* defined(__DrinkXiyouji__GameScene__) */
