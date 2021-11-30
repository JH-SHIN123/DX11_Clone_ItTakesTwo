#pragma once

#include "Client_Defines.h"
#include "RotatedRobotParts.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CStaticActor;
class CTriggerActor;
END

BEGIN(Client)

class CRotatedRobotBattery final : public CRotatedRobotParts
{
private:
	explicit CRotatedRobotBattery(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRotatedRobotBattery(const CRotatedRobotParts& rhs);
	virtual ~CRotatedRobotBattery() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual HRESULT Render_ShadowDepth() override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public: /* Getter */
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	void Push_Battery(_double dTimeDelta);

private:
	_bool		m_bRotate = false;
	_bool		m_IsCollide = false;
	_bool		m_bUpdate = true;
	_float		m_fRotateDelay = 0.f;
	RTROBOTDESC	m_tRtRobotPartsDesc;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CStaticActor*		m_pStaticActorCom = nullptr;
	CTriggerActor*		m_pTriggerCom = nullptr;

public:
	static CRotatedRobotBattery* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END