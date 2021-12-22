#pragma once

#include "Client_Defines.h"
#include "RotatedRobotParts.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CRotatedRobotHead final : public CRotatedRobotParts
{
private:
	explicit CRotatedRobotHead(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRotatedRobotHead(const CRotatedRobotParts& rhs);
	virtual ~CRotatedRobotHead() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual HRESULT Render_ShadowDepth() override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public: /* Getter */
	CTransform* Get_Transform() { return m_pTransformCom; }

public: /* Setter */
	void Set_Battery_Charged(_bool _bBatteryCharged) { m_bBatteryCharged = _bBatteryCharged; }
	void Set_Lever_Active(_bool _bLeverActive) { m_bLeverActive = _bLeverActive; }
	void Set_Lever_Hit_When_NoBattery(_bool _bHitLeverInActive) { m_bHitLeverInActive = _bHitLeverInActive; }

private:
	void Hit_Lever_InActive(_double dTimeDelta);

private:
	_bool		m_bBatteryCharged = false;
	_bool		m_bLeverActive = false;
	_bool		m_bHitLeverInActive = false;
	RTROBOTDESC	m_tRtRobotPartsDesc;

	/* Sound */
	_float m_fHeadBanging_Volume = 1.f;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;

public:
	static CRotatedRobotHead* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END