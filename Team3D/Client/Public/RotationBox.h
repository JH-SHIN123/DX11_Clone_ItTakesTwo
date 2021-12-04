#pragma once
#include "Dynamic_Env.h"

BEGIN(Client)
class CRotationBox final : public CDynamic_Env
{
private:
	explicit CRotationBox(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRotationBox(const CRotationBox& rhs);
	virtual ~CRotationBox() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

private:
	CStaticActor*	m_pStaticActorCom = nullptr;

	_bool			m_bTrigger = true;
	_bool			m_bDirChange = false;
	_bool			m_bSwitching = false;

	_double			m_dCoolTime = 0.f;
	_float			m_fAngle = 0.f;
	_uint			m_iRandom = 0;

private:
	void Movement(_double dTimeDelta);
	void Rotate_Right(_float fMaxAngle, _double dTimeDelta);
	void Rotate_Left(_float fMaxAngle, _double dTimeDelta);
	void Random_Rotate(_double dTimeDelta);

private:	// ¼öÁ¤ Áß
	_bool m_bRotate_Left = true;
	_bool m_bRotate_Random = false;
	_bool m_bRotate_Start = false;
	_double m_dRotate_Random = 0.f;

	void Rotate_Angle(_float fMaxAngle, _double dTimeDelta);
	void Rotate_Random(_double dTimeDelta);

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CRotationBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END