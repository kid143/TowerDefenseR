//
//  Bullets.cpp
//  TowerDefenseR
//
//  Created by kid143 on 14-5-27.
//
//

#include "Bullets.h"
#include "Enemy.h"

NormalBullet* NormalBullet::create(const std::string &filename)
{
    auto *instance = new NormalBullet();
    if (instance && instance->initWithFile(filename)) {
        instance->setTag(1001);
        instance->autorelease();
        return instance;
    }
    CC_SAFE_DELETE(instance);
    return nullptr;
}

void NormalBullet::damageEnemies(cocos2d::Vector<Enemy *> *enemies)
{
    Enemy *enemy = enemies->getRandomObject();
    
    enemy->getDamaged(_damage);
}

bool NormalBullet::hitOthers()
{
    // Normal bullet hit only one target.
    return false;
}

bool NormalBullet::canHitTarget(Enemy* enemy)
{
    return getBoundingBox().intersectsRect(enemy->getMySprite()->getBoundingBox());
}
