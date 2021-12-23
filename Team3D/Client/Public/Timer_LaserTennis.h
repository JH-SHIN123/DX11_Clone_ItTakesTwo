#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CTimer_LaserTennis final : public CGameObject
{
private:
	explicit CTimer_LaserTennis(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTimer_LaserTennis(const CTimer_LaserTennis& rhs);
	virtual ~CTimer_LaserTennis() = default;

public:/* Setter */
	void OnOff_Timer(_bool bOnOff);
	void Set_Ready() { m_bReady = true; }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;

private:
	_bool			m_bReady = false;
	_bool			m_bOnOff = false;

	_double			m_dStartTime = 0.0;
	_double			m_dButton_Time = 0.0;
	_double			m_dLargeButton_Time = 0.0;
	_double			m_dLaserActivation_Time = 0.0;

	_uint			m_iLaserActivation_Count = 0;

public:
	static CTimer_LaserTennis* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone_GameObject(void* pArg);
	virtual void Free() override;
};
END
