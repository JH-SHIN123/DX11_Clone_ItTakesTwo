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
class CMoonBaboonCore_Shield final : public CGameObject
{
private:
	explicit CMoonBaboonCore_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMoonBaboonCore_Shield(const CMoonBaboonCore_Shield& rhs);
	virtual ~CMoonBaboonCore_Shield() = default;

public:
	void Set_WorldMatrix();

public:
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

private:
	void OnTrigger(_double TimeDelta);

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
	CStaticActor* m_pStaticActorCom = nullptr;

public:
	static CMoonBaboonCore_Shield* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END
