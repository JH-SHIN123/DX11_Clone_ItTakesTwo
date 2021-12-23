#pragma once

#ifndef __WALLLASERTRAP_H__

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CWallLaserTrap final : public CGameObject
{
public:
	struct tagWallLaserTrap_Desc
	{
		_float4 vPosition;
		_float3 vRotateAngle;

		tagWallLaserTrap_Desc() {}
		tagWallLaserTrap_Desc(_float4 vPos, _float3 vAngle)
			: vPosition(vPos), vRotateAngle(vAngle) {}
	};

private:
	explicit CWallLaserTrap(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CWallLaserTrap(const CWallLaserTrap& rhs);
	virtual ~CWallLaserTrap() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

	void Set_TriggerActivate(_bool IsActivate);

public:
	virtual HRESULT Render_ShadowDepth() override;
	void Set_Transform(_vector vPos);
	void Set_Rotate(_float3 vAngle);

private:
	HRESULT Ready_Component(void * pArg);
	HRESULT Ready_PointInstance();
	void	Calcul_Transform(tagWallLaserTrap_Desc Data);
	_fmatrix Get_Matrix(_float4 vPos);
	void	Render_PointInstance();
	void	Check_Activate(_double TimeDelta);

private:
	_bool	m_IsActivate	= true;
	_float	m_fActivateTime = 1.f;

private:
	/* For.Component */
	CRenderer*					m_pRendererCom			= nullptr;
	CTransform*					m_pTransformCom			= nullptr;
	CModel*						m_pModelInstanceCom		= nullptr;
	CTriggerActor*				m_pTriggerCom			= nullptr;
	CTransform*					m_pTransformCom_Trigger = nullptr;

	VTXMATRIX_CUSTOM_ST*				m_pPointInstanceBuffer	= nullptr;
	CVIBuffer_PointInstance_Custom_ST*	m_pPointInstanceCom		= nullptr;
	CTextures*							m_pTexturesCom_Mask		= nullptr;
	const _int							m_iPointInstance_Count = 4;

	const _float2		m_vHalfSize = { 1.8f, 5.f };
	_float4				m_vPosition_Lamp[4];

public:
	static CWallLaserTrap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __WALLLASERTRAP_H__
#endif
