#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CUFORadarScreen;
class CUFORadarLever;

typedef struct tagUFORadarDesc
{
	_vector vPosition = {};
	_float  RotX = 0.f;
	_float  RotY = 0.f;
	_float  RotZ = 0.f;
}RADARDESC;

class CUFORadarSet : public CGameObject
{
protected:
	explicit CUFORadarSet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUFORadarSet(const CUFORadarSet& rhs);
	virtual ~CUFORadarSet() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual HRESULT Render_ShadowDepth() override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public: /* Getter */
	CUFORadarLever* Get_RadarLever() { return m_pUFORadarLever; }
	CUFORadarScreen* Get_RaderScreen() { return m_pUFORadarScreen; }

public: /* Setter */

private: /* Child */
	CUFORadarScreen* m_pUFORadarScreen = nullptr;
	CUFORadarLever* m_pUFORadarLever = nullptr;

private:
	RADARDESC m_tRadarDesc;

public:
	static CUFORadarSet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END