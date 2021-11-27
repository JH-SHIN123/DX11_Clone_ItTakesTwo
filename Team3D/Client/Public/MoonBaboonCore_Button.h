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
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

private: /* For.Component */
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CDynamicActor* m_pStaticActorCom = nullptr;

public:
	static CMoonBaboonCore_Button* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;

};
END
