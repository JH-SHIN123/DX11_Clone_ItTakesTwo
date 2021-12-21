#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CStaticActor;
END

BEGIN(Client)
class CMoonBaboon_MainLaser final : public CGameObject
{
private:
	explicit CMoonBaboon_MainLaser(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMoonBaboon_MainLaser(const CMoonBaboon_MainLaser& rhs);
	virtual ~CMoonBaboon_MainLaser() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	_bool Get_LaserUp() const { return m_IsLaserUp; }
	CTransform* Get_Transform() const { return m_pTransformCom; }

public:
	void Set_LaserOperation(_bool IsActive);
	void Set_MainLaserUp(_float fMaxDistance, _float fSpeed);

public:
	_bool Get_ArrivalLastFloor() const { return m_IsArrivalLastFloor; }


private:
	void Laser_AttackPattern(_double TimeDelta);
	void Laser_Down(_double TimeDelta);


private:
	_uint	m_iPatternState = 0;
	_double m_dPatternDeltaT = 0.0;
	_double m_dDownTime = 0.0;
	_bool	m_IsLaserOperation = false;
	_bool	m_IsLaserUp = false;
	_bool	m_IsLaserCreate = true;
	_float3	m_vMaxPos;
	_float m_fMaxY = 0.f;
	_bool m_IsGoUp = false;
	_float m_fUpSpeed = 0.f;
	_float m_fDistance = 0.f;
	_bool m_IsArrivalLastFloor = false;

private:
	vector<class CLaser_TypeB*>				m_vecLaser_TypeB;

private:
	void GoUp(_double dTimeDelta);

private: /* For.Component */
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CModel*			m_pModelCom = nullptr;
	/*CStaticActor*	m_pStaticActorCom = nullptr;*/

public:
	static CMoonBaboon_MainLaser* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END
