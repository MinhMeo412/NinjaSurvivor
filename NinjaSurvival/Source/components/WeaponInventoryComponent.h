#ifndef __WEAPON_INVENTORY_COMPONENT_H__
#define __WEAPON_INVENTORY_COMPONENT_H__

struct WeaponInventoryComponent
{
    std::array<std::pair<std::string,int>, 4> weapons = {};
    std::array<std::pair<std::string, int>, 4> buffs   = {};

    WeaponInventoryComponent() = default;
    WeaponInventoryComponent(const std::string& weapon)
    {
        weapons[0] = {weapon, 1};
    }
};

#endif  // __WEAPON_INVENTORY_COMPONENT_H__
