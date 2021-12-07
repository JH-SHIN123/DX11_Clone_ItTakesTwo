#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CPipeJumpWall final : public CGameObject
{
private:
	explicit CPipeJumpWall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPipeJumpWall(const CPipeJumpWall& rhs);
	virtual ~CPipeJumpWall() = default;

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
	_bool				m_IsCollide = false;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTriggerActor*		m_pTriggerCom = nullptr;
	CStaticActor*		m_pStaticActorCom = nullptr;
	CTransform*			m_pPhysxTransformCom = nullptr;

public:
	static CPipeJumpWall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END