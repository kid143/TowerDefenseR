//
//  Tower.cpp
//  TowerDefenseR
//
//  Created by kid143 on 14-5-21.
//
//

#include "Tower.h"
#include "Enemy.h"
#include "SimpleAudioEngine.h"
#include <math.h>

USING_NS_CC;

Tower::~Tower()
{

}

Tower* Tower::nodeWithMainSceneAndLocation(HelloWorld *mainScene, const Point &point)
{
    Tower *newInstance = new Tower();
    if (newInstance && newInstance->initWithMainSceneAndLocation(mainScene, point)) {
        newInstance->autorelease();
        return newInstance;
    }
    CC_SAFE_DELETE(newInstance);
    return nullptr;
}

bool Tower::initWithMainSceneAndLocation(HelloWorld *mainScene, const Point &point)
{
    if (!Node::init()) {
        return false;
    }
    _mainScene = mainScene;
    fireRange = 70;
    damage = 10;
    fireRate = 1.0f;
        
    _mySprite = Sprite::create("tower.png");
    addChild(_mySprite);
    _mySprite->setPosition(point);
        
    _mainScene->addChild(this, 2);
        
    scheduleUpdate();
    
    _chosenEnemy = nullptr;
    
    return true;
}

void Tower::update(float delta)
{
    // Here we decide the tower to attack or to find enemy
    if (_chosenEnemy) {
        // Turret rotation
        Point direction = (_chosenEnemy->getMySprite()->getPosition() - _mySprite->getPosition()).normalize();
        _mySprite->setRotation(CC_RADIANS_TO_DEGREES(atan2(direction.y, -direction.x)) + 90);
        
        if (!_mainScene->circleCollideWithOtherCircle(_mySprite->getPosition(),
                                                      fireRange,
                                                      _chosenEnemy->getMySprite()->getPosition(),
                                                      1)) {
            lostSightOfEnemy();
        }
    } else {
        for (Enemy *enemy : *_mainScene->getEnemies()) {
            if (_mainScene->circleCollideWithOtherCircle(enemy->getMySprite()->getPosition(),
                                                         1,
                                                         _mySprite->getPosition(),
                                                         fireRange)) {
                chooseEnemyForAttack(enemy);
                break;
            }
        }
    }
}

void Tower::chooseEnemyForAttack(Enemy *enemy)
{
    _chosenEnemy = nullptr;
    _chosenEnemy = enemy;
    attackEnemy();
    _chosenEnemy->getAttackedBy(this);
}

void Tower::attackEnemy()
{
    schedule(schedule_selector(Tower::shootWeapon), fireRate);
}

void Tower::shootWeapon(float delta)
{
    Sprite *bullet = Sprite::create("bullet.png");
    bullet->setPosition(_mySprite->getPosition());
    _mainScene->addChild(bullet, 2);
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("laser_shoot.wav");
    /**
     * This is not a good way to deal with damages to enemies, but a simpler way.
     * The more extensible and reasonable way is to check collisions between enemies 
     * and the bullets using events to build communication between the two.
     */
    bullet->runAction(Sequence::create(MoveTo::create(0.1, _chosenEnemy->getMySprite()->getPosition()),
                                       CallFunc::create(CC_CALLBACK_0(Tower::damageEnemy, this)),
                                       CallFuncN::create(CC_CALLBACK_1(Tower::removeBullet, this)),NULL));
}

void Tower::damageEnemy()
{
    // There might be some bug in cocos2d-x's unschedule functions,
    // when the enemy is attacked by multiple towers and killed,
    // this method should not be invoked, but I keep getting null pointer error.
    // This is weird.
    // So we check the _chosenEnemy for safety, or the program will crash.
    // If you are working on cocos2d-iphone, this is not neccesary
    // since Objective C is safe with sending messages to nil object.
    if (_chosenEnemy)
        _chosenEnemy->getDamaged(damage);
}

void Tower::removeBullet(Node *bullet)
{
    bullet->getParent()->removeChild(bullet);
}

void Tower::targetKilled()
{
    if (_chosenEnemy) {
        _chosenEnemy = nullptr;
    }
    unschedule(schedule_selector(Tower::shootWeapon));
}

void Tower::lostSightOfEnemy()
{
    _chosenEnemy->getLostInSight(this);
    if (_chosenEnemy) {
        _chosenEnemy = nullptr;
    }
    unschedule(schedule_selector(Tower::shootWeapon));
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

void Tower::onDraw(Renderer *renderer, const kmMat4 &transform, bool transformUpdated)
{
    kmGLPushMatrix();
    kmGLLoadMatrix(&transform);
    
    ::glLineWidth(1);
    Color4B blackColor = Color4B(255, 255, 255, 255);
    
    DrawPrimitives::setDrawColor4B(blackColor.r, blackColor.g, blackColor.b, blackColor.a);
    DrawPrimitives::drawCircle(_mySprite->getPosition(), fireRange, 360, 30, false);
}

void Tower::draw(Renderer *renderer, const kmMat4 &transform, bool transformUpdated)
{
    CustomCommand *command = new CustomCommand();
    command->init(_globalZOrder);
    command->func = CC_CALLBACK_0(Tower::onDraw, this, renderer, transform, transformUpdated);
    
    renderer->addCommand(command);
    
    Node::draw(renderer, transform, transformUpdated);
}
//-----------------------------------------------------------------------------------