#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(XMVECTOR direction, XMVECTOR colour)
	: m_direction{ direction }, Light{ colour }
{
}

XMVECTOR DirectionalLight::Direction()
{
	XMMATRIX rx, ry, rz;

	rx = XMMatrixRotationX(XMConvertToRadians(m_rotation.x));
	ry = XMMatrixRotationY(XMConvertToRadians(m_rotation.y));
	rz = XMMatrixRotationZ(XMConvertToRadians(m_rotation.z));

	return XMVector3Transform(m_direction, rx * ry * rz);
}

void DirectionalLight::SetDirection(XMVECTOR direction)
{
	m_direction = direction;
}

void DirectionalLight::SetRotation(XMFLOAT3 rotation)
{
	m_rotation = rotation;
}

void DirectionalLight::Rotate(XMFLOAT3 rotation)
{
	m_rotation.x = fmodf(m_rotation.x + rotation.x, 360);
	m_rotation.y = fmodf(m_rotation.y + rotation.y, 360);
	m_rotation.z = fmodf(m_rotation.z + rotation.z, 360);
}
