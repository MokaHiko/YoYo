#pragma once

#include "ScriptableEntity.h"

struct VillageItem
{

};

struct VillagerProps
{
    int gender;
};

struct VillageProps
{
    float spawn_rate;
    int max_villagers;
};

class VillageManagerComponent : public ScriptableEntity
{
public:
    VillageManagerComponent(Entity e);
    virtual ~VillageManagerComponent();

    // virtual void OnCreate() override;
    // virtual void OnStart() override;
    virtual void OnUpdate(float dt) override;

    void SpawnVillager(const VillagerProps& props = {});
    void SpawnItem(const VillageItem& item);
private:
    float m_timer = 0;
    int m_villager_count = 0;
};


