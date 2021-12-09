#pragma once
#include "GameObject.h"
#include "Enemy.h"

class Bullet :
    public GameObject
{
private:

    XMVECTOR m_Direction{ 0, 0, 0 };
    XMFLOAT3 m_Start{ 0, 0, 0 };
    float m_Speed{ .25f }, m_Size{ 0.5f }, m_MaxDSq{ 40000 };
    bool m_ThroughEnemies{ false };
    int m_Damage{ 1 };

public:
    Bullet(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader);

    void Shoot(XMFLOAT3 start, XMFLOAT3 target, int damage = 1, bool throughEnemies = false);
    bool Move(std::vector<GameObject*> Obstacles, float lagAdjust=1);
    bool TargetCheck(std::vector<Enemy*> Enemies, int* index);
};

