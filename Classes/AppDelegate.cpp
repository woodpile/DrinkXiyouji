#include "AppDelegate.h"
#include "LoadScene.h"

#include "game/Net.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("My Game");
        glview->setFrameSize(375, 668);
        director->setOpenGLView(glview);
    }

    glview->setDesignResolutionSize(1500, 2668, ResolutionPolicy::NO_BORDER);
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();
    log("visibleSize: %f, %f.", visibleSize.width, visibleSize.height);
    log("origin: %f, %f.", origin.x, origin.y);
    auto ws = Director::getInstance()->getWinSize();
    log("win: %f, %f.", ws.width, ws.height);
    auto gs = Director::getInstance()->getOpenGLView()->getFrameSize();
    log("gs: %f, %f.", gs.width, gs.height);

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    UtilNet::regRecvList();

    // create a scene. it's an autorelease object
    auto scene = LoadScene::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
