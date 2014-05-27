//
//  Tower.h
//  TowerDefenseR
//
//  Created by kid143 on 14-5-21.
//
//

#ifndef __TowerDefenseR__Tower__
#define __TowerDefenseR__Tower__

#include <iostream>
#include "cocos2d.h"
#include "HelloWorldScene.h"

class HelloWorld;
class Enemy;

class Tower : public cocos2d::Node {
private:
    int fireRange;
    int damage;
    float fireRate;
    
    void attackEnemy();
    void shootWeapon(float delta);
    void damageEnemy();
    void removeBullet(cocos2d::Node *bullet);
    void lostSightOfEnemy();
    void chooseEnemyForAttack(Enemy* enemy);
public:
    Tower() {}
    virtual ~Tower();
    
    CC_SYNTHESIZE(HelloWorld*, _mainScene, MainScene);
    CC_SYNTHESIZE(cocos2d::Sprite*, _mySprite, MySprite);
    CC_SYNTHESIZE(bool, _attacking, Attacking);
    CC_SYNTHESIZE(Enemy*, _chosenEnemy, ChosenEnemy);
    
    virtual void update(float delta);
#ifdef COCOS2D_DEBUG
    virtual void draw(cocos2d::Renderer *renderer, const kmMat4 &transform, bool transformUpdated);
    void onDraw(cocos2d::Renderer *renderer, const kmMat4 &transform, bool transformUpdated);
#endif
    static Tower* nodeWithMainSceneAndLocation(HelloWorld* mainScene, const cocos2d::Point& point);
    bool initWithMainSceneAndLocation(HelloWorld* mainScene, const cocos2d::Point& point);
    
    void targetKilled();
};
#endif /* defined(__TowerDefenseR__Tower__) */
