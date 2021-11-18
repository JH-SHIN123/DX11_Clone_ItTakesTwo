#pragma once

#ifndef __TEST_ENVOBJECT_H__

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CModel_Instance;
END

BEGIN(Client)

class CEffect_FireDoor;

class CTest_EnvObject final : public CGameObject
{
private:
	explicit CTest_EnvObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTest_EnvObject(const CTest_EnvObject& rhs);
	virtual ~CTest_EnvObject() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

public:
	virtual HRESULT Render_ShadowDepth() override;

private:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CModel_Instance*	m_pModelCom = nullptr;
	_uint				m_iRenderNum = 0;
	PxRigidDynamic*		m_pTest = nullptr;

public:
	static CTest_EnvObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __TEST_ENVOBJECT_H__
#endif // !__TEST_ENVOBJECT_H__
