#pragma once

#include "Base.h"

BEGIN(Engine)

typedef struct ENGINE_DLL tagActorData
{

}ACTORDATA;

class CPhysX final : public CBase
{
	DECLARE_SINGLETON(CPhysX)
private:
	explicit CPhysX() = default;
	virtual ~CPhysX() = default;

public:
	HRESULT Ready_PhysX();


private:
	PxDefaultAllocator		m_Allocator;
	PxDefaultErrorCallback	m_ErrorCallback;
	PxFoundation*			m_pFoundation = nullptr;
	PxPhysics*				m_pPhysics = nullptr;
	PxDefaultCpuDispatcher*	m_pDispatcher = nullptr;
	PxScene*				m_pScene = nullptr;
	PxMaterial*				m_pMaterial = nullptr;
	PxCooking*				m_pCooking = nullptr;

public:
	virtual void Free() override;

#ifdef _DEBUG
	PxPvd* m_pPVD = nullptr;
#endif
};

END