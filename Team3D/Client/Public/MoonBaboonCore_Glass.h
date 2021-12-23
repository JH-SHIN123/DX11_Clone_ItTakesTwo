#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CStaticActor;
END

BEGIN(Client)
class CMoonBaboonCore_Glass final : public CGameObject
{
private:
	explicit CMoonBaboonCore_Glass(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMoonBaboonCore_Glass(const CMoonBaboonCore_Glass& rhs);
	virtual ~CMoonBaboonCore_Glass() = default;

public:
	void Set_Broken();
	void Set_WorldMatrix();

public:
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

private:
	void OnTrigger(_double TimeDelta);

private:
	_bool m_bBroken = false;

private:
	_float3 m_vOffsetPos = { 0.f, -4.f, 0.f };
	_float3 m_vLocalPos = { 0.f,0.f,0.f };

private:
	class CMoonBaboonCore* m_pParent = nullptr;
	CTransform* m_pParentTransform = nullptr;

private: /* For.Component */
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	
	CModel* m_pModelCom = nullptr;
	CModel* m_pModelCom_Glass = nullptr;
	CModel* m_pModelCom_GlassBroken = nullptr;

	CModel* m_pModelCom_Pillar = nullptr;

	CStaticActor*	m_pStaticActorCom = nullptr;

public:
	static CMoonBaboonCore_Glass* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END

