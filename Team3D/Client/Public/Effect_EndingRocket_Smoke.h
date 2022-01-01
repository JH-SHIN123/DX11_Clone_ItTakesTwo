#pragma once
#include "InGameEffect.h"

BEGIN(Client)
class CEffect_EndingRocket_Smoke final : public CInGameEffect
{
private:
	explicit CEffect_EndingRocket_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_EndingRocket_Smoke(const CEffect_EndingRocket_Smoke& rhs);
	virtual ~CEffect_EndingRocket_Smoke() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	void Set_Pos(_fvector vPos);
	void Set_Boosting() { m_IsBoosting = true; }
	void Set_IsActivate(_bool IsActivate) { m_IsActivate = IsActivate; }
	void Set_WorldMatrix(_fmatrix WorldMatrix);

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
	void	Check_TargetMatrix();
	_matrix	Rotate_WorldMatrix(_fmatrix WorldMatrix, _float fDegree, CTransform::STATE eState);

private:
	_double m_dControlTime = 0.0; //
	_double m_dActivateTime = 0.0;
	_bool m_IsActivate = true;
	_bool m_IsBoosting = false;
	_float m_fBoostTime = 0.f;

private:
	CVIBuffer_PointInstance_Custom_STT* m_pPointInstanceCom_STT = nullptr;
	VTXMATRIX_CUSTOM_STT*				m_pInstanceBuffer_STT = nullptr;
	CTextures*							m_pTexturesCom_Distortion = nullptr;
	_double*							m_pInstance_Update_TextureUV_Time = nullptr;

	_float m_fNextUV = 0.f;

	const _float  m_fAlphaTime_Power = 0.5f;
	const _float  m_fSize_Power = 2.f;
	const _float  m_fInstance_SpeedPerSec = 0.5f;
	const _double m_dInstance_Pos_Update_Time = 0.75;
	const _float2 m_vDefaultSize = { 0.8f, 1.6f };
	const _float2 m_vBoostingSize = { 2.f, 6.f };

public:
	static CEffect_EndingRocket_Smoke* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END