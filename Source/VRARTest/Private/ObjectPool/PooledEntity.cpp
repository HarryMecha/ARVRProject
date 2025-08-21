#include "ObjectPool/PooledEntity.h"
#include "Map/MapSection.h"

void APooledEntity::BeginPlay()
{
    Super::BeginPlay();
    // You can leave this empty or add default logic.
}

void APooledEntity::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // Also safe to leave empty or extend in child classes.
}