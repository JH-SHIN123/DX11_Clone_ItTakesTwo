#pragma once

#include "Client_Defines.h"
#include "Actor.h"

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
	void	Set_IsCollision(_bool bSet) { m_bIsCollision = bSet; };
	/*Getter*/
	_fvector Get_Position();
	_bool	Get_Collision() { return m_bIsCollision; }
public:
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual HRESULT	NativeConstruct_Prototype() override;
	PxControllerCollisionFlags	Move(_fvector vMove, _double dTimeDelta);
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
	class CCameraHitReport*			m_pHitReport = nullptr;

	_bool							m_bIsCollision = false;
public:
	static CCameraActor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END