#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CControlRoom_Battery : public CGameObject
{
protected:
	explicit CControlRoom_Battery(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CControlRoom_Battery(const CControlRoom_Battery& rhs);
	virtual ~CControlRoom_Battery() = default;

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
	_bool Get_BatteryHolding() { return m_IsBatteryHolding; }

private:
	_float										m_fAngle = 0.f;
	_float										m_fRotate = 0.f;
	_float4										m_vCodyPos;
	_bool										m_IsCameRange = false;
	_bool										m_IsCollision = false;
	_bool										m_IsPlayerInterActive = false;
	_bool										m_IsBatteryHolding = false;

private:
	vector<class CControlRoom_Door*>			m_vecDoor;

protected:
	/* For.Component */
	CRenderer*									m_pRendererCom = nullptr;
	CTransform*									m_pTransformCom = nullptr;
	CModel*										m_pModelCom = nullptr;
	CStaticActor*								m_pStaticActorCom = nullptr;
	CTriggerActor*								m_pTriggerCom = nullptr;
	CTransform*									m_pTriggerTransform = nullptr;

private:
	_int InterActive_Battery(_double TimeDelta);

private:
	HRESULT Ready_Layer_Door(const _tchar * pLayerTag, _uint iCount);
	HRESULT InterActive_UI();

public:
	static CControlRoom_Battery* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END