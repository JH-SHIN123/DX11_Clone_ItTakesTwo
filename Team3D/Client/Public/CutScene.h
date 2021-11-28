#pragma once

#include"Client_Defines.h"


BEGIN(Client)
class CCutScene : public CBase
{
public:
	enum class CutSceneOption {
		CutScene_Intro,CutScene_End
	};
public:
	CCutScene();
	virtual ~CCutScene() = default;
public:
	_bool Tick_CutScene(_double dTimeDelta);
	

public:
	HRESULT Start_CutScene();


	//For.Ready_CutScene.Set Performers;
	HRESULT Start_CutScene_Intro();
private:
	HRESULT Ready_CutScene_Intro();
private:
	HRESULT NativeConstruct(CutSceneOption eOption);
private:
	class CCutScenePlayer* m_pCutScenePlayer = nullptr;


	_double m_dDuration = 0.0;
	_double m_dTime = 0.0;
	CutSceneOption m_eCutSceneOption = CutSceneOption::CutScene_End;
public:
	static CCutScene* Create(CutSceneOption eOption);
	virtual void Free() override;
};

END