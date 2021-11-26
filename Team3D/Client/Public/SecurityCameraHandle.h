#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CSecurityCameraHandle final : public CGameObject
{
protected:
	explicit CSecurityCameraHandle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSecurityCameraHandle(const CSecurityCameraHandle& rhs);
	virtual ~CSecurityCameraHandle() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;

public:
	static CSecurityCameraHandle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END