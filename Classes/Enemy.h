//
//  Enemy.h
//  TowerDefenseR
//
//  Created by kid143 on 14-5-21.
//
//

#ifndef __TowerDefenseR__Enemy__
#define __TowerDefenseR__Enemy__

#include <iostream>
#include "cocos2d.h"
#include "HelloWorldScene.h"

class HelloWorld;
class Tower;
class Waypoint;

class Enemy : public cocos2d::Node {
private:
    cocos2d::Point _myPosition;
    int _maxHp;
    int _currentHp;
    float _walkingSpeed;
    Waypoint *_destinationWaypoint;
    bool _active;
    
public:
    ~Enemy();
    CC_SYNTHESIZE(HelloWorld*, _mainScene, MainScene);
    CC_SYNTHESIZE(cocos2d::Sprite*, _mySprite, MySprite);
    CC_SYNTHESIZE(cocos2d::Vector<Tower*>*, _attackedBy, AttackedBy);
    
    static Enemy* nodeWithMainScene(HelloWorld *mainScene);
    bool initWithMainScene(HelloWorld *mainScene);
    
    void update(float delta);
    
    void doActivate(float delta);
    void getRemoved();
    
    void getAttackedBy(Tower* attacker);
    void getLostInSight(Tower* attacker);
    void getDamaged(int damage);
    
    
    virtual void draw(cocos2d::Renderer *renderer, const kmMat4 &transform, bool transformUpdated);
    void onDraw(cocos2d::Renderer *renderer, const kmMat4 &transform, bool transformUpdated);
};

#endif /* defined(__TowerDefenseR__Enemy__) */
