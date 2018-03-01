#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

HelloWorld::HelloWorld(){
    borderScale = 2;
    m_size = Director::getInstance()->getVisibleSize();
    m_hasWon = false;
    m_goodLuck = nullptr;
}

Scene* HelloWorld::createScene()
{
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
}

void HelloWorld::createBackGround()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto colorLayer_1 = LayerGradient::create(Color4B(0x7E, 0xC0, 0xEE, 255), Color4B(0x7C, 0xCD, 0x7C, 255));
    colorLayer_1->setContentSize(visibleSize);
    colorLayer_1->setPosition(Vec2(0,0));
    this->addChild(colorLayer_1, -1);
    
    auto colorLayer_2 = LayerGradient::create(Color4B(0xB0, 0xC4, 0xDE, 255), Color4B(0x9B, 0xCD, 0x9B, 255));
    colorLayer_2->setContentSize(visibleSize);
    colorLayer_2->setPosition(Vec2(visibleSize.width, 0));
    colorLayer_2->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    this->addChild(colorLayer_2, -1);
}

void HelloWorld::startUpdate()
{
    scheduleUpdate();
}

void HelloWorld::scanGameUI()
{
    auto size = Director::getInstance()->getVisibleSize();
    int length = borderScale * size.width - size.width/4*3;
    auto moveBy_1 =MoveBy::create(3, Vec2(-length, 0));
    auto moveBy_2 = moveBy_1->reverse();
    auto callFunc = CallFunc::create(CC_CALLBACK_0(HelloWorld::startUpdate, this));
    auto seq = Sequence::create(moveBy_1, moveBy_2, callFunc, NULL);
    this->runAction(seq);
}

void HelloWorld::delayRun(float dt)
{
    auto scene = Director::getInstance()->getRunningScene();
    m_scene = scene;
    this->createMenuItem(scene);
    this->createArrow();
}
bool HelloWorld::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    scheduleOnce(CC_SCHEDULE_SELECTOR(HelloWorld::delayRun), 0);
    
    //this->createBackGround();
    this->initPhysicsWorld();
    
    setTouchEnabled(true);
    setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
    
    this->scanGameUI();
    //this->setPosition(Vec2(-(borderScale * m_size.width - m_size.width/4*3), 0));
    return true;
}

void HelloWorld::focusPencil()
{
    auto size = Director::getInstance()->getVisibleSize();
    b2Vec2 pencilPos = m_pencilBody->GetPosition();
    Vec2 uiPos = Vec2(pencilPos.x * PTM_RATIO, pencilPos.y * PTM_RATIO);
//    log("pencilPos:%f, %f", uiPos.x, uiPos.y);
    float disPosX = uiPos.x - size.width/2;
    float disPosY = uiPos.y - size.height/4;
    
    this->setPosition(Vec2(-disPosX, -disPosY));
}

void HelloWorld::update(float dt)
{
    float timeStep = 0.03f;
    int32 velocityIterations = 8;
    int32 positionIterations = 1;

    m_world->Step(timeStep, velocityIterations, positionIterations);
    
    for (b2Body * b = m_world->GetBodyList(); b; b = b->GetNext())
    {
        if(b->GetUserData() != nullptr){
            Sprite *sprite = (Sprite *)b->GetUserData();
            sprite->setPosition(Vec2(b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO));
            sprite->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
        }
    }
    this->focusPencil();
    
    auto erasePos = m_eraser->GetTransform().p;
    m_arrowSprite->setPosition(Vec2(erasePos.x * PTM_RATIO, erasePos.y * PTM_RATIO));
    //check has won
    if(!m_hasWon){
        b2ContactEdge * edge = m_pencilBody->GetContactList();
        while(edge){
            b2Contact * contact = edge->contact;
            b2Fixture * fixtureA = contact->GetFixtureA();
            b2Fixture * fixtureB = contact->GetFixtureB();
            
            b2Body * bodyA = fixtureA->GetBody();
            b2Body * bodyB = fixtureB->GetBody();
            
            if (bodyA == m_sensorBody || bodyB == m_sensorBody){
                m_hasWon = true;
                this->goodLuck();
                log("good luck , you won!");
                break;
            }
            edge = edge->next;
        }
    }
}

bool HelloWorld::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    touchBeginPos = touch->getLocation();
    return true;
}

void HelloWorld::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    auto curPos = touch->getLocation();
    auto diff = touchBeginPos - curPos;
    auto normal = diff.getNormalized();
    auto radians = normal.getAngle();
    auto degrees = CC_RADIANS_TO_DEGREES(radians);
    m_arrowSprite->setRotation(-degrees);
}

void HelloWorld::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    auto endPos = touch->getLocation();
    auto diff = touchBeginPos - endPos;
    auto normal = diff.getNormalized();
    auto length = diff.getLength();
    m_eraser->SetLinearVelocity(b2Vec2(normal.x * length, normal.y * length));
}


void HelloWorld::initPhysicsWorld()
{
    b2Vec2 gravity;
    gravity.Set(0.0f, -10.0f);
    m_world = new b2World(gravity);
    m_world->SetContinuousPhysics(true);
    m_world->SetAllowSleeping(true);
    
    this->setDebugDraw();
    this->createPhysicsEdge();
    this->createPencil();
    this->createEraser();
    this->createCup();
    
}

void HelloWorld::setDebugDraw()
{
    m_debugDraw = new GLESDebugDraw(PTM_RATIO);
    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
    flags += b2Draw::e_jointBit;
    flags += b2Draw::e_centerOfMassBit;
    m_debugDraw->SetFlags(flags);
    
    m_world->SetDebugDraw(m_debugDraw);
}

void HelloWorld::createPhysicsEdge()
{
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0, 0);
    b2Body * groundBody = m_world->CreateBody(&groundBodyDef);
    
    auto size = Director::getInstance()->getVisibleSize();
    b2EdgeShape groundBox;
    //bottom
    groundBox.Set(b2Vec2(0, 10/PTM_RATIO), b2Vec2(size.width * borderScale/PTM_RATIO, 10/PTM_RATIO));
    groundBody->CreateFixture(&groundBox, 0);
    //left
    groundBox.Set(b2Vec2(0, 10/PTM_RATIO), b2Vec2(0, size.height * borderScale/PTM_RATIO));
    groundBody->CreateFixture(&groundBox, 0);
    //right
    groundBox.Set(b2Vec2(size.width * borderScale/PTM_RATIO, 10/PTM_RATIO), b2Vec2(size.width * borderScale/PTM_RATIO, size.height * borderScale/PTM_RATIO));
    groundBody->CreateFixture(&groundBox, 0);
    //top
    groundBox.Set(b2Vec2(0, size.height * borderScale/PTM_RATIO), b2Vec2(size.width * borderScale/PTM_RATIO, size.height * borderScale/PTM_RATIO));
    groundBody->CreateFixture(&groundBox, 0);
    
    auto bookSprite = Sprite::create("book.png");
    auto spriteSize = bookSprite->getContentSize();
    bookSprite->setPosition(Vec2(size.width/2, 48));
    this->addChild(bookSprite);
    
    b2PolygonShape polygonShape;
    polygonShape.SetAsBox(spriteSize.width / 2 / PTM_RATIO, spriteSize.height/ 2 / PTM_RATIO, b2Vec2(size.width/2/PTM_RATIO, 1.5f), 0);
    groundBody->CreateFixture(&polygonShape, 0);
}

void HelloWorld::createPencil()
{
    auto pencilSprite = Sprite::create("pencil.png");
    auto spriteSize = pencilSprite->getContentSize();
    this->addChild(pencilSprite);
    
    auto size = Director::getInstance()->getVisibleSize();
    
    b2BodyDef pencilDef;
    pencilDef.type = b2_dynamicBody;
    pencilDef.position.Set(size.width/2/PTM_RATIO, 4.0f);
    m_pencilBody = m_world->CreateBody(&pencilDef);
    m_pencilBody->SetUserData(pencilSprite);
    
    b2PolygonShape pencilShape;
    pencilShape.SetAsBox(spriteSize.width / PTM_RATIO / 2, spriteSize.height / PTM_RATIO / 2);
    
    b2FixtureDef pencilFixtureDef;
    pencilFixtureDef.shape = &pencilShape;
    pencilFixtureDef.density = 1.0f;
    pencilFixtureDef.friction = 1.0f;
    m_pencilBody->CreateFixture(&pencilFixtureDef);
}

void HelloWorld::createEraser()
{
    auto eraseSprite = Sprite::create("eraser.png");
    auto spriteSize = eraseSprite->getContentSize();
    this->addChild(eraseSprite);
    
    auto size = Director::getInstance()->getVisibleSize();
    b2BodyDef eraserDef;
    eraserDef.type = b2_dynamicBody;
    eraserDef.bullet = true;
    eraserDef.position.Set((size.width/2 - 50)/PTM_RATIO, 60/PTM_RATIO);
    m_eraser = m_world->CreateBody(&eraserDef);
    m_eraser->SetUserData(eraseSprite);
    
    b2PolygonShape eraserShape;
    eraserShape.SetAsBox(spriteSize.width / 2 / PTM_RATIO, spriteSize.height / 2 / PTM_RATIO);
    
    b2FixtureDef eraserFixtureDef;
    eraserFixtureDef.shape = &eraserShape;
    eraserFixtureDef.density = 10.0f;
    eraserFixtureDef.friction = 0.3f;
    m_eraser->CreateFixture(&eraserFixtureDef);
}

void HelloWorld::createArrow()
{
    m_arrowSprite = Sprite::create("arrow.png");
    this->addChild(m_arrowSprite);
    m_arrowSprite->setScaleX(0.05);
    m_arrowSprite->setScaleY(0.03);
    m_arrowSprite->setAnchorPoint(Vec2(0, 0.5));
    m_arrowSprite->setPosition(Vec2(m_size.width/2 - 50, 30));
}

void HelloWorld::createCup()
{
    int posX = 500;
    
    auto cupSprite = Sprite::create("cup.png");
    this->addChild(cupSprite);
    cupSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    cupSprite->setPosition((borderScale * m_size.width - posX), 0.1 * PTM_RATIO);
    
    b2BodyDef cupDef;
    //left
    cupDef.position.Set((borderScale * m_size.width - posX) / PTM_RATIO, 0.1);
    m_cupBody = m_world->CreateBody(&cupDef);
    
    float width_1 = 6.0f;
    float width_2 = 4.0f;
    float height_1 = 8.0f;
    float height_2 = 1.0f;
    
    b2Vec2 vs[9];
    vs[0].Set(0.0f, 0.0f);
    vs[1].Set(-width_1 / 2, 0.0f);
    vs[2].Set(-width_1 / 2, height_1);
    vs[3].Set(-width_2 / 2, height_1);
    vs[4].Set(-width_2 / 2, height_2);
    vs[5].Set(width_2 / 2, height_2);
    vs[6].Set(width_2 / 2, height_1);
    vs[7].Set(width_1 / 2, height_1);
    vs[8].Set(width_1 / 2, 0.0f);
    
    b2ChainShape shape;
    shape.CreateLoop(vs, 9);
    m_cupBody->CreateFixture(&shape, 0.0f);
    
    this->createSensorBody();
}

void HelloWorld::createSensorBody()
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    int posX = 500;
    bodyDef.position.Set((borderScale * m_size.width - posX) / PTM_RATIO, 3);
    m_sensorBody = m_world->CreateBody(&bodyDef);
    
    b2PolygonShape shape;
    shape.SetAsBox(1.8f, 1.8f);
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.isSensor = true;
    m_sensorBody->CreateFixture(&fixtureDef);
}

void HelloWorld::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    GL::enableVertexAttribs( cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION );
    
    m_world->DrawDebugData();
    
    CHECK_GL_ERROR_DEBUG();
}

HelloWorld::~HelloWorld()
{
    CC_SAFE_DELETE(m_world);
    CC_SAFE_DELETE(m_debugDraw);
}

void HelloWorld::createMenuItem(Scene *scene)
{
    auto layer = Layer::create();
    scene->addChild(layer);
    auto menuItem_1 = MenuItemFont::create("moveLeft", CC_CALLBACK_0(HelloWorld::moveLeft, this));
    
    auto menuItem_2 = MenuItemFont::create("moveRight", CC_CALLBACK_0(HelloWorld::moveRight, this));
    
    auto menuItem_3 = MenuItemFont::create("reset", CC_CALLBACK_0(HelloWorld::resetBody, this));
    auto menu = Menu::create(menuItem_1, menuItem_2, menuItem_3, NULL);
    menu->alignItemsVertically();
    layer->addChild(menu);
    menu->setPosition(Vec2(m_size.width - 200, 200));
}

void HelloWorld::moveLeft()
{
    auto transform = m_eraser->GetTransform();
    m_eraser->SetTransform(b2Vec2(transform.p.x - 0.3, transform.p.y), 0);
}

void HelloWorld::moveRight()
{
    auto transform = m_eraser->GetTransform();
    m_eraser->SetTransform(b2Vec2(transform.p.x + 0.3, transform.p.y), 0);
}

void HelloWorld::resetBody()
{
    m_pencilBody->SetTransform(b2Vec2(m_size.width/2/PTM_RATIO, 4.0f), 0);
    
    m_eraser->SetTransform(b2Vec2((m_size.width/2 - 50)/PTM_RATIO, 60/PTM_RATIO), 0);
    m_hasWon = false;
    if(m_goodLuck){
        m_goodLuck->setVisible(false);
    }
}

void HelloWorld::goodLuck()
{
    if(m_goodLuck){
        m_goodLuck->setVisible(true);
        return;
    }
    m_goodLuck = Label::createWithTTF("good luck ,you win !", "fonts/Marker Felt.ttf", 36);
    m_scene->addChild(m_goodLuck);
    m_goodLuck->setPosition(Vec2(m_size.width/2, m_size.height / 2));
}


