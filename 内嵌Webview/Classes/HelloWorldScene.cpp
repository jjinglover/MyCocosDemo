/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"

#if (CC_TARGET_PLATFORM==CC_PLATFORM_IOS)
#import "privacyPolicyViewCpp.h"
#endif

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#endif

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto size = Director::getInstance()->getWinSize();
    ui::Button *clickBtn=ui::Button::create();
    clickBtn->setTitleText("Click Load Webview");
    clickBtn->setTitleFontSize(36);
    clickBtn->setPosition(size*0.5);
    clickBtn->addClickEventListener(CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    this->addChild(clickBtn);
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    const char* url="https://www.baidu.com";
    const char* title="Privacy Privacy";
#if (CC_TARGET_PLATFORM==CC_PLATFORM_IOS)
    loadPrivacyPolicyView(url, title);
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "org/cocos2dx/cpp/AppActivity", "loadPolicyView", "(Ljava/lang/String;Ljava/lang/String;)V")) {
        jstring str0 = t.env->NewStringUTF(url);
        jstring str1 = t.env->NewStringUTF(title);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, str0, str1);
        t.env->DeleteLocalRef(str0);
        t.env->DeleteLocalRef(str1);
        t.env->DeleteLocalRef(t.classID);
    }
#endif
}
