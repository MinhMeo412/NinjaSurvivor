#include "WeaponUpgradeUtils.h"

namespace WeaponUpgradeUtils
{
std::string getDescription(const std::string& name, int level)
{
    if (name == "sword")
    {
        switch (level)
        {
        case 0:
            return "Basic sword equipped";
        case 1:
            return "Slash to faced direction";
        case 2:
            return "Stronger slash with wider range";
        case 3:
            return "Double slash combo";
        default:
            return "No description available";
        }
    }
    else if (name == "shuriken")
    {
        switch (level)
        {
        case 0:
            return "Basic shield equipped";
        case 1:
            return "Block frontal attacks";
        case 2:
            return "Wider block angle";
        case 3:
            return "Reflect projectiles";
        default:
            return "No description available";
        }
    }
    else if (name == "kunai")
    {
        switch (level)
        {
        case 0:
            return "Basic bow equipped";
        case 1:
            return "Shoot single arrow";
        case 2:
            return "Faster arrow speed";
        case 3:
            return "Triple arrow shot";
        default:
            return "No description available";
        }
    }
    return "No description available";
}
}  // namespace WeaponUpgradeUtils
