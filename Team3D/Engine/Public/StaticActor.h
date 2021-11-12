#pragma once

#include "Actor.h"

BEGIN(Engine)

class ENGINE_DLL CStaticActor final : public CActor
{
private:
	explicit CStaticActor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStaticActor(const CStaticActor& rhs);
	virtual ~CStaticActor() = default;

public: /* Struct */
	typedef struct tagArgumentDesc
	{
		_matrix			WorldMatrix;
		PxGeometry*		pGeometry;
		PxMaterial*		pMaterial;
		const char*		pActorName;

		tagArgumentDesc() {}
		tagArgumentDesc(_fmatrix _WorldMatrix, PxGeometry* _pGeometry, PxMaterial* _pMaterial, const char* pName) : WorldMatrix(_WorldMatrix), pGeometry(_pGeometry), pMaterial(_pMaterial), pActorName(pName) {}
	}ARG_DESC;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;

private:
	PxRigidStatic* m_pActor = nullptr;

public:
	static CStaticActor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END