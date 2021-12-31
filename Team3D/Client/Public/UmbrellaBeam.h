#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CUmbrellaBeam : public CGameObject
{
protected:
	explicit CUmbrellaBeam(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUmbrellaBeam(const CUmbrellaBeam& rhs);
	virtual ~CUmbrellaBeam() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	void Set_BeamActivate(_bool IsCheck);

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	_float										m_fVerticalAngle = 0.f;
	_float										m_fHorizontalAngle = 0.f;
	_float										m_fUmbrellaSoundVolume = 1.f;
	_float										m_fUmbrellaEffectSoundVolume = 0.5f;
	_bool										m_IsBeamActivate = false;
	_bool										m_IsPutGravitationalField = false;

private:
	class CUmbrellaBeam_Stand*					m_pUmbrellaBeam_Stand = nullptr;
	class CEffect_Umbrella_Pipe*				m_pUmbrellaBeam_Effect = nullptr;

protected:
	/* For.Component */
	CRenderer*									m_pRendererCom = nullptr;
	CTransform*									m_pTransformCom = nullptr;
	CModel*										m_pModelCom = nullptr;
	CStaticActor*								m_pStaticActorCom = nullptr;
	CTriggerActor*								m_pTriggerCom = nullptr;

private:
	void KeyInput_Rotate(_double TimeDelta);
	void PutGravitationalField();

private:
	HRESULT Ready_Layer_UmbrellaBeam_Stand(const _tchar * pLayerTag);

public:
	static CUmbrellaBeam* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END