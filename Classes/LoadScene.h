#ifndef __LOAD_SCENE_H__
#define __LOAD_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class LoadScene : public cocos2d::Layer, public cocos2d::ui::EditBoxDelegate
{
public:
    LoadScene();
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();
    static LoadScene* getInstance();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    //登录服务器菜单键
    void menuLoginCallback(cocos2d::Ref* pSender);
    //进入游戏场景菜单键
    void menuStartCallback(cocos2d::Ref* pSender);
    //设置用户名菜单键
    void menuSetNameCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(LoadScene);
    
    //登录返回信息处理
    void netLoginCallback(int uid);

    virtual void editBoxReturn(cocos2d::ui::EditBox* editBox);

private:
    //主菜单
    cocos2d::Menu* _menu_main;
    //用户名输入框
    cocos2d::ui::EditBox* _input_name;
    //输入的用户名
    std::string _uname;
    //已存在的用户名标签
    cocos2d::Label* _label_uname;

    //生成登录前菜单
    void genMenuBeforeLogin(void);
    //生成登录后菜单
    void genMenuAfterLogin(void);
};

#endif // __LOAD_SCENE_H__
