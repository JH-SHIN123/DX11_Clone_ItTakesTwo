#pragma once

#ifndef __SPACE_VALVE_STAR_H__

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CSpace_Valve_Star final : public CGameObject
{
private:
	explicit CSpace_Valve_Star(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSpace_Valve_Star(const CSpace_Valve_Star& rhs);
	virtual ~CSpace_Valve_Star() = default;

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

private:
	HRESULT Ready_Component();

private:
	const _int m_iModelInstance_Count = 7;

private:
	CRenderer*			m_pRendererCom	= nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel_Instance*	m_pModelCom		= nullptr;

public:
	static CSpace_Valve_Star* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __SPACE_VALVE_STAR_H__
#endif
