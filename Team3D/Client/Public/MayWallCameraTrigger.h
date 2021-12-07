#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CTriggerActor;
END

BEGIN(Client)
class CMayWallCameraTrigger : public CGameObject
{

private:
	explicit CMayWallCameraTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMayWallCameraTrigger(const CMayWallCameraTrigger& rhs);
	virtual ~CMayWallCameraTrigger() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject);

private:
	/* For.Component */
	CTransform*		m_pTransformCom = nullptr;
	CTriggerActor*	m_pTriggerActorCom = nullptr;

private:
	_bool			m_IsCollide = false;


public:
	static CMayWallCameraTrigger* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END
