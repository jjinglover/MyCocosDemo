#include "HelloWorldScene.h"

USING_NS_CC;
using namespace RVO;
const float maxSpeed = 200;
Vector2 goalPos = Vector2(480, 320);
Vector2 bigGoalPos = Vector2(0, 0);

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    Scene *scene = Scene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}
HelloWorld::~HelloWorld()
{
	delete m_sim;
	m_sim = nullptr;
	
	_eventDispatcher->removeEventListener(m_touchListener);
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}
	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	m_touchListener = EventListenerTouchOneByOne::create();
	m_touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	m_touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
	m_touchListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
	m_touchListener->setEnabled(true);
	m_touchListener->setSwallowTouches(false);
	_eventDispatcher->addEventListenerWithFixedPriority(m_touchListener, -1);

	m_goalSp = Sprite::create("CloseSelected.png");
	m_goalSp->setPosition(goalPos.x(), goalPos.y());
	this->addChild(m_goalSp);

	this->initSet();
	scheduleUpdate();
	return true;
}

void HelloWorld::initSet()
{
	m_sim = new RVO::RVOSimulator();
	m_sim->setTimeStep(0.016f);
	m_sim->setAgentDefaults(60, 10, 1, 1, 20, maxSpeed);
	
	Vector2 p(100, 100);
	m_sim->addAgent(p);
	m_sim->setAgentMass(0, 1000.0f);

	m_bigSp = Sprite::create("big.png");
	m_bigSp->setPosition(p.x(), p.y());
	this->addChild(m_bigSp);

	for (int i = 0; i < 10; ++i)
	{
		Vector2 p(640, 320);
		m_sim->addAgent(p);
		auto sp = Sprite::create("ball.png");
		m_spVec.push_back(sp);
		sp->setPosition(640, 320);
		this->addChild(sp);

		auto txt = Label::create();
		txt->setString(std::to_string(i));
		sp->addChild(txt);
		txt->setTextColor(Color4B::BLACK);
		txt->setPosition(sp->getContentSize() * 0.5);
	}
}

void HelloWorld::update(float dt) 
{
	for (size_t i = 1; i < m_sim->getNumAgents(); ++i) {
		auto p = m_sim->getAgentPosition(i) - goalPos;
		if (absSq(p) < m_sim->getAgentRadius(i) * m_sim->getAgentRadius(i) * 2 * 2)
		{
			//到达目标，暂停
			m_sim->setAgentPrefVelocity(i, Vector2(0, 0));
		}
	}

	if (bigGoalPos.x()!=0)
	{
		unsigned int index = 0;
		auto p = m_sim->getAgentPosition(index) - bigGoalPos;
		if (absSq(p) < m_sim->getAgentRadius(index) * m_sim->getAgentRadius(index) * 2 * 2)
		{
			//到达目标，暂停
			m_sim->setAgentPrefVelocity(index, Vector2(0, 0));
		}
	}

	this->setPreferredVelocity();
	m_sim->doStep();
	this->updateSpPositon();
}

void HelloWorld::setPreferredVelocity()
{
	for (size_t i = 1; i < m_sim->getNumAgents(); ++i)
	{
		Vector2 goalVector = goalPos - m_sim->getAgentPosition(i);
		if (absSq(goalVector) > 1.0f) {
			goalVector = normalize(goalVector) * maxSpeed;
		}
		m_sim->setAgentPrefVelocity(i, goalVector);

		float angle = RandomHelper::random_real(0.0, 1.0) * 2.0 * 3.14159;
		float dist = RandomHelper::random_real(0.0, 1.0);
		Vector2 newV = (m_sim->getAgentPrefVelocity(i) + Vector2(cos(angle), sin(angle))) * 1.0;
		m_sim->setAgentPrefVelocity(i, newV);
	}

	unsigned int index = 0;
	if (bigGoalPos.x() != 0)
	{
		Vector2 goalVector = bigGoalPos - m_sim->getAgentPosition(index);
		if (absSq(goalVector) > 1.0f) {
			goalVector = normalize(goalVector) * maxSpeed;
		}
		m_sim->setAgentPrefVelocity(index, goalVector);
	}
	else
	{
		m_sim->setAgentPrefVelocity(index, Vector2(0, 0));
	}
}

void HelloWorld::updateSpPositon()
{
	for (int i = 0; i < m_spVec.size(); i++)
	{
		Vector2 v = m_sim->getAgentPosition(i+1);
		m_spVec[i]->setPosition(Vec2(v.x(), v.y()));
	}

	auto p= m_sim->getAgentPosition(0);
	m_bigSp->setPosition(p.x(), p.y());
}

int type = 0;
bool HelloWorld::onTouchBegan(cocos2d::Touch* touch, Event* unused_event)
{
	bigGoalPos= Vector2(0,0);
	if (m_goalSp->getBoundingBox().containsPoint(touch->getLocation()))
	{
		type = 1;
		return true;
	}
	if (m_bigSp->getBoundingBox().containsPoint(touch->getLocation()))
	{
		type = 2;
		return true;
	}
	return false;
}

void HelloWorld::onTouchMoved(cocos2d::Touch* touch, Event* unused_event)
{
	auto touchPos = touch->getLocation();
	if (type==1)
	{
		goalPos = Vector2(touchPos.x, touchPos.y);
		m_goalSp->setPosition(goalPos.x(), goalPos.y());
	}
}

void HelloWorld::onTouchEnded(cocos2d::Touch* touch, Event* unused_event)
{
	if (type == 2)
	{
		auto touchPos = touch->getLocation();
		bigGoalPos = Vector2(touchPos.x, touchPos.y);
	}
}
