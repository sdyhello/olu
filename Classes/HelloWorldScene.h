#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "GLES-Render.h"

#define PTM_RATIO 32

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    HelloWorld();
    virtual ~HelloWorld();
    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    void update(float dt);
    
    void createBackGround();
    
    void initPhysicsWorld();
    
    void setDebugDraw();
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags);
    virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    virtual void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void createPhysicsEdge();
    
    void createPencil();
    
    void createEraser();
    
    void focusPencil();
    
    void scanGameUI();
    
    void startUpdate();
    
    void createCup(float half_width, float height, int posX);
    
    void createMenuItem(cocos2d::Scene *scene);
    
    void moveLeft();
    
    void moveRight();
    
    void resetBody();
    
    void delayRun(float dt);
    
    void createSensorBody(float width, float height, int posX);
    
    void createArrow();
    
    void goodLuck();
private:
    b2World *m_world;
    GLESDebugDraw *m_debugDraw;
    b2Body *m_eraser;
    b2Body *m_pencilBody;
    b2Body *m_cupBody;
    b2Body *m_sensorBody;
    int borderScale;
    cocos2d::Size m_size;
    cocos2d::Scene * m_scene;
    cocos2d::Vec2 touchBeginPos;
    bool m_hasWon;
    cocos2d::Sprite * m_arrowSprite;
    cocos2d::Label * m_goodLuck;
};

#endif // __HELLOWORLD_SCENE_H__
