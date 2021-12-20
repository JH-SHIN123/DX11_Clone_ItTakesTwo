#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CGravityPath final : public CGameObject
{
private:
	explicit CGravityPath(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGravityPath(const CGravityPath& rhs);
	virtual	~CGravityPath() = default;

public:/* Struct */
	typedef struct tagStatic_Env_Dsec
	{
		_float4x4		WorldMatrix;
		_tchar			szModelTag[MAX_PATH];
		_uint			iMaterialIndex;
		_float			fCullRadius;
		GameID::Enum	eGameID;
	}STATIC_ENV_DESC;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eRender) override;
	virtual HRESULT Render_ShadowDepth() override;

private:
	/* For.Component */
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	class CSepdStaticActor*	m_pStaticActorCom = nullptr;
	STATIC_ENV_DESC			m_Static_Env_Desc;
	USERDATA*				m_arrUserData = nullptr;
	_float					m_fDrawOption = 0.f;
	_double					m_dOptionChangeTerm = 0.0;

public:
	static CGravityPath* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END