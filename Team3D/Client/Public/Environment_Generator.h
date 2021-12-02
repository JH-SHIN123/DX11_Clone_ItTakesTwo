#pragma once

#include "Client_Defines.h"
#include "Instancing_Env.h"
#include "Static_Env.h"

BEGIN(Client)
class CEnvironment_Generator final : public CBase
{
	DECLARE_SINGLETON(CEnvironment_Generator)

private:
	explicit CEnvironment_Generator();
	virtual ~CEnvironment_Generator() = default;

public:
	/* 프로토타입 생성 */
	/* TXT 파일 로드 */
	HRESULT Load_Prototype_Model_Others_TXT(_tchar* pFilePath);
	HRESULT Load_Prototype_Model_Instancing_TXT();
	HRESULT Load_Prototype_GameObject_TXT();
	/* DAT 파일 로드 */
	HRESULT Load_Prototype_Model_Others(_tchar* pFilePath);
	HRESULT Load_Prototype_Model_Instancing();
	HRESULT Load_Prototype_GameObject();

	/* 클론 로드 */
	HRESULT Load_Stage_Space();
	
public:
	HRESULT NativeConstruct_Environment_Generator(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;
	CGameInstance*				m_pGameInstance = nullptr;

private:
	CGameObject* Create_Class(_tchar* pPrototypeTag, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	void Set_Info_Model(CStatic_Env::ARG_DESC& tInfo);
	void Adjustment_Model_Position(_tchar* pModelTag, _float4x4& rWorld);

private:
	HRESULT Load_Default_Prototype_GameObject();
	HRESULT Load_Environment_Space();
	HRESULT Load_Environment_Space_Boss();
	HRESULT Load_Environment_Interactive_Instancing();
	HRESULT Load_Environment_SpaceRail();
	HRESULT Load_Environment_Trigger();

public:
	virtual void Free() override;
};
END