#include "GameObject.h"

bool GameObject::SphereCollision(std::vector<GameObject*> others)
{
	for (GameObject* other : others)
	{
		if (other == this) continue;

		switch (other->GetCollisionType())
		{
		case ColliderShape::Box: 
			return GetSphere().Intersects(other->GetBox());
		case ColliderShape::Sphere:
			return CheckCollisionSphere(other);
		}
	}
	return false;
}

bool GameObject::SphereCollision(GameObject* other)
{
	switch (other->GetCollisionType())
	{
	case ColliderShape::Box:
		return GetSphere().Intersects(other->GetBox());
	case ColliderShape::Sphere:
		return CheckCollisionSphere(other);
	default:
		return false;
	}
}

bool GameObject::BoxCollision(std::vector<GameObject*> others)
{
	for (GameObject* other : others)
	{
		if (other == this) continue;

		switch (other->GetCollisionType())
		{
		case ColliderShape::Box:
			return CheckCollisionX(other) && CheckCollisionY(other) && CheckCollisionZ(other);
		case ColliderShape::Sphere:
			return GetBox().Intersects(other->GetSphere());
		}
	}
	return false;
}

bool GameObject::BoxCollision(GameObject* other)
{
	switch (other->GetCollisionType())
	{
	case ColliderShape::Box:
		return CheckCollisionX(other) && CheckCollisionY(other) && CheckCollisionZ(other);
	case ColliderShape::Sphere:
		return GetBox().Intersects(other->GetSphere());
	default:
		return false;
	}
}

GameObject::GameObject(ID3D11Device* device, ID3D11DeviceContext* context, AssetManager* assets, char* model, char* texture, char* shader)
	: Drawable{ device, context, assets, texture, shader }
{
	mp_Object = assets->GetModel(model);

	SetCollisionPos();

	mp_BoxCollider = new BoundingBox(m_collisionPos, m_dimensions);
	mp_SphereCollider = new BoundingSphere(m_collisionPos, GetSphereRad());
}

GameObject::~GameObject()
{
	if (mp_Object) mp_Object = nullptr; // Passed in and deleted from asset manager
	if (mp_BoxCollider)
	{
		delete mp_BoxCollider;
		mp_BoxCollider = nullptr;
	}
	if (mp_SphereCollider)
	{
		delete mp_SphereCollider;
		mp_SphereCollider = nullptr;
	}
}

bool GameObject::CheckCollision(std::vector<GameObject*> others)
{
	switch (m_collisionType)
	{
	case ColliderShape::Box:
		return BoxCollision(others);
	case ColliderShape::Sphere:
		return SphereCollision(others);
	default:
		return false;
	}
}

bool GameObject::CheckCollision(GameObject* other)
{
	switch (m_collisionType)
	{
	case ColliderShape::Box:
		return BoxCollision(other);
	case ColliderShape::Sphere:
		return SphereCollision(other);
	default:
		return false;
	}
}

bool GameObject::CheckCollisionX(GameObject* other)
{
	return abs(m_x - other->GetX()) < other->GetDimensions().x + GetDimensions().x;
}

bool GameObject::CheckCollisionY(GameObject* other)
{
	return abs(m_y - other->GetY()) < other->GetDimensions().y + GetDimensions().y;
}

bool GameObject::CheckCollisionZ(GameObject* other)
{
	return abs(m_z - other->GetZ()) < other->GetDimensions().z + GetDimensions().z;
}

bool GameObject::CheckCollisionSphere(GameObject* other)
{
	float d_sq = powf(GetX() - other->GetX(), 2) + powf(GetY() - other->GetY(), 2) + powf(GetZ() - other->GetZ(), 2);
	return (d_sq <= powf(GetDimensions().x + other->GetDimensions().x, 2));
}

bool GameObject::PassedThrough(GameObject* target)
{
	return (m_PrevX < target->GetPrevX() && target->GetX() < m_x || m_x < target->GetX() && target->GetPrevX() < m_PrevX)
		&& (m_PrevY < target->GetPrevY() && target->GetY() < m_y || m_y < target->GetY() && target->GetPrevY() < m_PrevY)
		&& (m_PrevZ < target->GetPrevZ() && target->GetZ() < m_z || m_z < target->GetZ() && target->GetPrevZ() < m_PrevZ);
}

void GameObject::SetCollisionPos()
{
	XMFLOAT3 minPoint{ 0, 0, 0 }, maxPoint{ 0, 0, 0 }, centrePoint{ 0, 0, 0 };

	for (int v = 0; v < mp_Object->numverts; v++)
	{
		minPoint.x = min(minPoint.x, mp_Object->vertices[v].Pos.x);
		minPoint.y = min(minPoint.y, mp_Object->vertices[v].Pos.y);
		minPoint.z = min(minPoint.z, mp_Object->vertices[v].Pos.z);

		maxPoint.x = max(maxPoint.x, mp_Object->vertices[v].Pos.x);
		maxPoint.y = max(maxPoint.y, mp_Object->vertices[v].Pos.y);
		maxPoint.z = max(maxPoint.z, mp_Object->vertices[v].Pos.z);
	}

	centrePoint.x = minPoint.x + (maxPoint.x - minPoint.x) / 2;
	centrePoint.y = minPoint.y + (maxPoint.y - minPoint.y) / 2;
	centrePoint.z = minPoint.z + (maxPoint.z - minPoint.z) / 2;

	m_collisionPos = centrePoint;

	m_dimensions.x = (maxPoint.x - minPoint.x) / 2;
	m_dimensions.y = (maxPoint.y - minPoint.y) / 2;
	m_dimensions.z = (maxPoint.z - minPoint.z) / 2;
}

XMFLOAT3 GameObject::GetCollisionPos()
{
	return m_collisionPos;
}

XMFLOAT3 GameObject::GetDimensions()
{
	switch (m_collisionType)
	{
	case ColliderShape::Box:
		return XMFLOAT3(m_dimensions.x * m_xScale, m_dimensions.y * m_yScale, m_dimensions.z * m_zScale);
	case ColliderShape::Sphere:
		return XMFLOAT3(mp_SphereCollider->Radius * m_xScale, mp_SphereCollider->Radius * m_yScale, mp_SphereCollider->Radius * m_zScale);
	default:
		break;
	};
}

float GameObject::GetSphereRad()
{
	float maxRad = 0;
	for (int v = 0; v < mp_Object->numverts; v++)
	{
		float rad = powf(powf(mp_Object->vertices[v].Pos.x - m_collisionPos.x, 2) +
			powf(mp_Object->vertices[v].Pos.y - m_collisionPos.y, 2) +
			powf(mp_Object->vertices[v].Pos.z - m_collisionPos.z, 2), 0.5f);

		maxRad = max(maxRad, rad);
	}
	return maxRad;
}

BoundingBox GameObject::GetBox()
{
	BoundingBox transformed = *mp_BoxCollider;
	transformed.Transform(transformed, GetWorldMatrix());
	return transformed;
}

BoundingSphere GameObject::GetSphere()
{
	BoundingSphere transformed = *mp_SphereCollider;
	transformed.Transform(transformed, GetWorldMatrix());
	return transformed;
}

ColliderShape GameObject::GetCollisionType()
{
	return m_collisionType;
}

float GameObject::GetX()
{
	return m_x; 
}

float GameObject::GetY() 
{ 
	return m_y;
}

float GameObject::GetZ()
{
	return m_z;
}

float GameObject::GetPrevX()
{
	return m_PrevX;
}

float GameObject::GetPrevY()
{
	return m_PrevY;
}

float GameObject::GetPrevZ()
{
	return m_PrevZ;
}

void GameObject::SetCollisionType(ColliderShape shape)
{
	m_collisionType = shape;
}

void GameObject::LookAt_XZ(float x, float z)
{
	float dx = x - m_x;
	float dz = z - m_z;
	m_yRot = XMConvertToDegrees(atan2f(dx, dz));
}

void GameObject::LookAt(float x, float y, float z)
{
	LookAt_XZ(x, z);
	m_xRot = XMConvertToDegrees(-atan2f(y - m_y, powf(powf(x - m_x, 2) + powf(z - m_z, 2), 0.5f)));
}

void GameObject::MoveForward_XZ(float step, std::vector<GameObject*> others, float adjust)
{
	m_x += sin(XMConvertToRadians(m_yRot)) * step * adjust;
	if (CheckCollision(others)) m_x -= sin(XMConvertToRadians(m_yRot)) * step * adjust;
	m_z += cos(XMConvertToRadians(m_yRot)) * step * adjust;
	if (CheckCollision(others)) m_z -= cos(XMConvertToRadians(m_yRot)) * step * adjust;
}

void GameObject::MoveForward(float step, float adjust)
{
	m_x += sin(XMConvertToRadians(m_yRot)) * step * cos(XMConvertToRadians(m_xRot)) * adjust;
	m_y += -sin(XMConvertToRadians(m_xRot)) * step * adjust;
	m_z += cos(XMConvertToRadians(m_yRot)) * step * cos(XMConvertToRadians(m_xRot)) * adjust;
}

void GameObject::MoveForward(float step, std::vector<GameObject*> others, float adjust)
{
	float tempx{ m_x }, tempy{ m_y }, tempz{ m_z };

	m_x += sin(XMConvertToRadians(m_yRot)) * step * cos(XMConvertToRadians(m_xRot)) * adjust;
	for (GameObject* o : others)
		if (CheckCollision(o)) m_x = tempx;

	m_y += -sin(XMConvertToRadians(m_xRot)) * step * adjust;
	for (GameObject* o : others)
		if (CheckCollision(o)) m_y = tempy;

	m_z += cos(XMConvertToRadians(m_yRot)) * step * cos(XMConvertToRadians(m_xRot)) * adjust;
	for (GameObject* o : others)
		if (CheckCollision(o)) m_z = tempz;
}

void GameObject::Fall(std::vector<GameObject*> obstacles, float grav, float adjust)
{
	if (!m_Falls) return;

	m_FallVel += grav * adjust;
	m_y -= m_FallVel * adjust;

	m_Grounded = false;
	for (GameObject* object : obstacles)
	{
		if (object == this) continue;
		if (CheckCollision(object))
		{
			MoveToTop(object, grav); // Add grav to prevent collision with floor when walking
			return;
		}
	}
}

void GameObject::SetFall(bool falls)
{
	m_Falls = falls;
}

void GameObject::GetPushed(std::vector<GameObject*> pushers, std::vector<GameObject*> obstacles)
{
	for (GameObject* p : pushers)
	{
		if (p == this) continue;

		float tempX = m_x, tempZ = m_z;
		if (CheckCollision(p))
		{
			// Calculate which side 'pusher' is closest to
			int xDir = (p->GetX() < m_x) ? 1 : -1;
			int zDir = (p->GetZ() < m_z) ? 1 : -1;
			int yDir = (p->GetY() < m_y) ? 1 : -1;
			float dx = abs((p->GetX() + p->GetDimensions().x * xDir) - (m_x + GetDimensions().x * -xDir));
			float dz = abs((p->GetZ() + p->GetDimensions().z * zDir) - (m_z + GetDimensions().z * -zDir));
			float dy = abs((p->GetY() + p->GetDimensions().y * yDir) - (m_y + GetDimensions().y * -yDir));

			if (dx < dz && dx < dy) m_x = p->GetX() + (p->GetDimensions().x + GetDimensions().x) * xDir;
			else if (dz < dy) m_z = p->GetZ() + (p->GetDimensions().z + GetDimensions().z) * zDir;
			else (p->MoveToTop(this));

			for (GameObject* o : obstacles)
			{
				if (CheckCollision(o))
				{
					if (dx < dz && dx < dy)
					{
						m_x = tempX;
						p->SetX(GetX() - (p->GetDimensions().x + GetDimensions().x) * xDir);
					}
					else if (dz < dy)
					{
						m_z = tempZ;
						p->SetZ(GetZ() - (p->GetDimensions().z + GetDimensions().z) * zDir);
					}

					break;
				}
			}
		}
	}
}

void GameObject::MoveToTop(GameObject* other, float offset)
{
	int yDir = (other->GetY() < m_y) ? 1 : -1;
	m_y = other->GetY() + ((other->GetDimensions().y + GetDimensions().y) + offset) * yDir;
	m_Grounded = yDir == 1;
	m_FallVel = m_Grounded ? 0 : max(m_FallVel, 0);
}

void GameObject::SetPos(float x, float y, float z)
{
	Drawable::SetPos(x, y, z);
	m_PrevX = x; m_PrevY = y; m_PrevZ = z;
}

void GameObject::SetPos(XMFLOAT3 pos)
{
	Drawable::SetPos(pos);
	m_PrevX = pos.x; m_PrevY = pos.y; m_PrevZ = pos.z;
}

void GameObject::Draw(XMMATRIX view, XMMATRIX projection, Light* ambient, DirectionalLight* directional, PointLight* point)
{
	Drawable::Draw(view, projection, ambient, directional, point);
	mp_Object->Draw();

	m_PrevX = m_x; m_PrevY = m_y; m_PrevZ = m_z;
}
