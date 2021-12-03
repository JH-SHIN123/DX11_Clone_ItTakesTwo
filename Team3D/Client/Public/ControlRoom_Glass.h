#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CControlRoom_Glass : public CGameObject
{
protected:
	explicit CControlRoom_Glass(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CControlRoom_Glass(const CControlRoom_Glass& rhs);
	virtual ~CControlRoom_Glass() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

protected:
	/* For.Component */
	CRenderer*									m_pRendererCom = nullptr;
	CTransform*									m_pTransformCom = nullptr;
	CModel*										m_pModelCom = nullptr;
	CStaticActor*								m_pStaticActorCom = nullptr;

public:
	static CControlRoom_Glass* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END