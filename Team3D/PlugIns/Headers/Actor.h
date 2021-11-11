#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CActor abstract : public CComponent
{
protected:
	explicit CActor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CActor(const CActor& rhs);
	virtual ~CActor() = default;

public: /* Struct */
	typedef struct tagActorData
	{

	}ACTORDATA;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;

protected:
	class CPhysX* m_pPhysX = nullptr;

public:
	virtual CComponent* Clone_Component(void* pArg) PURE;
	virtual void Free() PURE;
};

END