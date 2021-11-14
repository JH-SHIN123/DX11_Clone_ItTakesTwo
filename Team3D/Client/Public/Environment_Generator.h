#pragma once

#include "Client_Defines.h"
#include "GameInstance.h"
#include "Instancing_Env.h"
#include "Static_Env.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CEnvironment_Generator final : public CBase
{
	DECLARE_SINGLETON(CEnvironment_Generator)

private:
	explicit CEnvironment_Generator();
	virtual ~CEnvironment_Generator() = default;

public:
	HRESULT Load_Environment_Model_Prototype(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	HRESULT Load_Environment_GameObject_Prototype(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	HRESULT Load_Environment_Clone();

private:
	CGameInstance*	m_pGameInstance = nullptr;

private:
	CGameObject* Create_Class(_tchar* pPrototypeTag, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	void Add_Environment_Desc(CInstancing_Env::INS_ENV_DESC& EnvDesc);
	void Add_Environment_Desc(CStatic_Env::STATIC_ENV_DESC& EnvDesc);

public:
	virtual void Free() override;
};

END