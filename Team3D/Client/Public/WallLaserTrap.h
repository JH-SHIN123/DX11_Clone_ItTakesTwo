#pragma once

#ifndef __WALLLASERTRAP_H__

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CWallLaserTrap final : public CGameObject
{
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

public:
	virtual HRESULT Render_ShadowDepth() override;

private:
	HRESULT Ready_Component();
	void	Check_Stage_Value();

private:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTriggerActor*		m_pTriggerCom = nullptr;

public:
	static CWallLaserTrap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __WALLLASERTRAP_H__
#endif
