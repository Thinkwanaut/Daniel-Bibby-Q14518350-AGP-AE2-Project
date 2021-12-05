#pragma once
#include <DirectXCollision.h>
#define _XM_NO_INSTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>

#include "Drawable.h"

#include "objfilemodel.h"

enum class ColliderShape
{
	None, Box, Sphere
};

class GameObject : public Drawable
{
protected:	
	ObjFileModel* mp_Object = nullptr;

	BoundingBox* mp_BoxCollider = NULL;
	BoundingSphere* mp_SphereCollider = NULL;
	XMFLOAT3 m_collisionPos{ 0, 0, 0 };
	XMFLOAT3 m_dimensions{ 0, 0, 0 };

	ColliderShape m_collisionType = ColliderShape::None;

	bool SphereCollision(std::vector<GameObject*> others);
	bool SphereCollision(GameObject* other);
	bool BoxCollision(std::vector<GameObject*> others);
	bool BoxCollision(GameObject* other);

	float m_FallVel{ 0 };
	bool m_Grounded{ false }, m_Falls{ true };

public:
	GameObject(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader);
	~GameObject();

	void SetCollisionPos();
	XMFLOAT3 GetCollisionPos();
	XMFLOAT3 GetDimensions();
	float GetSphereRad();
	BoundingBox GetBox();
	BoundingSphere GetSphere();
	ColliderShape GetCollisionType();
	bool CheckCollision(std::vector<GameObject*> others);
	bool CheckCollision(GameObject* other);
	bool CheckCollisionX(GameObject* other);
	bool CheckCollisionY(GameObject* other);
	bool CheckCollisionZ(GameObject* other);

	bool CheckCollisionSphere(GameObject* other);

	float GetX();
	float GetY();
	float GetZ();
	
	void SetCollisionType(ColliderShape shape);
	void LookAt_XZ(float x, float z);
	void LookAt(float x, float y, float z);
	void MoveForward_XZ(float step, std::vector<GameObject*> others, float adjust = 1);
	void MoveForward(float step, float adjust = 1);
	void MoveForward(float step, std::vector<GameObject*> others, float adjust = 1);
	void Fall(std::vector<GameObject*> obstacles, float grav = 1, float adjust = 1);
	void SetFall(bool fall);
	void GetPushed(std::vector<GameObject*> pushers, std::vector<GameObject*> obstacles);
	void MoveToTop(GameObject* other, float offset = 0);

	void Draw(XMMATRIX view, XMMATRIX projection, Light* ambient, DirectionalLight* directional);
};

