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
		class CTransform*	pTransform;
		class CModel*		pModel;
		USERDATA*			pUserData;
	}ARG_DESC;

public:
	/* True면 피직스 물리 안먹음 */
	void Set_Simulation(_bool bCheck);

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	void			Update_StaticActor();

private:
	class CTransform*		m_pTransform = nullptr;
	vector<PxRigidStatic*>	m_pActors;

public:
	static CStaticActor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END

/*
	Static 액터의 Update는 Transform 위치로 액터를 이동시킴.
	Dynamic 액터의 Update는 액터의 위치로 Transform을 이동시킴.

	Static 액터는 메쉬 형태의 액터만 만듦.
*/