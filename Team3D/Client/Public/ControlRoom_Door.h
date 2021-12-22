#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CControlRoom_Door : public CGameObject
{
protected:
	explicit CControlRoom_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CControlRoom_Door(const CControlRoom_Door& rhs);
	virtual ~CControlRoom_Door() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	void Set_OpenDoor();

public:
	virtual HRESULT Render_ShadowDepth() override;


public:
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	_bool										m_IsOpenDoor = false;
	_bool										m_IsOpenDoorEnd = false;
	_uint										m_iOption = 0;

	_float										m_fTestAngle = 0.f;
	_float										m_fMove = 0.f;

protected:
	/* For.Component */
	CRenderer*									m_pRendererCom = nullptr;
	CTransform*									m_pTransformCom = nullptr;
	CModel*										m_pModelCom = nullptr;
	CStaticActor*								m_pStaticActorCom = nullptr;

private:
	void SetUp_DefaultPositionSetting();

private:
	HRESULT Ready_Layer_PlateLock(const _tchar * pLayerTag, _uint iCount);

public:
	static CControlRoom_Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END