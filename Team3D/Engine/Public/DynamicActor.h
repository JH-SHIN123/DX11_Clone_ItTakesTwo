#pragma once

#include "Actor.h"

BEGIN(Engine)

class ENGINE_DLL CDynamicActor final : public CActor
{
private:
	explicit CDynamicActor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CDynamicActor(const CDynamicActor& rhs);
	virtual ~CDynamicActor() = default;

public: /* Struct */
	typedef struct tagArgumentDesc
	{
		class CTransform*	pTransform;
		PxGeometry*			pGeometry;
		_float				fDensity;
		PxVec3				vVelocity;
		USERDATA*			pUserData;
	}ARG_DESC;

public:
	PxRigidDynamic*&	Get_Actor();

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	void			Update_DynamicActor();

private:
	class CTransform*	m_pTransform = nullptr;
	PxRigidDynamic*		m_pActor = nullptr;

public:
	static CDynamicActor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END

/*
	Dynamic 액터의 Update는 액터의 위치로 Transform을 이동시킴.
	Static 액터의 Update는 Transform 위치로 액터를 이동시킴.
*/