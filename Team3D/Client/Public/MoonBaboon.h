#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CMoonBaboon : public CGameObject
{
protected:
	explicit CMoonBaboon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMoonBaboon(const CMoonBaboon& rhs);
	virtual ~CMoonBaboon() = default;

public:
	enum MOON_STATE {
		MOON_ENTIRE_ANIMATION, MOON_HITREACTION, MOON_JETPACK_FLYING, MOON_JETPACK_FLYING_START, MOON_JETPACK_LANDING_MH, MOON_JETPACK_LANDTORUN, MOON_JETPACK_MH, MOON_JETPACKTOLANDING,
		MOON_RUN, MOON_UFO_CODYHOLDING, MOON_UFO_CODYHOLDING_ENTER, MOON_UFO_FIREROCKET, MOON_UFO_GROUNDPOUND, MOON_UFO_INSIDE_MH, MOON_UFO_KNOCKDOWNMH, MOON_UFO_LASER_HITPOD, MOON_UFO_MH,
		MOON_UFO_PROGRAMMING, MOON_CUTSCENE_BEAMUP_INTRO, CUTSCENE_EJECT_MOONBABOON, CUTSCENE_ENTERUFO_MOONBABOON, CUTSCENE_BOSSINTRO_MOONBABOON, CUTSCENE_OUTRO_MOONBABOON, 
		CUTSCENE_POWERCOREDESTROYED_MOONBABOON, CUTSCENE_LASERRIPPEDOFF_MOONBABOON, CUTSCENE_ROCKETPHASEFINISHED_MOONBABOON, MOON_STATE_END
	};
	enum MOON_TARGET { TARGET_CODY, TARGET_MAY, TARGET_END };

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }
	CModel*		Get_Model() { return m_pModelCom; }

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	//CControllableActor* m_pActorCom = nullptr;

	CTransform*			m_pCodyTransform = nullptr;
	CTransform*			m_pMayTransform = nullptr;

	CModel*				m_pUFOModel = nullptr;
	CTransform*			m_pUFOTransform = nullptr;

public:
	CMoonBaboon::MOON_STATE Check_State(_double dTimeDelta);
	void Change_State(_double dTimeDelta);
	void During_Animation_Behavior(_double dTimeDelta);

	void Fix_MoonBaboon_Chair(_double dTimeDelta);


private:
	MOON_STATE m_eCurState = MOON_ENTIRE_ANIMATION;
	MOON_STATE m_eNextState = MOON_ENTIRE_ANIMATION;

	MOON_TARGET m_eTarget = TARGET_END;




public:
	static CMoonBaboon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END