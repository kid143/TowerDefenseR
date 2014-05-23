#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class Tower;
class Waypoint;
class Enemy;

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    virtual ~HelloWorld();
    void onTouchesBegan(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *unused_event);
    
private:
    int _wave;
    int _playerHp;
    int _playerGold;
    int _maxWave;
    double _currentWaveTime;
    bool _gameOver;
    
    cocos2d::Label *_ui_wave_lbl;
    cocos2d::Label *_ui_gold_lbl;
    cocos2d::Label *_ui_hp_lbl;
    cocos2d::Vector<cocos2d::Sprite *> *_towerBases;
    CC_SYNTHESIZE(cocos2d::Vector<Tower *>*, _towers, Towers);
    CC_SYNTHESIZE(cocos2d::Vector<Waypoint *>*, _waypoints, Waypoints);
    CC_SYNTHESIZE(cocos2d::Vector<Enemy *>*, _enemies, Enemies);
    
    void loadTowerPositions(void);
    void addWaypoints();
    
    bool circleCollideWithOtherCircle(const cocos2d::Point& circlePoint1,
                                      float circleRadius1,
                                      const cocos2d::Point& circlePoint2,
                                      float circleRadius2);
    void ccFillPoly(cocos2d::Point *poli, int points, bool closePolygon);
    void awardedForGold(int gold);
    void enemyGotKilled();
    void getHpDamage();
    void doGameOver();
    bool canBuyTower();
    
    bool loadWave();
    void update(float delta);
};

#endif // __HELLOWORLD_SCENE_H__
