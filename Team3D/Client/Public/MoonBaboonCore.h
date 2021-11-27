#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CMoonBaboonCore final : public CGameObject
{
private:
	explicit CMoonBaboonCore(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMoonBaboonCore(const CMoonBaboonCore& rhs);
	virtual ~CMoonBaboonCore() = default;

public:
	CTransform* Get_Transform() const { return m_pTransformCom; }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;

private:
	CTransform* m_pTransformCom = nullptr;

private:
	class CMoonBaboonCore_Pillar*	m_pCorePillar = nullptr;	// 장식
	class CMoonBaboonCore_Button*	m_pCoreButton = nullptr;	// 밟으면 올라감
	//class CMoonBaboonCore_Shield*	m_pCoreShield = nullptr;	// shield / pillar shield : 코어필러가 다올라오면 실드가 내려감
	//class CMoonBaboonCore_Glass*	m_pCoreGlass = nullptr;		// 데미지 달면, 부서지고, 코어 비활성화 (다내려가고, 버튼쪽 문닫힘)
	// Core

public:
	static CMoonBaboonCore* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END