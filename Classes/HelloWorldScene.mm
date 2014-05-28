#include "HelloWorldScene.h"
#include "Tower.h"
#include "Waypoint.h"
#include "Enemy.h"
#include "Bullets.h"
#include "SimpleAudioEngine.h"
#include <sys/timeb.h>

#define kTOWER_COST 300

USING_NS_CC;


//------------------------------------------------------------------------
// Init & Destroy
//------------------------------------------------------------------------
Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

HelloWorld::~HelloWorld()
{
    _towerBases->clear();
    delete _towerBases;
    _towers->clear();
    delete _towers;
    _waypoints->clear();
    delete _waypoints;
    _enemies->clear();
    delete _enemies;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    cocos2d::Size visibleSize = Director::getInstance()->getVisibleSize();
   
    Sprite *background = Sprite::create("bg.png");
    background->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    addChild(background, -1);
    
    _towerBases = new Vector<Sprite *>();
    _towers = new Vector<Tower *>();
    _waypoints = new Vector<Waypoint *>();
    _enemies = new Vector<Enemy *>();
    
    loadTowerPositions();
    addWaypoints();
    loadWave();
    
    timeb now;
    ftime(&now);
    _currentWaveTime = now.time + now.millitm / 1000;
    
    addChild(_ui_wave_lbl, 10);
    _ui_wave_lbl->setPosition(400, visibleSize.height - 12);
    _ui_wave_lbl->setAnchorPoint(cocos2d::Point(0, 0.5));
    
    _playerHp = 5;
    char hp_text[10] = {0};
    sprintf(hp_text, "HP: %d", _playerHp);
    _ui_hp_lbl = Label::createWithBMFont("font_red_14.fnt", hp_text);
    addChild(_ui_hp_lbl, 10);
    _ui_hp_lbl->setPosition(35, visibleSize.height - 12);
    _ui_hp_lbl->setAnchorPoint(cocos2d::Point(0, 0.5));
    
    _playerGold = 1000;
    char gold_text[10] = {0};
    sprintf(gold_text, "GOLD: %d", _playerGold);
    _ui_gold_lbl = Label::createWithBMFont("font_red_14.fnt", gold_text);
    addChild(_ui_gold_lbl, 10);
    _ui_gold_lbl->setPosition(135, visibleSize.height - 12);
    _ui_gold_lbl->setAnchorPoint(cocos2d::Point(0,0.5));
    
    // cocos2d-x 3.0 now use a brand new event dispatching technique for handling
    // all the events including the user interaction events.
    // The old setUserInteraction method is now deprecated.
    auto eventListener = EventListenerTouchAllAtOnce::create();
    eventListener->onTouchesBegan = CC_CALLBACK_2(HelloWorld::onTouchesBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
    
    // collision detect event.
    auto collisionEventListener = EventListenerCustom::create("bullet_hits_enemy", CC_CALLBACK_1(HelloWorld::bulletHitCallback, this));
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(collisionEventListener, this);
    
    // Audio play
    CocosDenshion::SimpleAudioEngine *audioPlayer = CocosDenshion::SimpleAudioEngine::getInstance();
    audioPlayer->playBackgroundMusic("8bitDungeonLevel.mp3", true);
    audioPlayer->preloadEffect("tower_place.wav");
    audioPlayer->preloadEffect("life_lose.wav");
    audioPlayer->preloadEffect("enemy_destroy.wav");
    audioPlayer->preloadEffect("enemy_destroy_2.wav");
    audioPlayer->preloadEffect("laser_shoot.wav");
    
    scheduleUpdate();
    
    return true;
}

//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Game Logics
//------------------------------------------------------------------------

void HelloWorld::update(float delta)
{
    timeb now;
    ftime(&now);
    double currentTime = now.time + now.millitm / 1000;
    if (currentTime - _currentWaveTime >= 15.0f) {
        loadWave();
        _currentWaveTime = currentTime;
    }
    
    bulletHitDetection();
    wipeHittedBullet();
    
    if (_wave >= _maxWave && _enemies->empty()) {
        doGameOver();
    }
}

// This is for iOS only, actually I shouldn't deal with plists like this.
// When working on a product project, I should provide a plist reader in C++ and
// use that reader instead of Apple's native api.

void HelloWorld::loadTowerPositions()
{
    NSString* plistPath = [[NSBundle mainBundle] pathForResource:@"TowersPosition" ofType:@"plist"];
    NSArray * towerPositions = [NSArray arrayWithContentsOfFile:plistPath];
    for(NSDictionary * towerPos in towerPositions)
    {
        Sprite * towerBase = Sprite::create("open_spot.png");
        addChild(towerBase, 1);
        float towerX = [[towerPos objectForKey:@"x"] floatValue];
        float towerY = [[towerPos objectForKey:@"y"] floatValue];
        towerBase->setPosition(towerX, towerY);
        _towerBases->pushBack(towerBase);
    }
}

/**
 * For real game, the waypoint data is stored in files and loaded in each level,
 * for this demo, the waypoints are hard-coded.
 */
void HelloWorld::addWaypoints()
{
    Waypoint * waypoint1 = Waypoint::nodeWithMainSceneAndLocation(this, cocos2d::Point(420, 35));
    _waypoints->pushBack(waypoint1);
    
    Waypoint * waypoint2 = Waypoint::nodeWithMainSceneAndLocation(this, cocos2d::Point(35, 35));
    _waypoints->pushBack(waypoint2);
    waypoint2->setNextWaypoint(waypoint1);
    
    Waypoint * waypoint3 = Waypoint::nodeWithMainSceneAndLocation(this, cocos2d::Point(35, 130));
    _waypoints->pushBack(waypoint3);
    waypoint3->setNextWaypoint(waypoint2);
    
    Waypoint * waypoint4 = Waypoint::nodeWithMainSceneAndLocation(this, cocos2d::Point(445, 130));
    _waypoints->pushBack(waypoint4);
    waypoint4->setNextWaypoint(waypoint3);
    
    Waypoint * waypoint5 = Waypoint::nodeWithMainSceneAndLocation(this, cocos2d::Point(445, 220));
    _waypoints->pushBack(waypoint5);
    waypoint5->setNextWaypoint(waypoint4);
    
    Waypoint * waypoint6 = Waypoint::nodeWithMainSceneAndLocation(this, cocos2d::Point(-40, 220));
    _waypoints->pushBack(waypoint6);
    waypoint6->setNextWaypoint(waypoint5);
}

// Same as HelloWorld::loadTowerPositions, this is iOS only code.
bool HelloWorld::loadWave()
{
    NSString* plistPath = [[NSBundle mainBundle] pathForResource:@"Waves" ofType:@"plist"];
    NSArray * waveData = [NSArray arrayWithContentsOfFile:plistPath];
    
    _maxWave = [waveData count];
    if(_wave >= _maxWave)
    {
        return false;
    }
    
    NSArray * currentWaveData =[NSArray arrayWithArray:[waveData objectAtIndex:_wave]];
    
    for(NSDictionary * enemyData in currentWaveData)
    {
        Enemy *enemy = Enemy::nodeWithMainScene(this);
        _enemies->pushBack(enemy);
        float spawnTime = [[enemyData objectForKey:@"spawnTime"] floatValue];
        enemy->schedule(schedule_selector(Enemy::doActivate),
                        spawnTime);
    }
    
    _wave++;
    char wave_text[10] = {0};
    sprintf(wave_text, "WAVE: %d", _wave);
    if (!_ui_wave_lbl) {
        _ui_wave_lbl = Label::createWithBMFont("font_red_14.fnt", "");
    }
    _ui_wave_lbl->setString(wave_text);
    
    return true;
}

bool HelloWorld::circleCollideWithOtherCircle(const cocos2d::Point &circlePoint1,
                                              float circleRadius1,
                                              const cocos2d::Point &circlePoint2,
                                              float circleRadius2)
{
    float distance = (circlePoint2 - circlePoint1).getLength();
    if (distance < circleRadius1 + circleRadius2)
        return true;
    return false;
}

void HelloWorld::awardedForGold(int gold)
{
    _playerGold +=gold;
    
    char gold_text[10] = {0};
    sprintf(gold_text, "Gold: %d", _playerGold);
    _ui_gold_lbl->setString(gold_text);
}

bool HelloWorld::canBuyTower()
{
    if (_playerGold - kTOWER_COST >= 0) {
        return true;
    }
    return false;
}

void HelloWorld::bulletHitDetection()
{
    // bullet hit detection
    enumerateChildrenByTagWithFunction(1001, [=](cocos2d::Node* child, bool *stop) {
        Bullet *bullet = (Bullet *)child;
        Vector<cocos2d::Node*> *targets = new Vector<cocos2d::Node*>();
        this->enumerateChildrenByTagWithFunction(2001, [=](cocos2d::Node* child, bool *stop2) {
            if (bullet->canHitTarget((Enemy *)child)) {
                targets->pushBack(child);
                *stop2 = !bullet->hitOthers();
            }
        });
        
        if (targets->size() > 0) {
            // trigger bullet hit event.
            std::map<std::string, Vector<cocos2d::Node *>*> *info = new std::map<std::string, Vector<cocos2d::Node *>*>();
            Vector<cocos2d::Node*> *sender = new Vector<cocos2d::Node*>();
            sender->pushBack(bullet);
            info->insert(std::make_pair("sender", sender));
            info->insert(std::make_pair("receiver", targets));
            
            EventCustom bulletHitEvent("bullet_hits_enemy");
            bulletHitEvent.setUserData(info);
            _eventDispatcher->dispatchEvent(&bulletHitEvent);
        }
    });
}

void HelloWorld::wipeHittedBullet()
{
    for (auto child : this->getChildren()) {
        if (child->getTag() == 1001) {
            // get rid of non hit bullet
            if (child->getReferenceCount() > 0 && child->getNumberOfRunningActions() == 0) {
                child->removeFromParent();
                child->release();
            }
        }
    }
}

void HelloWorld::bulletHitCallback(cocos2d::EventCustom *event)
{
    std::map<std::string, cocos2d::Node*> *info = (std::map<std::string, cocos2d::Node*> *)event->getUserData();
    Vector<cocos2d::Node*> *sender = (Vector<cocos2d::Node*> *)info->find("sender")->second;
    Bullet *bullet = (Bullet *)sender->getRandomObject();
    Vector<cocos2d::Node*> *targets = (Vector<cocos2d::Node*> *)info->find("receiver")->second;
    Vector<Enemy*> *targetEnemies = (Vector<Enemy*>*)targets;
    
    bullet->damageEnemies(targetEnemies);
    bullet->removeFromParent();
    
    bullet->release();
    sender->clear();
    targets->clear();
    delete sender;
    delete targets;
    info->clear();
    delete info;
}

void HelloWorld::ccFillPoly(cocos2d::Point *poli, int points, bool closePolygon)
{

}

void HelloWorld::enemyGotKilled()
{

}

void HelloWorld::getHpDamage()
{
    _playerHp--;
    char hp[10] = {0};
    sprintf(hp, "HP: %d", _playerHp);
    _ui_hp_lbl->setString(hp);
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("life_lose.wav");
    
    if (_playerHp <= 0) {
        doGameOver();
    }
}

void HelloWorld::doGameOver()
{
    if (!_gameOver) {
        _gameOver = true;
        
        Director::getInstance()->replaceScene(TransitionRotoZoom::create(1, HelloWorld::createScene()));
    }
}

//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Callbacks For User Interactions
//------------------------------------------------------------------------

void HelloWorld::onTouchesBegan(const std::vector<Touch *> &touches, cocos2d::Event *unused_event)
{
    for (Touch *touch : touches) {
        cocos2d::Point loc = touch->getLocation();
        
        for (Sprite *tb : *_towerBases) {
            if (canBuyTower() && tb->getBoundingBox().containsPoint(loc) && !tb->getUserData()) {
                Tower *tower = Tower::nodeWithMainSceneAndLocation(this, tb->getPosition());
                _towers->pushBack(tower);
                tb->setUserData(tower);
                
                _playerGold -= kTOWER_COST;
                char gold_text[10] = {0};
                sprintf(gold_text, "Gold: %d", _playerGold);
                _ui_gold_lbl->setString(gold_text);
                
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("tower_place.wav");
                break;
            }
        }
    }
}

//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Helper methods
//------------------------------------------------------------------------
void HelloWorld::enumerateChildrenByTagWithFunction(int tag,
                                                    const std::function<void (cocos2d::Node *, bool *)>& callback)
{
    bool shouldStop = false;
    bool *stop = &shouldStop;
    Vector<cocos2d::Node*> children = getChildren();
    auto it = children.begin();
    while (!*stop && it != children.end()) {
        if ((*it)->getTag() == tag) {
            callback(*it, stop);
        }
        it++;
    }
}
//------------------------------------------------------------------------