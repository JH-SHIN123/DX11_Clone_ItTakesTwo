#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CStaticActor;
class CTriggerActor;
END

BEGIN(Client)
class CToyBoxButton_Button : public CGameObject
{
protected:
	explicit CToyBoxButton_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CToyBoxButton_Button(const CToyBoxButton_Button& rhs);
	virtual ~CToyBoxButton_Button() = default;

public:
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

protected:
	/* For.Component */
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CStaticActor* m_pStaticActorCom = nullptr;
	CTriggerActor* m_pTriggerActorCom = nullptr;

private:
	class CToyBoxButton* m_pParent = nullptr;

public:
	static CToyBoxButton_Button* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone_GameObject(void* pArg) override { return this; };
	virtual void Free() override;
};
END
