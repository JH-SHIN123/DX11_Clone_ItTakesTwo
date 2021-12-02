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

class CBigButton final : public CGameObject
{
private:
	explicit CBigButton(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBigButton(const CBigButton& rhs);
	virtual ~CBigButton() = default;

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
	void Set_Press(_bool bPressed) { m_bPressed = bPressed; }

private:
	void Check_Collision_PlayerAnim();

private:
	_float		m_fMoveDist = 0.f;
	_bool		m_bPressed = false;
	_bool		m_IsCollide = false;
	_bool		m_bUpdate = true;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CStaticActor*		m_pStaticActorCom = nullptr;
	CTriggerActor*		m_pTriggerCom = nullptr;

public:
	static CBigButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END