#ifndef __IDENTITY_COMPONENT_H__
#define __IDENTITY_COMPONENT_H__

#include <string>

struct IdentityComponent
{
    std::string type;
    std::string name;

    IdentityComponent(const std::string& type, const std::string& name) : type(type), name(name) {}
};

#endif  // __IDENTITY_COMPONENT_H__
