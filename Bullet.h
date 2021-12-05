#pragma once
#include "GameObject.h"
#include "Enemy.h"

class Bullet :
    public GameObject
{
private:

    XMVECTOR m_Direction{ 0, 0, 0 };
    XMVECTOR m_Start{ 0, 0, 0 };
    float m_Speed{ 0.5f }, m_Size{ 0.05f }, m_MaxDSq{ 50 };

public:
    Bullet(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader);

    void Shoot(XMVECTOR start, XMVECTOR target);
    bool Move(std::vector<GameObject*> Obstacles, float lagAdjust=1);
    int TargetCheck(std::vector<Enemy*> Enemys);
};

