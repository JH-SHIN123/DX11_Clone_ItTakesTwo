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
class CMoonBaboonCore_Pillar final : public CGameObject
{
private:
	explicit CMoonBaboonCore_Pillar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMoonBaboonCore_Pillar(const CMoonBaboonCore_Pillar& rhs);
	virtual ~CMoonBaboonCore_Pillar() = default;

public:
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

private:
	class CMoonBaboonCore* m_pMoonBaboonCore = nullptr;

private: /* For.Component */
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CStaticActor* m_pStaticActorCom = nullptr;

public:
	static CMoonBaboonCore_Pillar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;

};
END
