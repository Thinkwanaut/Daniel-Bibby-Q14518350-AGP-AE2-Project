#pragma once
#include "GameObject.h"
#include "Timer.h"
class Enemy :
    public GameObject
{
private:
    float m_Speed{ 0.05f }, m_Size{ 3.0f }, m_FlashTime{ 0.1f };
    XMVECTOR m_FlashCol{ 0.75f, 0.0f, 0.0f, 1.0f }, m_DefaultCol{};
    int m_Health{ 3 };

    char* m_KeyModel = nullptr;
    char* m_KeyTexture = nullptr;

    Timer* mp_Timer = nullptr;

public:
    Enemy(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader);

    void SetKey(char* model, char* texture);

    void Move(GameObject* target, std::vector<GameObject*> Obstacles, float lagAdjust=1);
    void GetHit(int damage = 1);
    bool IsDead();

    GameObject* SpawnKey();
};

