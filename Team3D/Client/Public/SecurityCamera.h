#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CSecurityCamera final : public CGameObject
{
protected:
	explicit CSecurityCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSecurityCamera(const CSecurityCamera& rhs);
	virtual ~CSecurityCamera() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render() override;

public:
	virtual HRESULT Render_ShadowDepth() override;
	
	/* Getter */
public:		
	CTransform* Get_Transform() { return m_pTransformCom; }


private:
	void Find_Target(_double dTimeDelta);
	void Watch_Target(_double dTimeDelta);

private:
	_bool m_bIsTargetNear = false;

private:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;

	//Target
	CGameObject* m_pTargetObj = nullptr;

public:
	static CSecurityCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END