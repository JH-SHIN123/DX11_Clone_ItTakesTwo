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

class CRotatedNoBatterySign final : public CRotatedRobotParts
{
private:
	explicit CRotatedNoBatterySign(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRotatedNoBatterySign(const CRotatedRobotParts& rhs);
	virtual ~CRotatedNoBatterySign() = default;

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

public: /* Setter */
	void Set_BatteryCharged(_bool _bBatteryCharged) { m_bBatteryCharged = _bBatteryCharged; }
	void Set_HitLever(_bool _bHitLever) { m_bHitLever = _bHitLever; }

private:
	_bool				m_bBatteryCharged = false;
	_bool				m_bHitLever = false;
	_float				m_fRotateTime = 0.f;
	RTROBOTDESC			m_tRtRotatedRobotPartsDesc;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CStaticActor*		m_pStaticActorCom = nullptr;

public:
	static CRotatedNoBatterySign* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END