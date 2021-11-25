#pragma once

#include "Client_Defines.h"
#include "Actor.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CCameraActor final : public CActor
{
private:
	explicit CCameraActor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCameraActor(const CCameraActor& rhs);
	virtual ~CCameraActor() = default;

public: /* Struct */
	typedef struct tagArgumentDesc
	{
		class CTransform*	pTransform;
		USERDATA*			pUserData;
	}ARG_DESC;

public: /* Setter */
	void	Set_Position(_fvector vPosition);

public:
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual HRESULT	NativeConstruct_Prototype() override;
	void	Move(_fvector vMove, _double dTimeDelta);
	void	Update(_double dTimeDelta);
	void	Set_CorrectPosition();

private:
	GameID::Enum					m_eID;
	PxController*					m_pController = nullptr;
	PxRigidDynamic*					m_pActor = nullptr;
	PxRigidStatic*					m_pTrigger = nullptr;
	PxControllerFilters*			m_pFilters = nullptr;
	CTransform*						m_pTransform = nullptr;
	class CCameraBehaviorCallback*	m_pBehaviorCallback = nullptr;
	class CCameraFilterCallback*	m_pFilterCallback = nullptr;

public:
	static CCameraActor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END