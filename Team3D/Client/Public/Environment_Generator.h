#pragma once
#include "GameOption.h"
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
	HRESULT Load_Prototype_Model_Others_Space(_uint iIndex);
	HRESULT Load_Prototype_GameObject_TXT();
	/* 클론 생성 */
	HRESULT Load_Stage_Space();
	HRESULT Load_EndingCredit();

public:
	HRESULT NativeConstruct_Environment_Generator(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;
	CGameInstance*				m_pGameInstance = nullptr;
	_float4x4					m_PivotMatrix;
	_float4x4					m_PivotMatrix_SpaceShip;

private:
	CGameObject* Create_Class(_tchar* pPrototypeTag, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	_matrix		 Set_Model_PivotMatrix(_tchar* pPrototypeTag);
	void		 Set_Info_Model(CStatic_Env::ARG_DESC& tInfo);
	void		 Set_Info_Model(CInstancing_Env::ARG_DESC& tInfo);
	void		 Adjustment_Model_Position(_tchar* pModelTag, _float4x4& rWorld);

private:
	/* 프로토타입 로드*/
	HRESULT Load_Default_Prototype_GameObject();
	HRESULT Load_Prototype_Model_ByIndex_Space(_uint iIndex);

	/* 클론 로드 */
	HRESULT Load_Environment_Space();
	HRESULT Load_Environment_Space_Boss();
	HRESULT Load_Environment_Space_SpaceShip();
	HRESULT Load_Environment_Bridge();
	HRESULT Load_Environment_SpaceRail();
	HRESULT Load_Environment_Trigger();
	HRESULT Load_Environment_Trigger_SpaceShip();
	HRESULT Load_Others();

public:
	virtual void Free() override;
};
END