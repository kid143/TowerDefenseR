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

void NormalBullet::damageEnemy(Enemy *enemy)
{
    enemy->getDamaged(_damage);
}
