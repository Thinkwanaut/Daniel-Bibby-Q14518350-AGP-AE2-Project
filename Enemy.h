#pragma once
#include "GameObject.h"
class Enemy :
    public GameObject
{
private:
    float m_Speed{ 0.1 };

    int m_Health{ 3 };

    char* m_KeyModel = nullptr;
    char* m_KeyTexture = nullptr;

public:
    Enemy(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader);

    void SetKey(char* model, char* texture);

    void Move(GameObject* target, std::vector<GameObject*> Obstacles, float lagAdjust=1);
    void GetHit();
    bool IsDead();

    GameObject* SpawnKey();
};

