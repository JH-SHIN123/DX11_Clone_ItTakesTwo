#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CSepdStaticActor final : public CActor
{
private:
	explicit CSepdStaticActor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSepdStaticActor(const CSepdStaticActor& rhs);
	virtual ~CSepdStaticActor() = default;

public: /* Struct */
	typedef struct tagArgumentDesc
	{
		class CTransform*	pTransform;
		class CModel*		pModel;
		USERDATA*			arrUserData;
	}ARG_DESC;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	void			Update_StaticActor();

private:
	class CTransform*		m_pTransform = nullptr;
	vector<PxRigidStatic*>	m_pActors;

public:
	static CSepdStaticActor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END