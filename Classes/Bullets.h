//
//  Bullets.h
//  TowerDefenseR
//
//  Created by kid143 on 14-5-27.
//
//

#ifndef __TowerDefenseR__Bullets__
#define __TowerDefenseR__Bullets__

#include <iostream>
#include "cocos2d.h"

class Enemy;

class Bullet : public cocos2d::Sprite {

public:
    Bullet() {}
    ~Bullet() {}
    
    virtual void damageEnemies(cocos2d::Vector<Enemy*>* enemies)=0;
    virtual bool hitOthers()=0;
    virtual bool canHitTarget(Enemy* enemy)=0;
};

class NormalBullet : public Bullet {
    
private:
    int _damage;
    
public:
    NormalBullet() { _damage = 10; }
    
    static NormalBullet* create(const std::string& filename);
    
    virtual void damageEnemies(cocos2d::Vector<Enemy*>* enemies);
    virtual bool hitOthers();
    virtual bool canHitTarget(Enemy* enemy);
};

#endif /* defined(__TowerDefenseR__Bullets__) */
