#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CTriggerActor;
END

BEGIN(Client)

class CTestObject01 final : public CGameObject
{
private:
	explicit CTestObject01(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTestObject01(const CTestObject01& rhs);
	virtual ~CTestObject01() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;


public:
	 CTransform* Get_Transform() { return m_pTransformCom; }
private:

	/* For.Component */
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CModel*			m_pModelCom = nullptr;
	CTriggerActor*	m_pTriggerCom = nullptr;

public:
	static CTestObject01* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END