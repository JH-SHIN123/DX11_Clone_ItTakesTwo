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
	HRESULT Load_Environment_GameObject_Prototype();

	/* 프로토타입 생성 */
	/* TXT 파일 로드 */
	HRESULT Load_Model_Data_TXT(_tchar* pFilePath);
	HRESULT Load_Model_Instancing_TXT();
	/* DAT 파일 로드 */
	HRESULT Load_Model_Others(_tchar* pFilePath);
	HRESULT Load_Model_Instancing();

	/* 클론 로드 */
	HRESULT Load_Environment_Space();
	HRESULT Load_Environment_Space_Boss();
	HRESULT Load_Environment_Interactive_Instancing();

	HRESULT Load_Environment_Model_Prototype();

public:
	HRESULT NativeConstruct_Environment_Generator(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;
	CGameInstance*				m_pGameInstance = nullptr;

private:
	CGameObject* Create_Class(_tchar* pPrototypeTag, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	void Set_Info_Model(CStatic_Env::STATIC_ENV_DESC& tInfo);

public:
	virtual void Free() override;
};
END