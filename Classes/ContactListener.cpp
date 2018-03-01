//
//  ContactListener.cpp
//  CPlus-mobile
//
//  Created by 涛 吴 on 2018/2/27.
//

#include "ContactListener.h"

void ContactListener::BeginContact(b2Contact * contact)
{
    b2Body * bodyA = contact->GetFixtureA()->GetBody();
    b2Body * bodyB = contact->GetFixtureB()->GetBody();
    
    auto spriteA = (Sprite *)bodyA->GetUserData();
    auto spriteB = (Sprite *)bodyB->GetUserData();
    
    if(spriteA != nullptr && spriteB != nullptr)
    {
        spriteA->setColor(Color3B::YELLOW);
        spriteB->setColor(Color3B::YELLOW);
    }
}
