#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CPressureBigPlate : public CGameObject
{
protected:
	explicit CPressureBigPlate(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPressureBigPlate(const CPressureBigPlate& rhs);
	virtual ~CPressureBigPlate() = default;

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
	void Set_Effect(_bool IsEffect) { m_IsEffect_Done = IsEffect; }

public:
	_bool Get_PowerSupplyActive() { return m_IsPowerSupplyActive; }
	_bool Get_PowerSupplyAvailable() { return m_IsPowerSupplyAvailable;  }
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	_bool										m_IsButtonActive = false;
	_bool										m_IsPipeCurveRotate = false;
	_bool										m_IsCollision = false;
	_bool										m_IsReset = false;
	_bool										m_IsPowerButtonUp = true;
	_bool										m_IsPowerSupplyAvailable = false;
	_bool										m_IsPowerSupplyActive = false;
	_bool										m_IsBatteryCheck = false;
	_bool										m_IsButtonDown = false;
	_bool										m_IsDoorOpen = false;
	_bool										m_IsEffect_Once = false; // Effect
	_bool										m_IsEffect_Done = false; // Effect
	_float										m_fMove = 0.f;
	_float										m_fActiveMove = 0.f;
	_float										m_fWaitingTime = 0.f;
	_float										m_fOpenWaitTime = 0.f;
	_uint										m_iOption = 0;


private:
	class CPressurePlateLock*					m_pPlateLock = nullptr;
	class CPressurePlateFrame*					m_pPlateFrame = nullptr;
	class CBatteryBox*							m_pBatteryBox = nullptr;
	vector<class CPressurePlate*>				m_vecPressurePlate;


protected:
	/* For.Component */
	CRenderer*									m_pRendererCom = nullptr;
	CTransform*									m_pTransformCom = nullptr;
	CModel*										m_pModelCom = nullptr;
	CStaticActor*								m_pStaticActorCom = nullptr;
	CTriggerActor*								m_pTriggerCom = nullptr;
	
private:
	void SetUp_DefaultPositionSetting();
	void RotationButton_Active(_double TimeDelta);
	void Check_Collision_PlayerAnim();
	void PowerConnectionButton_Active(_double TimeDelta);

private:
	HRESULT Ready_Layer_Plate(const _tchar * pLayerTag, _uint iCount);
	HRESULT Ready_Layer_PlateLock(const _tchar * pLayerTag);
	HRESULT Ready_Layer_BatteryBox(const _tchar * pLayerTag);
	HRESULT Ready_Layer_PlateFrame(const _tchar * pLayerTag);

public:
	static CPressureBigPlate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END