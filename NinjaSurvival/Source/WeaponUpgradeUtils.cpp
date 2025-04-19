#include "WeaponUpgradeUtils.h"

namespace WeaponUpgradeUtils
{
std::string getDescription(const std::string& name, int level)
{
    std::string description;

    // Weapons
    if (name == "sword")
    {
        description = "Sword";
        switch (level)
        {
        case 1:
            description += std::string(" - Level 1") + "\n" + "Slash to faced direction";
            return description;
        case 2:
            return "Add another backward slash";
        case 3:
            return "Reduce slash cooldown & Increase dame";
        case 4:
            return "Reduce slash cooldown";
        case 5:
            return "Increase overall size & Increase dame";
        default:
            return "No description available";
        }
    }
    else if (name == "shuriken")
    {
        switch (level)
        {
        case 1:
            return "Create a shuriken moves around character";
        case 2:
            return "Add shuriken";
        case 3:
            return "Add shuriken & Reduce cooldown";
        case 4:
            return "Add shuriken & Reduce cooldown";
        case 5:
            return "Add shuriken & Increase overall size & Remove CD & Increase dame";
        default:
            return "No description available";
        }
    }
    else if (name == "kunai")
    {
        switch (level)
        {
        case 1:
            return "Throw kunai at move direction";
        case 2:
            return "Reduce CD";
        case 3:
            return "Reduce CD & Increase dame";
        case 4:
            return "Reduce CD";
        case 5:
            return "Reduce CD & Increase dame";
        default:
            return "No description available";
        }
    }
    else if (name == "big_kunai")
    {
        switch (level)
        {
        case 1:
            return "Throw a big kunai at random direction and bounce off the wall";
        case 2:
            return "Reduce CD & Increase size";
        case 3:
            return "Increase duration & dame & speed";
        case 4:
            return "Increase duration & size";
        case 5:
            return "Reduce CD & Increase dame & Add 1 Big Kunai";
        default:
            return "No description available";
        }
    }
    // Buffs
    else if (name == "attack")
    {
        switch (level)
        {
        case 1:
            return "Increase 10% attack";
        case 2:
            return "Increase 10% attack";
        case 3:
            return "Increase 10% attack";
        case 4:
            return "Increase 10% attack";
        case 5:
            return "Increase 10% attack";
        default:
            return "No description available";
        }
    }
    else if (name == "health")
    {
        switch (level)
        {
        case 1:
            return "Increase 10% health";
        case 2:
            return "Increase 10% health";
        case 3:
            return "Increase 10% health";
        case 4:
            return "Increase 10% health";
        case 5:
            return "Increase 10% health";
        default:
            return "No description available";
        }
    }
    else if (name == "speed")
    {
        switch (level)
        {
        case 1:
            return "Increase 10% speed";
        case 2:
            return "Increase 10% speed";
        case 3:
            return "Increase 10% speed";
        case 4:
            return "Increase 10% speed";
        case 5:
            return "Increase 10% speed";
        default:
            return "No description available";
        }
    }
    else if (name == "xp_gain")
    {
        switch (level)
        {
        case 1:
            return "Increase 10% XP gain";
        case 2:
            return "Increase 10% XP gain";
        case 3:
            return "Increase 10% XP gain";
        case 4:
            return "Increase 10% XP gain";
        case 5:
            return "Increase 10% XP gain";
        default:
            return "No description available";
        }
    }
    //Other items
    else if (name == "coin")
    {
        switch (level)
        {
        case 0:
            return "Get 25 coin";
        default:
            return "No description available";
        }
    }
    else if (name == "heart")
    {
        switch (level)
        {
        case 0:
            return "Recover 25 HP";
        default:
            return "No description available";
        }
    }
    return "No description available";
}
}  // namespace WeaponUpgradeUtils
