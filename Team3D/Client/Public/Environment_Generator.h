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
	HRESULT Load_Model_Others(_tchar* pFilePath);
	HRESULT Load_Model_Instancing();
	HRESULT Load_Environment_Model_Prototype();
	HRESULT Load_Environment_GameObject_Prototype();

	HRESULT Load_Environment_Space();

public:
	HRESULT NativeConstruct_Environment_Generator(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;
	CGameInstance*				m_pGameInstance = nullptr;

private:
	CGameObject* Create_Class(_tchar* pPrototypeTag, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

public:
	virtual void Free() override;
};
END