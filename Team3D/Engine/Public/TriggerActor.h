#pragma once

#include "Actor.h"

BEGIN(Engine)

class ENGINE_DLL CTriggerActor final : public CActor
{
private:
	explicit CTriggerActor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTriggerActor(const CTriggerActor& rhs);
	virtual ~CTriggerActor() = default;

public: /* Struct */
	typedef struct tagArgumentDesc
	{
		class CTransform*	pTransform;
		PxGeometry*			pGeometry;
		USERDATA*			pUserData;
	}ARG_DESC;

public:
	PxRigidStatic* Get_Actor();

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	void			Update_TriggerActor();
	/* 위치 보정해야하는 경우 */
	void			Update_TriggerActor(_fvector vOffset);

private:
	class CTransform*	m_pTransform = nullptr;
	PxRigidStatic*		m_pActor = nullptr;

public:
	static CTriggerActor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END