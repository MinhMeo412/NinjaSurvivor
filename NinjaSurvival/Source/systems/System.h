#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "axmol.h"

class System
{
public:
    virtual void init() = 0;
    virtual void update(float dt) = 0;
    virtual ~System()             = default;
};

#endif  // __SYSTEM_H__
