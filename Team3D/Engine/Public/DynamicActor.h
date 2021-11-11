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
	}ARG_DESC;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;

private:
	PxRigidStatic* m_pActor = nullptr;

public:
	static CDynamicActor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END