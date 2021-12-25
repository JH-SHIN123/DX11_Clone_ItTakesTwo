#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CTriggerActor;
class CStaticActor;
END

BEGIN(Client)

class CStarBuddy : public CGameObject
{
private:
	explicit CStarBuddy(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStarBuddy(const CStarBuddy& rhs);
	virtual ~CStarBuddy() = default;

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

private:
	void Launch_StarBuddy(_double dTimeDelta);

private:
	_float		m_fUpAcceleration = 0.f;
	_bool		m_bLaunch = false;
	_float		m_fLifeTime = 0.f;
	_bool		m_IsCodyCollide = false;
	_bool		m_IsMayCollide = false;
	_bool		m_bSaveOnce = false;

	GameID::Enum		m_PlayerID = GameID::eSTARBUDDY;
	UI::INTERACTIVE_ID  m_eInterActiveID = UI::INTERACTIVE_ID_END;

	_vector		m_vSavedPlayerPos = {};
	_vector		m_vMoveDirection = {};

	/* Sound */
	_float m_fStarBuddy_Volume = 1.f;
	_bool  m_bSoundOnce = false;

private:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTriggerActor*		m_pTriggerCom = nullptr;
	CStaticActor*		m_pStaticActorCom = nullptr;

private:
	class CEffect_StarBuddy_Move* m_pMoveEffect = nullptr;

public:
	static CStarBuddy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END