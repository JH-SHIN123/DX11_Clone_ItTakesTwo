#pragma once

#ifndef __WALLLASERTRAP_BUTTON_H__

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)
class CWallLaserTrap_Button final : public CGameObject
{
private:
	explicit CWallLaserTrap_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CWallLaserTrap_Button(const CWallLaserTrap_Button& rhs);
	virtual ~CWallLaserTrap_Button() = default;

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
	_bool m_IsActivate = true;


private:
	/* For.Component */
	CRenderer*			m_pRendererCom		= nullptr;
	CTransform*			m_pTransformCom		= nullptr;
	CModel*				m_pModelCom			= nullptr;
	CStaticActor*		m_pStaticActorCom	= nullptr;
	CTriggerActor*		m_pTriggerCom		= nullptr;

private:
	class CWallLaserTrap* m_pLaserTrap_Left		= nullptr;
	class CWallLaserTrap* m_pLaserTrap_Right	= nullptr;

public:
	static CWallLaserTrap_Button* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __WALLLASERTRAP_BUTTON_H__
#endif // !__WALLLASERTRAP_BUTTON_H__
