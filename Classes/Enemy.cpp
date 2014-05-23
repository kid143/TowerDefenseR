//
//  Enemy.cpp
//  TowerDefenseR
//
//  Created by kid143 on 14-5-21.
//
//

#include "Enemy.h"
#include "Waypoint.h"
#include "Tower.h"
#include "SimpleAudioEngine.h"
#include <math.h>

#define HEALTH_BAR_WIDTH 20
#define HEALTH_BAR_ORIGIN -10

USING_NS_CC;

Enemy* Enemy::nodeWithMainScene(HelloWorld *mainScene)
{
    Enemy *instance = new Enemy();
    if (instance && instance->initWithMainScene(mainScene)) {
        instance->autorelease();
        return instance;
    }
    CC_SAFE_DELETE(instance);
    return nullptr;
}

Enemy::~Enemy()
{
    _attackedBy->clear();
    delete _attackedBy;
}

bool Enemy::initWithMainScene(HelloWorld *mainScene)
{
    if (!Node::init()) {
        return false;
    }
    
    _mainScene = mainScene;
    _maxHp = 40;
    _currentHp = _maxHp;
    
    _active = false;
    
    _walkingSpeed = 0.5;
    
    _mySprite = Sprite::create("enemy.png");
    addChild(_mySprite);
    
    Vector<Waypoint*> *waypoints = _mainScene->getWaypoints();
    Waypoint * waypoint = waypoints->at(waypoints->size() - 1);
    
    _destinationWaypoint = waypoint->getNextWaypoint();
    
    Point pos = waypoint->getMyPosition();
    _myPosition = pos;
    
    _mySprite->setPosition(pos);
    
    _mainScene->addChild(this, 2);
    
    scheduleUpdate();
    
    _attackedBy = new Vector<Tower*>(5);
    
    return true;
}

void Enemy::update(float delta)
{
    if (!_active) return;
    
    if (_mainScene->circleCollideWithOtherCircle(_myPosition, 1, _destinationWaypoint->getMyPosition(), 1)) {
        if (_destinationWaypoint->getNextWaypoint()) {
            _destinationWaypoint = _destinationWaypoint->getNextWaypoint();
        } else {
            _mainScene->getHpDamage();
            getRemoved();
            return;
        }
    }
    
    Point targetPoint = _destinationWaypoint->getMyPosition();
    float moveSpeed = _walkingSpeed;
    Point direction = (targetPoint - _myPosition).normalize();
    _mySprite->setRotation(CC_RADIANS_TO_DEGREES(atan2(direction.y, -direction.x)));
    _myPosition = direction * moveSpeed + _myPosition;
    
    _mySprite->setPosition(_myPosition);
}

void Enemy::doActivate(float delta)
{
    _active = true;
}

void Enemy::getRemoved()
{
    int randomInt = arc4random() % 2;
    if (randomInt == 0) {
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("enemy_destroy.wav");
    } else {
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("enemy_destroy_2.wav");
    }
    for (Tower* attacker : *_attackedBy) {
        attacker->targetKilled();
    }
    this->getParent()->removeChild(this);
    _mainScene->getEnemies()->eraseObject(this);
    
    // Notify the main scene that the enemy is killed. A better design is to employ a event dispatcher
    // to invoke the callbacks for larger project. It should be a simple work in cocos2d-x v3.0,
    // since the framework is already armed with a built in event dispatcher and the customization of
    // events is so easy.
    //
    // Check out this link for more info: http://www.cocos2d-x.org/wiki/EventDispatcher_Mechanism
    _mainScene->enemyGotKilled();
}

void Enemy::getAttackedBy(Tower* attacker)
{
    _attackedBy->pushBack(attacker);
}

void Enemy::getLostInSight(Tower *attacker)
{
    _attackedBy->eraseObject(attacker);
}

void Enemy::getDamaged(int damage)
{
    _currentHp -= damage;
    if (_currentHp < 0) {
        getRemoved();
        _mainScene->awardedForGold(200);
    }
}

//-----------------------------------------------------------------------------------
/*
 * The custom drawing code in cocos2d-x 3.0 is quite different from 2.0 or cocos2d-iphone.
 * Because the api is redesigned and is prepared for multithread task. The code below is
 * the new code for customizing the drawing.
 *
 * More reference: http://stackoverflow.com/questions/22549233/cocos2d-x-3-0-draw-vs-ondraw
 *
 */

void Enemy::draw(cocos2d::Renderer *renderer, const kmMat4 &transform, bool transformUpdated)
{
    CustomCommand *command = new CustomCommand();
    command->init(_globalZOrder);
    command->func = CC_CALLBACK_0(Enemy::onDraw, this, renderer, transform, transformUpdated);
    
    renderer->addCommand(command);
    
    Node::draw(renderer, transform, transformUpdated);
}

void Enemy::onDraw(cocos2d::Renderer *renderer, const kmMat4 &transform, bool transformUpdated)
{
    kmGLPushMatrix();
    kmGLLoadMatrix(&transform);
    
    // Draw the HP gauge.
    DrawPrimitives::drawSolidRect(_myPosition + Point(HEALTH_BAR_ORIGIN, 16),
                    _myPosition + Point(HEALTH_BAR_ORIGIN + HEALTH_BAR_WIDTH, 14),
                    Color4F(1.0, 0, 0, 1.0));
    
    DrawPrimitives::drawSolidRect(_myPosition + Point(HEALTH_BAR_ORIGIN, 16),
                    _myPosition + Point(HEALTH_BAR_ORIGIN + (float)(_currentHp * HEALTH_BAR_WIDTH) / _maxHp, 14),
                    Color4F(0, 1.0, 0, 1.0));
}
