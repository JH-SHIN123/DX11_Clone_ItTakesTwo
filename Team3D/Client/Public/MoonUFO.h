#pragma once
#include "Client_Defines.h"

BEGIN(Client)
class CMoonUFO : public CGameObject
{
protected:
	explicit CMoonUFO(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMoonUFO(const CMoonUFO& rhs);
	virtual ~CMoonUFO() = default;

public:/* Getter */
	CTransform* Get_Transform() { return m_pTransformCom; }
	CModel*		Get_Model()		{ return m_pModelCom; }

public:
	enum UFO_STATE {
		UFO_ENTIRE_ANIMATION , UFO_REF, UFO_BACK,
		UFO_CODYHOLDING, UFO_CODYHOLDING_ENTER, UFO_CODYHOLDING_LOW, 
		UFO_CONTROLLABLE_ADDITIVE, UFO_CONTROLLABLE_ADDITIVE_BOOST, UFO_CONTROLLABLE_ADDITIVE_FLYING,
		UFO_CONTROLLABLE_POSE_BCK, UFO_CONTROLLABLE_POSE_FWD, UFO_CONTROLLABLE_POSE_LEFT, UFO_CONTROLLABLE_POSE_RIGHT, 
		UFO_FIREROCKET_ADDITIVE_LEFT, UFO_FIREROCKET_ADDITIVE_RIGHT, UFO_FWD, UFO_GROUNDPOUND, UFO_HITREACTION_BCK, UFO_HITREACTION_FWD, 
		UFO_HITREACTION_LEFT, UFO_HITREACTION_RIGHT, UFO_KNOCKDOWNMH, UFO_LASER_HITPOD, UFO_LASER_MH, UFO_LASERRIPPEDOFF, UFO_LEFT, UFO_MH, UFO_RIGHT, UFO_ROCKETKNOCKDOWN_MH, 
		CUTSCENE_EJECT_FLYINGSAUCER, CUTSCENE_ENTERUFO_FLYINGSAUCER, CUTSCENE_UFO_BOSS_INTRO, CUTSCENE_UFO_LASERRIPPEDOFF_FLYINGSAUCER, 
		CUTSCENE_UFO_OUTRO, CUTSCENE_POWERCORESDESTROYED_UFO, CUTSCENE_ROCKETPHASEFINISHED_FLYINGSAUCER
	};

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

private:
	/* For.Component */
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	CDynamicActor*			m_pDynamicActorCom = nullptr;
	PxSphericalJoint*		m_pJoint = nullptr;

	UFO_STATE				m_eCurState = UFO_ENTIRE_ANIMATION;
	UFO_STATE				m_eNextState = UFO_ENTIRE_ANIMATION;

	CTransform*				m_pCodyTransform = nullptr;
	CTransform*				m_pMayTransform = nullptr;

private:
	void				KeyInPut(_double dTimeDelta);
	CMoonUFO::UFO_STATE Check_State(_double dTimeDelta);
	void				Change_State(_double dTimeDelta);
	void				During_Animation_Behavior(_double dTimeDelta);
	void				Laser_Pattern(_double dTimeDelta);

	void				Test(_double dTimeDelta);

private:
	_bool m_bRotateRight = false;
	_bool m_bRotateLeft = false;

private:
	HRESULT Ready_Component(void* pArg);

public:
	static CMoonUFO*     Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void         Free() override;
};
END