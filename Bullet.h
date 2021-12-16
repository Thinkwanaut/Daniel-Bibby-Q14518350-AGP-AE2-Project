#pragma once
#include "GameObject.h"
#include "Enemy.h"

class Bullet :
    public GameObject
{
private:

    XMVECTOR m_Direction{ 0, 0, 0 };
    XMFLOAT3 m_Start{ 0, 0, 0 };
    float m_Speed{ 2.5f }, m_Size{ 0.5f }, m_MaxDSq{ 40000 };
    bool m_ThroughEnemies{ false }, m_Hit{ false };
    int m_Damage{ 1 };

public:
    Bullet(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader);

    void Shoot(XMFLOAT3 start, XMFLOAT3 target, float speed = 1.0f, int damage = 1, float range = 200.0f, bool throughEnemies = false);
    bool Move(std::vector<GameObject*> Obstacles, std::vector<Enemy*> enemies, int* index, float adjust=1, bool sweptDetection = true);
    bool TargetCheck(std::vector<Enemy*> Enemies, int* index);
};

