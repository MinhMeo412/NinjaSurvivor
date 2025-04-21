#include "WeaponUpgradeUtils.h"

namespace WeaponUpgradeUtils
{
std::string getDescription(const std::string& name, int level)
{
    std::string description;

    // Weapons
    if (name == "sword")
    {
        description = "Katana";
        switch (level)
        {
        case 1:
            description += std::string(" - Level 1") + "\n" + "Slash to faced direction";
            return description;
        case 2:
            description += std::string(" - Level 2") + "\n" + "Add another backward slash";
            return description;
        case 3:
            description += std::string(" - Level 3") + "\n" + "Reduce slash cooldown & Increase damage";
            return description;
        case 4:
            description += std::string(" - Level 4") + "\n" + "Reduce slash cooldown";
            return description;
        case 5:
            description += std::string(" - Level 5") + "\n" + "Increase overall size & Increase damage";
            return description;
        default:
            return "No description available";
        }
    }
    else if (name == "shuriken")
    {
        description = "Shuriken Storm";
        switch (level)
        {
        case 1:
            description += std::string(" - Level 1") + "\n" + "Create a shuriken moves around character";
            return description;
        case 2:
            description += std::string(" - Level 2") + "\n" + "Add shuriken";
            return description;
        case 3:
            description += std::string(" - Level 3") + "\n" + "Add shuriken & Reduce cooldown";
            return description;
        case 4:
            description += std::string(" - Level 4") + "\n" + "Add shuriken & Reduce cooldown";
            return description;
        case 5:
            description += std::string(" - Level 5") + "\n" + "Add shuriken & Increase overall size & Remove CD & Increase damage";
            return description;
        default:
            return "No description available";
        }
    }
    else if (name == "kunai")
    {
        description = "Kunai";
        switch (level)
        {
        case 1:
            description += std::string(" - Level 1") + "\n" + "Throw kunai at move direction";
            return description;
        case 2:
            description += std::string(" - Level 2") + "\n" + "Reduce CD";
            return description;
        case 3:
            description += std::string(" - Level 3") + "\n" + "Reduce CD & Increase damage";
            return description;
        case 4:
            description += std::string(" - Level 4") + "\n" + "Reduce CD";
            return description;
        case 5:
            description += std::string(" - Level 5") + "\n" + "Reduce CD & Increase damage";
            return description;
        default:
            return "No description available";
        }
    }
    else if (name == "big_kunai")
    {
        description = "Another Kunai";
        switch (level)
        {
        case 1:
            description += std::string(" - Level 1") + "\n" + "Throw a big kunai at random direction and bounce off the wall";
            return description;
        case 2:
            description += std::string(" - Level 2") + "\n" + "Reduce CD & Increase size";
            return description;
        case 3:
            description += std::string(" - Level 3") + "\n" + "Increase duration & damage & speed";
            return description;
        case 4:
            description += std::string(" - Level 4") + "\n" + "Increase duration & Size";
            return description;
        case 5:
            description += std::string(" - Level 5") + "\n" + "Reduce CD & Increase damage & Add 1 Big Kunai";
            return description;
        default:
            return "No description available";
        }
    }
    else if (name == "spinner")
    {
        description = "Death Spinner";
        switch (level)
        {
        case 1:
            description += std::string(" - Level 1") + "\n" + "Summon deadly shuriken at random enemy and deals damage";
            return description;
        case 2:
            description += std::string(" - Level 2") + "\n" + "Reduce CD";
            return description;
        case 3:
            description += std::string(" - Level 3") + "\n" + "Add 1 Death Spinner & Increase duration";
            return description;
        case 4:
            description += std::string(" - Level 4") + "\n" + "Increase duration & Reduce CD";
            return description;
        case 5:
            description += std::string(" - Level 5") + "\n" + "Add 1 Death Spinner & Increase damage & Size";
            return description;
        default:
            return "No description available";
        }
    }
    else if (name == "explosion_kunai")
    {
        description = "Explosive Shot";
        switch (level)
        {
        case 1:
            description += std::string(" - Level 1") + "\n" + "Throw kunai to random enemy and explode on impact";
            return description;
        case 2:
            description += std::string(" - Level 2") + "\n" + "Reduce CD";
            return description;
        case 3:
            description += std::string(" - Level 3") + "\n" + "Add 1 Explosive Shot";
            return description;
        case 4:
            description += std::string(" - Level 4") + "\n" + "Increase damage & Reduce CD";
            return description;
        case 5:
            description += std::string(" - Level 5") + "\n" + "Add 1 Explosive Shot & Increase damage";
            return description;
        default:
            return "No description available";
        }
    }
    else if (name == "ninjutsu_spell")
    {
        description = "Ninjutsu Spell";
        switch (level)
        {
        case 1:
            description += std::string(" - Level 1") + "\n" + "Shot at random nearest enemy";
            return description;
        case 2:
            description += std::string(" - Level 2") + "\n" + "Add 1 spell projectile";
            return description;
        case 3:
            description += std::string(" - Level 3") + "\n" + "Add 1 spell projectile & Reduce CD";
            return description;
        case 4:
            description += std::string(" - Level 4") + "\n" + "Add 1 spell projectile & Reduce CD";
            return description;
        case 5:
            description += std::string(" - Level 5") + "\n" + "Reduce CD & Increase damage";
            return description;
        default:
            return "No description available";
        }
    }


    // Buffs
    else if (name == "attack")
    {
        description = "Power Strike";
        switch (level)
        {
        case 1:
            description += std::string(" - Level 1") + "\n" + "Increase 10% attack";
            return description;
        case 2:
            description += std::string(" - Level 2") + "\n" + "Increase 10% attack";
            return description;
        case 3:
            description += std::string(" - Level 3") + "\n" + "Increase 10% attack";
            return description;
        case 4:
            description += std::string(" - Level 4") + "\n" + "Increase 10% attack";
            return description;
        case 5:
            description += std::string(" - Level 5") + "\n" + "Increase 10% attack";
            return description;
        default:
            return "No description available";
        }
    }
    else if (name == "health")
    {
        description = "Endurance";
        switch (level)
        {
        case 1:
            description += std::string(" - Level 1") + "\n" + "Increase 10% max health";
            return description;
        case 2:
            description += std::string(" - Level 2") + "\n" + "Increase 10% max health";
            return description;
        case 3:
            description += std::string(" - Level 3") + "\n" + "Increase 10% max health";
            return description;
        case 4:
            description += std::string(" - Level 4") + "\n" + "Increase 10% max health";
            return description;
        case 5:
            description += std::string(" - Level 5") + "\n" + "Increase 10% max health";
            return description;
        default:
            return "No description available";
        }
    }
    else if (name == "speed")
    {
        description = "Haste";
        switch (level)
        {
        case 1:
            description += std::string(" - Level 1") + "\n" + "Increase 10% move speed";
            return description;
        case 2:
            description += std::string(" - Level 2") + "\n" + "Increase 10% move speed";
            return description;
        case 3:
            description += std::string(" - Level 3") + "\n" + "Increase 10% move speed";
            return description;
        case 4:
            description += std::string(" - Level 4") + "\n" + "Increase 10% move speed";
            return description;
        case 5:
            description += std::string(" - Level 5") + "\n" + "Increase 10% move speed";
            return description;
        default:
            return "No description available";
        }
    }
    else if (name == "xp_gain")
    {
        description = "Mind Expansion";
        switch (level)
        {
        case 1:
            description += std::string(" - Level 1") + "\n" + "Increase 10% XP gain";
            return description;
        case 2:
            description += std::string(" - Level 2") + "\n" + "Increase 10% XP gain";
            return description;
        case 3:
            description += std::string(" - Level 3") + "\n" + "Increase 10% XP gain";
            return description;
        case 4:
            description += std::string(" - Level 4") + "\n" + "Increase 10% XP gain";
            return description;
        case 5:
            description += std::string(" - Level 5") + "\n" + "Increase 10% XP gain";
            return description;
        default:
            return "No description available";
        }
    }
    else if (name == "pickup_range")
    {
        description = "Magnetic Field";
        switch (level)
        {
        case 1:
            description += std::string(" - Level 1") + "\n" + "Increase 10% pickup range";
            return description;
        case 2:
            description += std::string(" - Level 2") + "\n" + "Increase 10% pickup range";
            return description;
        case 3:
            description += std::string(" - Level 3") + "\n" + "Increase 10% pickup range";
            return description;
        case 4:
            description += std::string(" - Level 4") + "\n" + "Increase 10% pickup range";
            return description;
        case 5:
            description += std::string(" - Level 5") + "\n" + "Increase 10% pickup range";
            return description;
        default:
            return "No description available";
        }
    }
    else if (name == "reduce_receive_damage")
    {
        description = "Fortified Body";
        switch (level)
        {
        case 1:
            description += std::string(" - Level 1") + "\n" + "Reduce 5% receive damage";
            return description;
        case 2:
            description += std::string(" - Level 2") + "\n" + "Reduce 5% receive damage";
            return description;
        case 3:
            description += std::string(" - Level 3") + "\n" + "Reduce 5% receive damage";
            return description;
        case 4:
            description += std::string(" - Level 4") + "\n" + "Reduce 5% receive damage";
            return description;
        case 5:
            description += std::string(" - Level 5") + "\n" + "Reduce 5% receive damage";
            return description;
        default:
            return "No description available";
        }
    }
    else if (name == "coin_gain")
    {
        description = "Fortune Favors";
        switch (level)
        {
        case 1:
            description += std::string(" - Level 1") + "\n" + "Increase 10% coin gain";
            return description;
        case 2:
            description += std::string(" - Level 2") + "\n" + "Increase 10% coin gain";
            return description;
        case 3:
            description += std::string(" - Level 3") + "\n" + "Increase 10% coin gain";
            return description;
        case 4:
            description += std::string(" - Level 4") + "\n" + "Increase 10% coin gain";
            return description;
        case 5:
            description += std::string(" - Level 5") + "\n" + "Increase 10% coin gain";
            return description;
        default:
            return "No description available";
        }
    }
    else if (name == "curse")
    {
        description = "Devil's Deal";
        switch (level)
        {
        case 1:
            description += std::string(" - Level 1") + "\n" + "Increase 10% attack damage & Receive 10% more damage";
            return description;
        case 2:
            description += std::string(" - Level 2") + "\n" + "Increase 10% attack damage & Receive 10% more damage";
            return description;
        case 3:
            description += std::string(" - Level 3") + "\n" + "Increase 10% attack damage & Receive 10% more damage";
            return description;
        case 4:
            description += std::string(" - Level 4") + "\n" + "Increase 10% attack damage & Receive 10% more damage";
            return description;
        case 5:
            description += std::string(" - Level 5") + "\n" + "Increase 10% attack damage & Receive 10% more damage";
            return description;
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
