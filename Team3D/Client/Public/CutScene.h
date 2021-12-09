#pragma once

#include"Client_Defines.h"
#include"Cody.h"

BEGIN(Client)
class CCutScene : public CBase
{
public:
	enum class CutSceneOption {
		CutScene_Intro,CutScene_Active_GravityPath_01,CutScene_End
	};
public:
	CCutScene();
	virtual ~CCutScene() = default;
public:
	_bool Tick_CutScene(_double dTimeDelta);
	
	_bool Tick_CutScene_Intro(_double dTimeDelta);
	_bool Tick_CutScene_Active_GravityPath_01(_double dTimeDelta);
public:
	HRESULT Start_CutScene();


	//For.Ready_CutScene.Set Performers;
	HRESULT Start_CutScene_Intro();
	HRESULT Start_CutScene_Active_GravityPath_01();

	//For.End
	HRESULT End_CutScene_Intro();
	HRESULT End_CutScene_Active_GravityPath_01();

private:
	HRESULT Ready_CutScene_Intro();
	HRESULT Ready_CutScene_Active_GravityPath_01();
private:
	HRESULT NativeConstruct(CutSceneOption eOption);
	_fmatrix MakeRollPitchYawMatrix(_float3 vPos, _float3 vScale, _float3 vRot);
private:
	class CCutScenePlayer* m_pCutScenePlayer = nullptr;


	_double m_dDuration = 0.0;
	_double m_dTime = 0.0;
	CutSceneOption m_eCutSceneOption = CutSceneOption::CutScene_End;

	//For.Intro
	_float m_fCodySizingTime = 0.f;
	CCody::PLAYER_SIZE m_eCurCodySize = CCody::PLAYER_SIZE::SIZE_MEDIUM;
	CCody::PLAYER_SIZE m_ePreCodySize = CCody::PLAYER_SIZE::SIZE_MEDIUM;
	//For.Active_GravityPipe_01
	_bool m_bIsStartFilm = false;
public:
	static CCutScene* Create(CutSceneOption eOption);
	virtual void Free() override;
};

END