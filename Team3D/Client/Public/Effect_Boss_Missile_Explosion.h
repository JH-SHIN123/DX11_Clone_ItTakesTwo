#pragma once
#include "InGameEffect.h"

BEGIN(Client)
class CEffect_Boss_Missile_Explosion final : public CInGameEffect
{
private:
	explicit CEffect_Boss_Missile_Explosion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Boss_Missile_Explosion(const CEffect_Boss_Missile_Explosion& rhs);
	virtual ~CEffect_Boss_Missile_Explosion() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	void Set_Pos(_fvector vPos);
	void Set_IsActivate(_bool IsActivate) { m_IsActivate = IsActivate; }
	void Set_WorldMatrix(_fmatrix WorldMatrix) { m_pTransformCom->Set_WorldMatrix(WorldMatrix); }

private:
	void Check_Instance(_double TimeDelta);

private:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0)	override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0)	override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0)		override;

private:
	void Reset_Instance(_double TimeDelta, _float4 vPos, _int iIndex = 0);

private:
	HRESULT Ready_InstanceBuffer();

private:
	_double m_dAlphaTime = 1.0; //
	_bool m_IsActivate = true;
	_double m_dActivateTime = 0.0;

private:
	CVIBuffer_PointInstance_Custom_STT* m_pPointInstanceCom_STT = nullptr;
	CTextures*							m_pTexturesCom_Distortion = nullptr;
	VTXMATRIX_CUSTOM_STT*				m_pInstanceBuffer_STT = nullptr;
	_double*							m_pInstance_Update_TextureUV_Time = nullptr;
	_float3*							m_pInstance_Dir = nullptr;

	_float m_fNextUV = 0.f;
	_float  m_fInstance_SpeedPerSec = 5.f;

	const _float  m_fAlphaTime_Power = 0.5f;
	const _float  m_fSize_Power = 6.f;
	const _double m_dInstance_Pos_Update_Time = 3.25;
	const _float2 m_vDefaultSize = { 3.f, 3.f };

public:
	static CEffect_Boss_Missile_Explosion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END