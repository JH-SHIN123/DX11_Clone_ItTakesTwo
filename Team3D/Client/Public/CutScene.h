#pragma once

#include"Client_Defines.h"
#include"Cody.h"

BEGIN(Client)
class CCutScene : public CBase
{
public:
	enum class CutSceneOption {
		CutScene_Intro,CutScene_Active_GravityPath_01,CutScene_Clear_Umbrella,CutScene_Clear_Rail,
		CutScene_Boss_Intro,CutScene_Eject_InUFO,
		CutScene_GotoMoon,CutScene_Outro,CutScene_End
	};
public:
	CCutScene();
	virtual ~CCutScene() = default;
	CutSceneOption Get_Option() { return m_eCutSceneOption; }
public:
	_bool Tick_CutScene(_double dTimeDelta);
	
	_bool Tick_CutScene_Intro(_double dTimeDelta);
	_bool Tick_CutScene_Active_GravityPath_01(_double dTimeDelta);
	_bool Tick_CutScene_Clear_Umbrella(_double dTimeDelta);
	_bool Tick_CutScene_Clear_Rail(_double dTimeDelta);
	_bool Tick_CutScene_Boss_Intro(_double dTimeDelta);
	_bool Tick_CutScene_Eject_InUFO(_double dTimeDelta);
	_bool Tick_CutScene_GotoMoon(_double dTimeDelta);
	_bool Tick_CutScene_Outro(_double dTimeDelta);
public:
	HRESULT Start_CutScene();
	//For.Ready_CutScene.Set Performers;
	HRESULT Start_CutScene_Intro();
	HRESULT Start_CutScene_Active_GravityPath_01();
	HRESULT Start_CutScene_Clear_Umbrella();
	HRESULT Start_CutScene_Clear_Rail();
	HRESULT Start_CutScene_Boss_Intro();
	HRESULT Start_CutScene_Eject_UFO();
	HRESULT Start_CutScene_GotoMoon();
	HRESULT Start_CutScene_Outro();
	//For.End
	HRESULT End_CutScene_Intro();
	HRESULT End_CutScene_Active_GravityPath_01();
	HRESULT End_CutScene_Clear_Umbrella();
	HRESULT End_CutScene_Clear_Rail();
	HRESULT End_CutScene_Boss_Intro();
	HRESULT End_CutScene_Eject_InUFO();
	HRESULT End_CutScene_GotoMoon();
	HRESULT End_CutScene_Outro();
private:
	HRESULT Ready_CutScene_Intro();
	HRESULT Ready_CutScene_Active_GravityPath_01();
	HRESULT Ready_CutScene_Clear_Umbrella();
	HRESULT Ready_CutScene_Clear_Rail();
	HRESULT Ready_CutScene_Boss_Intro();
	HRESULT Ready_CutScene_Eject_InUFO();
	HRESULT Ready_CutScene_GotoMoon();
	HRESULT Ready_CutScene_Outro();
private:
	HRESULT NativeConstruct(CutSceneOption eOption);
	_fmatrix MakeRollPitchYawMatrix(_float3 vPos, _float3 vScale, _float3 vRot);

	//For.Intro
	void StartCodyLerp(_float3 vStartPos, _float3 vStartScale,_float3 vTargetPos , _float3 vTargetScale, _uint iLerpCount);
	void CodyLerp();
	//For.Outro
	void CodyMove_EjectUfo(_double dTimeDelta);
private:
	void Script_Intro(_double dTimeDelta);
	void Script_Boss_Intro(_double dTimeDelta);
	void Script_Eject_InUFO(_double dTimeDelta);
	void Script_Outro(_double dTimeDelta);
private:
	class CCutScenePlayer* m_pCutScenePlayer = nullptr;


	_double m_dDuration = 0.0;
	_double m_dTime = 0.0;
	CutSceneOption m_eCutSceneOption = CutSceneOption::CutScene_End;

	//For.Intro
	_float m_fCodySizingTime = 0.f;
	_uint	m_iCutSceneTake = 0;
	_bool	m_bIsChangeToCody = false;
	_bool	m_bIsChangeToMay = false;

	_bool	m_bStartLerpCody = false;
	_uint	m_iCodyLerpCount = 0;
	_uint	m_iMaxCodyLerpCount = 0;
	
	_float3 m_vTargetCodyPos = { 0.f,0.f,0.f };
	_float3 m_vTargetCodyScale = { 0.f,0.f,0.f };

	_float3 m_vCodyPos = { 0.f,0.f,0.f };
	_float3	m_vCodyScale = { 0.f,0.f,0.f };



	_float	m_fCodyLerpTime = 0.f;
	//For.Active_GravityPipe_01
	_bool m_bIsStartFilm = false;
	//For.BossIntro	
	_bool	m_bButtonPress[5] = { false,false,false,false,false };
	//For.Outro
	_float m_fCodyMove_EjectUfoTime = 0.f;
	_bool  m_bCodyEjectUFO = false;
public:
	static CCutScene* Create(CutSceneOption eOption);
	virtual void Free() override;
};

END