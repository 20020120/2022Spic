#include"WaveManager.h"
#define ProtoType
void WaveManager::fInitialize()
{
    // ‰Šú‰»
    
}

void WaveManager::fUpdate(float elapsedTime_)
{
#ifdef ProtoType
    mEnemyManager.fUpdate(elapsedTime_);

#else

#endif
}

void WaveManager::fFinalize()
{
    throw std::logic_error("Not implemented");
}

void WaveManager::fClear()
{
    throw std::logic_error("Not implemented");
}

const EnemyManager* WaveManager::fGetEnemyManager() const
{
    return &mEnemyManager;
}
