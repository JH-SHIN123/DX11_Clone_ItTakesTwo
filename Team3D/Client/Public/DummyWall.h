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

class CDummyWall final : public CGameObject
{
private:
	explicit CDummyWall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CDummyWall(const CDummyWall& rhs);
	virtual ~CDummyWall() = default;

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
	_bool		m_IsCollide = false;
	GameID::Enum		m_PlayerID = GameID::eDUMMYWALL;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTriggerActor*		m_pTriggerCom = nullptr;
	CStaticActor*		m_pStaticActorCom = nullptr;
	CTransform*			m_pPhysxTransformCom = nullptr;

public:
	static CDummyWall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END