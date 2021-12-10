#pragma once

#include "Laser.h"

BEGIN(Client)

class CLaser_TypeB final : public CLaser
{
private:
	explicit CLaser_TypeB(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLaser_TypeB(const CLaser_TypeB& rhs);
	virtual	~CLaser_TypeB() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eRender) override;
	virtual HRESULT Render_ShadowDepth() override;

public:
	void Set_StartPoint(_float4 vStartPoint);
	void Set_LaserDir(_float4 vLaserDir);
	void SetUp_Direction(_uint iOption);
	void Set_RotateSpeed(_float fSpeed);

public:
	_bool Get_LaserRecovery() const { return m_IsLaserRecovery; }

private:
	_float						m_fAngle = 0.f;
	_float						m_fRotateSpeed = 0.f;
	_uint						m_iOption = 0;
	_float4x4					m_matRotY;
	_bool						m_IsLaserRecovery = false;

public:
	static CLaser_TypeB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END