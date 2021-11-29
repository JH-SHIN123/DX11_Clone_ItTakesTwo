#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CBetteryBox : public CGameObject
{
protected:
	explicit CBetteryBox(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBetteryBox(const CBetteryBox& rhs);
	virtual ~CBetteryBox() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	virtual HRESULT Render_ShadowDepth() override;


public:
	CTransform* Get_Transform() { return m_pTransformCom; }

protected:
	/* For.Component */
	CRenderer*									m_pRendererCom = nullptr;
	CTransform*									m_pTransformCom = nullptr;
	CModel*										m_pModelCom = nullptr;
	CStaticActor*								m_pStaticActorCom = nullptr;


public:
	static CBetteryBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END