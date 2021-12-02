#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CPipeCurve : public CGameObject
{
protected:
	explicit CPipeCurve(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPipeCurve(const CPipeCurve& rhs);
	virtual ~CPipeCurve() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;
public:
	void Set_Position(_vector vPosition);
	void Set_Rotate(_bool IsCheck);

public:
	_bool Get_Connected();

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	_bool				m_IsRotate = false;
	_float				m_fAngle = 0.f;
	_float				m_fAngleIncreaseLimit = 0.f;
	_uint				m_iOption = 0;
	_bool				m_IsConnected = false;

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CStaticActor*		m_pStaticActorCom = nullptr;

private:
	void CompareAngle(_float fAngle);

public:
	static CPipeCurve* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END