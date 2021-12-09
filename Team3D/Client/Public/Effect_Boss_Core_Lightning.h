#pragma once

#include "InGameEffect.h"

BEGIN(Client)
class CEffect_Boss_Core_Lightning final : public CInGameEffect
{
private:
	explicit CEffect_Boss_Core_Lightning(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Boss_Core_Lightning(const CEffect_Boss_Core_Lightning& rhs);
	virtual ~CEffect_Boss_Core_Lightning() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;

private:
	HRESULT Ready_InstanceBuffer();
	_float2 Get_Rand_Size();
	void Check_InstanceBuffer(_double TimeDelta);

private:
	CVIBuffer_PointInstance_Custom_STT* m_pPointInstanceCom_STT = nullptr;
	VTXMATRIX_CUSTOM_STT*				m_pInstanceBuffer_STT = nullptr;
	_float3*							m_pInstanceBiffer_Dir = nullptr;
	XMINT2*								m_pInstanceBuffer_UVCount = nullptr;
	_double* m_pInstance_UVTime = nullptr;

	_bool m_IsActivate = true;
	_double m_dActivateTime = 0.0;
	_double m_dControlTime = 0.0;

	const _double	m_dTextureUV_Time	= 0.01;
	const XMINT2	m_vUVCount_Max		= { 4, 8 };
	const _float	m_fAlphaTime_Power	= 2.f;
	const _float	m_fInstance_SpeedPerSec = 0.2f;
	const _double	m_dInstance_Pos_Update_Time = 3.0;
	const _float2	m_vDefaultSize = { 2.5f, 2.5f };

public:
	static CEffect_Boss_Core_Lightning* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END