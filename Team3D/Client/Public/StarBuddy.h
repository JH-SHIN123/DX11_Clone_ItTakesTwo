#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CStarBuddy : public CGameObject
{
protected:
	explicit CStarBuddy(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStarBuddy(const CStarBuddy& rhs);
	virtual ~CStarBuddy() = default;

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

public:
	void Launch_StarBuddy(_double TimeDelta);

private:
	_float		m_fUpAcceleration = 0.f;
	_bool		m_bLaunch = false;
	_float		m_fLifeTime = 0.f;

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;

private:
	HRESULT InterActive_UI();

public:
	static CStarBuddy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END