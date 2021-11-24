#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Engine)

class CSky final : public CGameObject
{
private:
	explicit CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSky(const CSky& rhs);
	virtual ~CSky() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

private:
	/* For.Component */
	CTransform*	m_pTransformCom = nullptr;
	CRenderer*	m_pRendererCom = nullptr;
	CModel*		m_pModelCom = nullptr;
	_uint		m_iViewportIndex = 0;

public:
	static CSky* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END