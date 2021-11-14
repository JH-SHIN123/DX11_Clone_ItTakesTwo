#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "GameInstance.h"

BEGIN(Engine)
class CRenderer;
class CModel_Instance;
END

BEGIN(Client)

class CInstancing_Env final : public CGameObject
{
private:
	explicit CInstancing_Env(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CInstancing_Env(const CInstancing_Env& rhs);
	virtual ~CInstancing_Env() = default;

public:/* Struct */
	typedef struct tagInstancing_Env_Desc
	{
		CModel_Instance::ARG_DESC	Instancing_Arg;
		_tchar						szModelTag[MAX_PATH];
		_uint						ShaderPass;
		_uint						RenderGroup;
		_uint						iMaterialIndex;
	}INS_ENV_DESC;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

private:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CModel_Instance*	m_pModelCom = nullptr;

	INS_ENV_DESC		m_Ins_Env_Desc;

public:
	static CInstancing_Env* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END

