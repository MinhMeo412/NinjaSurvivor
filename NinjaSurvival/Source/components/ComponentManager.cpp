#include "ComponentManager.h"

template <typename T>
void ComponentManager<T>::addComponent(Entity entity, const T& component)
{
    components[entity] = std::make_unique<T>(component);
}

template <typename T>
T* ComponentManager<T>::getComponent(Entity entity)
{
    auto it = components.find(entity);
    if (it != components.end())
    {
        return it->second.get();
    }
    return nullptr;
}

template <typename T>
void ComponentManager<T>::removeComponent(Entity entity)
{
    components.erase(entity);
}

// Khai báo template cho tất cả kiểu Component
//template class ComponentManager<AttackAreaComponent>;
//template class ComponentManager<AttackComponent>;
//template class ComponentManager<AttackSpeedComponent>;
//template class ComponentManager<CoinComponent>;
//template class ComponentManager<CooldownReductionComponent>;
//template class ComponentManager<DropRateComponent>;
//template class ComponentManager<DurationComponent>;
//template class ComponentManager<GrowthComponent>;
//template class ComponentManager<HealthComponent>;
//template class ComponentManager<MapComponent>;
//template class ComponentManager<PhysicComponent>;
//template class ComponentManager<ProjectileNumberComponent>;
//template class ComponentManager<RecoveryComponent>;
//template class ComponentManager<SpeedComponent>;
//template class ComponentManager<SpriteComponent>;
//template class ComponentManager<TransformComponent>;
//template class ComponentManager<VelocityComponent>;
//template class ComponentManager<WeaponComponent>;
//template class ComponentManager<WeaponInventoryComponent>;
//template class ComponentManager<XPComponent>;
