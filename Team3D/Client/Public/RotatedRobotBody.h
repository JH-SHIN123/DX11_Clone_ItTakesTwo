#pragma once

#include "Client_Defines.h"
#include "RotatedRobotParts.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CStaticActor;
END

BEGIN(Client)

class CRotatedRobotBody final : public CRotatedRobotParts
{
private:
	explicit CRotatedRobotBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRotatedRobotBody(const CRotatedRobotParts& rhs);
	virtual ~CRotatedRobotBody() = default;

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

public:
	HRESULT Set_MeshRenderGroup();
	HRESULT Add_GameObject_ToRenderGroup();

private:
	RTROBOTDESC	m_tRtRobotPartsDesc;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CStaticActor*		m_pStaticActorCom = nullptr;

public:
	static CRotatedRobotBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END