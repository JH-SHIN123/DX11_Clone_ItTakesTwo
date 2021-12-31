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
	CDynamicActor*	m_pDynamicActorCom = nullptr;

	_bool			m_bDirChange = true;
	_bool			m_bRotate_Random = false;
	_bool			m_bRotate_Start = false;

	_double			m_dCoolTime = 0.0;
	_double			m_dAngle = 0.0;
	_uint			m_iRandom = 0;

	_bool			m_bSoundCheck = false;

private:
	void Movement(_double dTimeDelta);
	void Rotate_Angle(_double dMaxAngle, _double dSpeed, _double dTimeDelta);
	void Rotate_Random(_double dSpeed, _double dTimeDelta);
	void Rotate_Fix();
	void PlaySound_by_PlayerDistance();

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CRotationBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END