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
class CMoonBaboon_SubLaser final : public CGameObject
{
private:
	explicit CMoonBaboon_SubLaser(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMoonBaboon_SubLaser(const CMoonBaboon_SubLaser& rhs);
	virtual ~CMoonBaboon_SubLaser() = default;

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
	void SetUp_SubLaserPosition(_uint iOption);

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

private:
	vector<class CLaser_TypeB*>				m_vecLaser_TypeB;

private: /* For.Component */
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CModel*			m_pModelCom = nullptr;

public:
	static CMoonBaboon_SubLaser* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END
