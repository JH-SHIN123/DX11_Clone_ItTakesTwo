#pragma once

#ifndef __WARPGATE_H__

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CWarpGate final : public CGameObject
{
public:
	enum STAGE_VALUE
	{
		MAIN_UMBRELLA = 0,	STAGE_UMBRELLA,
		MAIN_PLANET,		STAGE_PLANET, 
		MAIN_TENNIS,		STAGE_TENNIS,
		STAGE_END
	};

public:
	typedef struct tagWarpGate_Desc
	{
		STAGE_VALUE eStageValue;
		_double		dClearEffect_Term;

		tagWarpGate_Desc() {}
		tagWarpGate_Desc(STAGE_VALUE eValue, _double dClearEffect_Term)
			: eStageValue(eValue), dClearEffect_Term(dClearEffect_Term){}
	}WARPGATE_DESC;

private:
	explicit CWarpGate(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CWarpGate(const CWarpGate& rhs);
	virtual ~CWarpGate() = default;

public:
	_uint Get_StageValue() { return (_uint)m_WarpGate_Desc.eStageValue; }

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
	_fmatrix Get_NextPortal_Matrix();

private:
	HRESULT Ready_Component();
	void	Check_Stage_Value();
	HRESULT	Check_WarpGate_Star();
	void	Check_ClearEffect(_double TimeDelta);
	void	Check_StageClear();
	void	Check_Tennis_Found();
	void	Check_Sound();

private:
	WARPGATE_DESC m_WarpGate_Desc;
	_float4		m_vWarpPos						= { 0.f, 0.f, 0.f, 1.f };
	_double		m_dActivate_WarpGate_Star_Time	= 0.f;
	_int		m_iWarpGate_Star_Count			= 5;

	_bool		m_bClearEffect		= false;
	_double		m_dClearEffect_Time = 0.0;
	_int		m_iClearEffect_Count = 0;

	_double		m_dActivateTime	 = 0.0;

private:
	/* For.Component */
	CRenderer*			m_pRendererCom			= nullptr;
	CTransform*			m_pTransformCom			= nullptr;
	CModel*				m_pModelCom				= nullptr;
	CTriggerActor*		m_pTriggerCom			= nullptr;
	CStaticActor*		m_pStaticActorCom		= nullptr;
	CTransform*			m_pTransformCom_Trigger = nullptr;
	CTriggerActor*		m_pTriggerCom_Tennis	= nullptr;

private:
	class CEffect_RespawnTunnel*		m_pRespawnTunnel		= nullptr;
	class CEffect_RespawnTunnel_Portal* m_pRespawnTunnel_Portal = nullptr;
	class CWarpGate_Star*				m_pWarpGate_Star_1		= nullptr;
	class CWarpGate_Star*				m_pWarpGate_Star_2		= nullptr;
	class CWarpGate_Star*				m_pWarpGate_Star_3		= nullptr;
	class CWarpGate_Star*				m_pWarpGate_Star_4		= nullptr;
	class CWarpGate_Star*				m_pWarpGate_Star_5		= nullptr;

	const _float4	m_vWormhole_Cody	= {};
	const _float4	m_vWormhole_May		= {};


public:
	static CWarpGate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __WARPGATE_H__
#endif
