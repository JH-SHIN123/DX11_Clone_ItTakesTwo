#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CTriggerActor;
END

BEGIN(Client)

class CTutorialDoor : public CGameObject
{
protected:
	explicit CTutorialDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTutorialDoor(const CTutorialDoor& rhs);
	virtual ~CTutorialDoor() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;


	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

public:
	void Set_Pull(_bool _bPull) { m_bPull = _bPull; }
	void Pull_TutorialDoor(_double dTimeDelta);

private:
	_float		m_fUpAcceleration = 0.f;
	_float		m_fMoveDist = 0.f;
	_bool		m_bPull = false;
	_bool		m_IsCollide = false;
	_bool		m_IsDelete_UI = false;

private:
	_bool m_IsNoGrab		= false;
	_bool m_IsPullMax		= false;
	_bool m_IsPullMax_Once	= false;

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTriggerActor*		m_pTriggerCom = nullptr;
	CStaticActor*		m_pStaticActorCom = nullptr;

	CTransform*			m_pTransformCom_Trigger = nullptr;


private:
	class CEffect_FireDoor* m_pEffectFireDoor = nullptr;

private:
	HRESULT InterActive_UI();

public:
	static CTutorialDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END