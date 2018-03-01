//
//  ContactListener.hpp
//  CPlus-mobile
//
//  Created by 涛 吴 on 2018/2/27.
//

#ifndef ContactListener_h
#define ContactListener_h

#include "cocos2d.h"
#include "Box2D/Box2D.h"
USING_NS_CC;

class ContactListener : public b2ContactListener
{
private:
    virtual void BeginContact(b2Contact * contact);
};

#endif /* ContactListener_hpp */
