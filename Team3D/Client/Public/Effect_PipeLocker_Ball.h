#pragma once
#include "InGameEffect_Model.h"

BEGIN(Client)
class CEffect_PipeLocker_Ball final : public CInGameEffect_Model
{
private:
	explicit CEffect_PipeLocker_Ball(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_PipeLocker_Ball(const CEffect_PipeLocker_Ball& rhs);
	virtual ~CEffect_PipeLocker_Ball() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual void	SetUp_WorldMatrix(_fmatrix WorldMatrix) override;

public:
	HRESULT Ready_Nodes();
	void Set_Pos(_vector vPos);

private:
	void Node_Check(_double TimeDelta);

private:
	_double m_dActivateTime = 0.0;
	_float4 m_vColor = { 0.101176534f, 0.101176534f, 1.000000000f, 1.0f };
	_float2 m_vDefaultSize = { 1.5f, 1.5f };
	_int	m_iNodeCount = 0;
	_bool	m_IsEffect_End = false;
	_float4 m_vNodes[12];
	_float	m_fSpeedPerSec = 20.f;

private:
	class CEffect_PipeLocker_Particle* m_pParticle = nullptr;

public:
	static CEffect_PipeLocker_Ball* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END