#pragma once

#ifndef __EFFECT_MAY_BOOTS_H__

#include "InGameEffect.h"

BEGIN(Engine)
class CVIBuffer_PointInstance_Custom_STT;
END

BEGIN(Client)
class CEffect_May_Boots final : public CInGameEffect
{
private:
	explicit CEffect_May_Boots(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_May_Boots(const CEffect_May_Boots& rhs);
	virtual ~CEffect_May_Boots() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

public:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;

public:
	void Set_Model(CModel* pModel);				// Initialize
	void Update_Matrix(_fmatrix WorldMatrix);	// Tick
	void Set_IsActivate_GravityBoots();
	void Add_WalkingParticle(_bool IsRightFoot);

private:
	HRESULT Ready_Component(void* pArg);
	HRESULT Ready_Instance();

private:
	void Update_Effect(_double TimeDelta);
	void Update_Particle(_double TimeDelta);
	void Update_Smoke(_double TimeDelta, _int iStartIndex, _int iEndIndex, _float4x4 ParentMatrix4x4);
	_int Respawn_Smoke(_int iIndex);


private:
	_vector Get_RandDir();
	void Set_Shader_Data();
	void Reset_UpdateTime();

private:
	_bool		m_IsActivate	= false;
	_bool		m_IsActivateFirst = false;
	_bool		m_IsRendering	= false;
	CModel*		m_pTargetModel	= nullptr;
	_float4x4	m_BoneMatrix_Left;	// LeftToeBase
	_float4x4	m_BoneMatrix_Right; // RightToeBase

private: // Smoke 0~9Left, 10~18Right
	const _int							m_iInstanceCount_Smoke			= 18;
	const _double						m_dRespawnTime_Smoke			= 1.25;
	const _float2						m_vDefaultSize_Smoke			= { 0.5f, 0.5f };
	const _float						m_fSpeedPerSec_Smoke			= 15.f;
	_float4*							m_pInstance_LocalPos			= nullptr;
	VTXMATRIX_CUSTOM_STT*				m_pInstanceBuffer_Smoke			= nullptr;
	CVIBuffer_PointInstance_Custom_STT* m_pPointInstanceCom_STT			= nullptr;
	

private: // Particle
	const _int	m_iInstanceCount_Particle = 60;


public:
	static CEffect_May_Boots* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_MAY_BOOTS_H__
#endif // !__EFFECT_MAY_BOOTS_H__
