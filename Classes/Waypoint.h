//
//  Waypoint.h
//  TowerDefenseR
//
//  Created by kid143 on 14-5-21.
//
//

#ifndef __TowerDefenseR__Waypoint__
#define __TowerDefenseR__Waypoint__

#include <iostream>
#include "cocos2d.h"
#include "HelloWorldScene.h"

class Waypoint : public cocos2d::Node {
private:
    HelloWorld *_mainScene;
    cocos2d::Point _myPosition;
    
public:
    Waypoint() : _mainScene(nullptr), _nextWaypoint(nullptr) {}
    virtual ~Waypoint();
    
    CC_SYNTHESIZE(Waypoint*, _nextWaypoint, NextWaypoint);
    // The original CC_SYNTHESIZE macro does not work right for _myPosition property, a non-heap object.
    void setMyPosition(const cocos2d::Point& location) { _myPosition = location; }
    cocos2d::Point getMyPosition(void) { return _myPosition; }
    
    static Waypoint* nodeWithMainSceneAndLocation(HelloWorld* mainScene,
                                                  const cocos2d::Point& location);
    bool initWithMainSceneAndLocation(HelloWorld* mainScene, const cocos2d::Point& location);
#ifdef COCOS2D_DEBUG
    virtual void draw(cocos2d::Renderer *renderer, const kmMat4 &transform, bool transformUpdated);
    void onDraw(cocos2d::Renderer *renderer, const kmMat4 &transform, bool transformUpdated);
#endif
};

#endif /* defined(__TowerDefenseR__Waypoint__) */
