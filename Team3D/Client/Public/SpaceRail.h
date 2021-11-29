#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CTriggerActor;
END

BEGIN(Client)
class CSpaceRail : public CGameObject
{
public:

private:
	explicit CSpaceRail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSpaceRail(const CSpaceRail& rhs) = delete;
	virtual ~CSpaceRail() = default;

public:
	_fmatrix Get_WorldMatrix() const;
	_fvector Get_Position() const;

public:
	virtual HRESULT	NativeConstruct(void* pArg) override;

public:
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject);

private:
	/* For.Component */
	CTransform* m_pTransformCom = nullptr;
	CTriggerActor* m_pTriggerActorCom = nullptr;

public:
	static CSpaceRail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone_GameObject(void* pArg) override { return nullptr; };
	virtual void Free() override;
};
END
