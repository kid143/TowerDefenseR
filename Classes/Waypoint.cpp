//
//  Waypoint.cpp
//  TowerDefenseR
//
//  Created by kid143 on 14-5-21.
//
//

#include "Waypoint.h"

USING_NS_CC;

Waypoint::~Waypoint() {

}

Waypoint* Waypoint::nodeWithMainSceneAndLocation(HelloWorld* mainScene, const Point& location)
{
    Waypoint *instance = new Waypoint();
    if (instance && instance->initWithMainSceneAndLocation(mainScene, location)) {
        instance->autorelease();
        return instance;
    }
    CC_SAFE_DELETE(instance);
    return nullptr;
}

bool Waypoint::initWithMainSceneAndLocation(HelloWorld* mainScene, const Point& location)
{
    if (!Node::init()) {
        return false;
    }
    
    _mainScene = mainScene;
    this->setPosition(Point::ZERO);
    
    _nextWaypoint = nullptr;
    
    _myPosition = location;
    
    _mainScene->addChild(this, 2);
    return true;
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

// Waypoint debug draw
#ifdef COCOS2D_DEBUG
void Waypoint::draw(cocos2d::Renderer *renderer, const kmMat4 &transform, bool transformUpdated)
{
    CustomCommand *command = new CustomCommand();
    command->init(_globalZOrder);
    command->func = CC_CALLBACK_0(Waypoint::onDraw, this, renderer, transform, transformUpdated);
    
    renderer->addCommand(command);
    
    Node::draw(renderer, transform, transformUpdated);
}

void Waypoint::onDraw(cocos2d::Renderer *renderer, const kmMat4 &transform, bool transformUpdated)
{
    kmGLPushMatrix();
    kmGLLoadMatrix(&transform);
    
    ::glLineWidth(1);
    Color4B greenColor = Color4B(0, 255, 2, 255);
    
    DrawPrimitives::setDrawColor4B(greenColor.r, greenColor.g, greenColor.b, greenColor.a);

    DrawPrimitives::drawCircle(_myPosition, 6, 360, 30, false);
    DrawPrimitives::drawCircle(_myPosition, 2, 360, 30, false);
    
    if(_nextWaypoint)
        DrawPrimitives::drawLine(_myPosition, _nextWaypoint->getMyPosition());
}
#endif
//-----------------------------------------------------------------------------------
