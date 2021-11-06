#pragma once

#include "Client_Defines.h"
#include "Character.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
class CBehaviorTree;
class CCody final : public CCharacter
{
public:
	enum STATE { STATE_END };

private:
	explicit CCody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCody(const CCody& rhs);
	virtual ~CCody() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

private:
	virtual void KeyProcess(_double TimeDelta) override;
	//void StateCheck();

private:
	CCamera*		m_pCamera = nullptr;
	CBehaviorTree*	m_pBehaviorTree = nullptr;

public:
	static CCody* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END