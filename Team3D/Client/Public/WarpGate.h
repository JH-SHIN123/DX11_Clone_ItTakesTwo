#pragma once

#ifndef __WARPGATE_H__

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CTriggerActor;
class CStaticActor;
END

BEGIN(Client)
class CWarpGate final : public CGameObject
{
public:
	enum STAGE_VALUE
	{
		MAIN_UMBRELLA,	STAGE_UMBRELLA,
		MAIN_PLANET,	STAGE_PLANET, 
		STAGE_END
	};

public:
	typedef struct tagWarpGate_Desc
	{
		STAGE_VALUE eStageValue;

		tagWarpGate_Desc(STAGE_VALUE eValue)
			: eStageValue(eValue){}
		

	}WARPGATE_DESC;

private:
	explicit CWarpGate(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CWarpGate(const CWarpGate& rhs);
	virtual ~CWarpGate() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	HRESULT Ready_Component();
	void	Check_Stage_Value();
	_fmatrix Get_NextPortal_Matrix(STAGE_VALUE eValue);

private:
	STAGE_VALUE m_eStageValue	= STAGE_END;
	_float4		m_vWarpPos		= { 0.f, 0.f, 0.f, 1.f };

private:
	/* For.Component */
	CRenderer*			m_pRendererCom			= nullptr;
	CTransform*			m_pTransformCom			= nullptr;
	CModel*				m_pModelCom				= nullptr;
	CTriggerActor*		m_pTriggerCom			= nullptr;
	CStaticActor*		m_pStaticActorCom		= nullptr;
	CTransform*			m_pTransformCom_Trigger = nullptr;

private:
	class CEffect_RespawnTunnel*		m_pRespawnTunnel		= nullptr;
	class CEffect_RespawnTunnel_Portal* m_pRespawnTunnel_Portal = nullptr;

	const _float4	m_vWormhole_Cody		= {};
	const _float4	m_vWormhole_May			= {};

public:
	static CWarpGate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __WARPGATE_H__
#endif
