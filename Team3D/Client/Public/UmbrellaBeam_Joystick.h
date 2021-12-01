#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CUmbrellaBeam_Joystick : public CGameObject
{
protected:
	explicit CUmbrellaBeam_Joystick(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUmbrellaBeam_Joystick(const CUmbrellaBeam_Joystick& rhs);
	virtual ~CUmbrellaBeam_Joystick() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	void Set_ControlActivate();
	void Set_HorizontalAngle(_float fAngle);
	void Set_Rotate(_float fAngle);
	void Set_OnParentRotate(_matrix ParentMatrix);

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	_float										m_fHorizontalAngle = 0.f;
	_float4x4									m_matParent;
	_bool										m_IsControlActivate = false;
	_bool										m_IsCollision = false;

private:
	class CUmbrellaBeam*						m_pUmbrellaBeam = nullptr;

protected:
	/* For.Component */
	CRenderer*									m_pRendererCom = nullptr;
	CTransform*									m_pTransformCom = nullptr;
	CModel*										m_pModelCom = nullptr;
	CStaticActor*								m_pStaticActorCom = nullptr;
	CTriggerActor*								m_pTriggerCom = nullptr;
	CTransform*									m_pTriggerTransformCom = nullptr;

private:
	HRESULT Ready_Layer_UmbrellaBeam(const _tchar * pLayerTag);

public:
	static CUmbrellaBeam_Joystick* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END