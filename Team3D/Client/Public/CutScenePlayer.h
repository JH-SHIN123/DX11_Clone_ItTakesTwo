#pragma once
#include "Client_Defines.h"
#include "CutScene.h"
BEGIN(Client)
class CCutScenePlayer :public CBase
{
	DECLARE_SINGLETON(CCutScenePlayer)
public:
	CCutScenePlayer();
	virtual ~CCutScenePlayer() = default;

	HRESULT NativeConstruct(ID3D11Device* pDevice,ID3D11DeviceContext* pDeviceContext);
public:
	HRESULT Add_CutScene(const _tchar* pCutSceneTag,CCutScene* pCutScene);
	_bool	Get_IsPlayCutScene() { return m_bIsPlayingCutScene; }
	//if Finish,Return false;
	_bool	Tick_CutScene(_double dTimeDelta);
	HRESULT Start_CutScene(const _tchar* pCutSceneTag);
	void	Stop_CutScene();
	void	Set_ViewPort(_fvector vLScreenDesc, _fvector vRScreenDesc, _bool bIsLerp,_float fLerpSpeed = 1.f);
public:
	HRESULT		Add_Performer(const _tchar* pPerformerTag, CGameObject* pPerformer);
	CGameObject* Find_Performer(const _tchar* pPerformerTag);
	CCutScene* Find_CutScene(const _tchar* pCutSceneTag);
private:
	typedef unordered_map<const _tchar*,CCutScene*> CUTSCENES;
	CUTSCENES m_CutScenes;
	CCutScene* m_pCurCutScene = nullptr;

	typedef unordered_map<const _tchar*, CGameObject*> PERFORMERS;
	PERFORMERS m_Performers;

	_bool m_bIsPlayingCutScene = false;

	class CGameInstance* m_pGameInstance = nullptr;

public:
	virtual void Free() override;

};

END