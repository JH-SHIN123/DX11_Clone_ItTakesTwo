#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CDynamicActor;
END

BEGIN(Client)
class CMoonBaboonCore_Button final : public CGameObject
{
private:
	explicit CMoonBaboonCore_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMoonBaboonCore_Button(const CMoonBaboonCore_Button& rhs);
	virtual ~CMoonBaboonCore_Button() = default;

public:
	void Set_WorldMatrix();

public:
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject);

private:
	void OnPressed(_double TimeDelta);

private:
	_bool m_bActiveCore = false;
	_bool m_bPressed = false;
	_bool m_bReleased = false;

	_float m_fMoveDeltaValue = 0.f;

	_float3 m_vOffsetPosition = { 0.f, -0.2f, 0.f };
	_float3 m_vLocalPosition = { 0.f,0.f,0.f };

private:
	class CMoonBaboonCore*	m_pParent = nullptr;
	CTransform*				m_pParentTransform = nullptr;

private: /* For.Component */
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CModel*			m_pModelCom = nullptr;
	CStaticActor*	m_pStaticActorCom = nullptr;
	CTriggerActor*	m_pTriggerActorCom = nullptr;

public:
	static CMoonBaboonCore_Button* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;

};
END
