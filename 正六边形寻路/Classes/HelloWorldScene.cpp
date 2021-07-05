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
#include "cocostudio/CocoStudio.h"
#include "TKGridAStar.h"
#include "TKHexagonGrid.h"

USING_NS_CC;

const int colGridNum = 16;

static std::map<int,std::vector<TKGridAStar::MapData>> mapVec;

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

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	auto closeItem = MenuItemImage::create(
										   "CloseNormal.png",
										   "CloseSelected.png",
										   CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

	if (closeItem == nullptr ||
		closeItem->getContentSize().width <= 0 ||
		closeItem->getContentSize().height <= 0)
	{
		problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
	}
	else
	{
		float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
		float y = origin.y + closeItem->getContentSize().height/2;
		closeItem->setPosition(Vec2(x,y));
	}

	// create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	/////////////////////////////
	// 3. add your codes below...

	// add a label shows "Hello World"
	// create and initialize a label
	
	FileUtils::getInstance()->addSearchPath("H:/Test317/Resources");

	

	m_touchListener = EventListenerTouchOneByOne::create();
	m_touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	m_touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
	m_touchListener->setSwallowTouches(false);
	m_touchListener->setEnabled(true);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_touchListener, this);

	m_node = Node::create();
	this->addChild(m_node,1);

	this->addSixSideGrid();
	return true;
}

void HelloWorld::addSixSideGrid()
{
	char buff[32] = { 0 };
	std::vector<TKGridAStar::MapData> childVec;
	TKGridAStar::MapData tmpData;
	int rowGridNum = 0;
	auto size = Director::getInstance()->getWinSize();
	int width = size.width;
	rowGridNum = width / (3 * cubeSideLen)*2;
	rowGridNum += (width / (3 * cubeSideLen) > (2 * cubeSideLen) ? 1 : 0);

	int halfX = ceil(rowGridNum * 0.5);
	int halfY = ceil(colGridNum * 0.5);
	Grid grid;

	GridHelper::setCenterPos(size.width * 0.5, size.height * 0.5);
	for (int i = -halfX; i <= halfX; i++)
	{
		childVec.clear();
		for (int j = -halfY; j < halfY; ++j) {
			tmpData.grid.x = i;
			tmpData.grid.y = j - i;
			tmpData.grid.z = -1 * (tmpData.grid.x + tmpData.grid.y);

			GridHelper::getPosByGrid(i, j, grid);
			if (grid.posY>=-cubeSideLen && grid.posY<=size.height+ cubeSideLen)
			{
				auto sp = Sprite::create("six_side.png");
				sp->setPosition(grid.posX, grid.posY);
				this->addChild(sp);

				auto txt = ui::Text::create();
				sprintf(buff, "%d,%d,%d", tmpData.grid.x, tmpData.grid.y, tmpData.grid.z);
				txt->setString(buff);
				txt->setTextColor(Color4B::BLACK);
				txt->setFontSize(24);
				sp->addChild(txt);
				txt->setPosition(sp->getContentSize() * 0.5);

				tmpData.grid.posX = grid.posX;
				tmpData.grid.posY = grid.posY;
				/*
				tmpData.isSpace = !(tmpData.grid.x == 7 && tmpData.grid.y == -1
					|| tmpData.grid.x == 8 && tmpData.grid.y == -1
					|| tmpData.grid.x == 8 && tmpData.grid.y == 0);
				if (!tmpData.isSpace) {
					sp->initWithFile("button_cwyc_sb.png");
				}
				*/
				childVec.push_back(tmpData);
			}
		}

		mapVec[i]=childVec;
	}
	ASTAR.setMapData(mapVec);
}

bool HelloWorld::onTouchBegan(Touch* touch, Event* unused_event)
{
	auto pos = touch->getLocation();
	Grid grid;
	GridHelper::getGridByPos(pos.x,pos.y, grid);
	CCLOG("goal->%d,%d", (int)grid.x, (int)grid.y);
	findPath(Vec2(0,0), Vec2(grid.x, grid.y));
	return true;
}

void HelloWorld::onTouchMoved(Touch* touch, Event* unused_event)
{
	auto pos = touch->getLocation();
	Grid grid;
	GridHelper::getGridByPos(pos.x, pos.y, grid);
	CCLOG("goal->%d,%d", (int)grid.x, (int)grid.y);
	findPath(Vec2(0, 0), Vec2(grid.x, grid.y));
}

void HelloWorld::findPath(const Vec2& startPos, const Vec2& endPos)
{
	std::vector<Grid> pathVec;

	Grid startGrid = Grid(startPos.x, startPos.y, 0);
	auto p = ASTAR.getMapDataByGrid(startGrid);
	startGrid.posX = p->grid.posX;
	startGrid.posY = p->grid.posY;
	startGrid.z = p->grid.z;

	Grid endGrid = Grid(endPos.x, endPos.y,0);
	p= ASTAR.getMapDataByGrid(endGrid);
	if (!p)
	{
		return;
	}
	endGrid.posX = p->grid.posX;
	endGrid.posY = p->grid.posY;
	endGrid.z = p->grid.z;

	ASTAR.findPath(startGrid, endGrid, pathVec);

	m_node->removeAllChildren();

	std::vector<Vec2> posVec;
	posVec.push_back(Vec2(endGrid.posX, endGrid.posY));

	for (auto aa: pathVec)
	{
		posVec.push_back(Vec2(aa.posX, aa.posY));
	}
	
	int num = posVec.size();
	for (int i = 0; i < num-1; ++i)
	{
		auto pos = posVec[i];
		DrawNode* drawNode = DrawNode::create();
		//drawNode->setPosition(pos);
		m_node->addChild(drawNode);
		drawNode->drawLine(pos, posVec[i+1], Color4F::GREEN);
	}

	int min = std::min(abs(startGrid.x - endGrid.x), abs(startGrid.y - endGrid.y));
	min = std::min(min, abs(startGrid.z - endGrid.z));
	CCLOG("min-------%d", min);
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
	CCLOG("--------start----------");
	int radius = 5;
	std::vector<Grid> results;
	GridHelper::getGridRingsByRadius(Grid(0, 0, 0),radius,results);
}
