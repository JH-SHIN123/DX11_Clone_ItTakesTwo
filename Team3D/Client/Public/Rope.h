#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
typedef struct tagRopeDesc
{
	_uint iNumPlayer = 0;
}ROPEDESC;

class CRope final : public CGameObject
{

private:
	explicit CRope(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRope(const CRope& rhs);
	virtual ~CRope() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual HRESULT Render_ShadowDepth() override;

public: /* Getter */
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	void Launch_Rope(_double dTimeDelta);

	_uint m_iNumPlayer = 0;
	_float m_fSizeY = 0.f;

private:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;

	CTransform*			m_pTargetTransformCom = nullptr;
	CModel*				m_pTargetModel = nullptr;

public:
	static CRope* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END