#pragma once
#include "GameObject.h"

BEGIN(Client)
class C3DText final : public CGameObject
{
public:
	typedef struct tagArgument
	{
		_uint	iIndex = 0;
		_float	fPosY = 0.f;
	}ARG_DESC;

private:
	explicit C3DText(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit C3DText(const C3DText& rhs);
	virtual ~C3DText() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTriggerActor*		m_pTriggerActorCom = nullptr;
	CTransform*			m_pCodyTransformCom = nullptr;
	_uint				m_iIndex = 0;

private:
	HRESULT Ready_Component(void* pArg);
	HRESULT Create_Particle();

public:
	static C3DText* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END