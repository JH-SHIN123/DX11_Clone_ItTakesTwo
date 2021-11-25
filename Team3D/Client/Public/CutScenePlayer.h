#pragma once
#include"Client_Defines.h"
#include "CutScene.h"

BEGIN(Client)
class CCutScenePlayer :public CBase
{
	DECLARE_SINGLETON(CCutScenePlayer)
public:
	CCutScenePlayer();
	virtual ~CCutScenePlayer() = default;


public:



private:
	typedef unordered_map<const _tchar*, CCutScene*> CUTSCENES;
	CUTSCENES m_CutScenes;



	// CBase을(를) 통해 상속됨
	virtual void Free() override;

};

END