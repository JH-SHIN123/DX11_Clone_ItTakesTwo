#pragma once

#include"Client_Defines.h"
#include "Component.h"
#include"Film.h"
#include"CamEffect.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)
class CCam_Helper : public CComponent
{
public:
	enum class CamHelperState { Helper_None, Helper_Act, Helper_SeeCamNode, Helper_End };
public:
	explicit	CCam_Helper(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	explicit	CCam_Helper(const CCam_Helper& rhs);
	virtual		~CCam_Helper() = default;


	virtual HRESULT NativeConstruct_Prototype()override;
public: //None일경우 자유이동 , 재생중이면 다른거 반환.이후 카메라는 Tick_Film으로 매트릭스얻어서 set,
	CamHelperState		Tick(_double TimeDelta,CFilm:: ScreenType eScreenTypeIdx);
	CamHelperState		Get_CamHelperState(CFilm::ScreenType eScreen) { return m_eState[eScreen]; }
public:
	//For.Film
	HRESULT		Add_Film(const _tchar* pFilmName, CFilm* pFilm, _double dDuration);
	HRESULT		Add_CamNode(const _tchar* pFilmName, CFilm::CamNode* pNode);
	CFilm*		Get_Film(const _tchar* pFilmName);

	//필름에 설정된 카메라의 정보를 바탕으로 매트릭스 만들어서 리턴
	_fmatrix	Tick_Film(_double dTimeDelta, CFilm::ScreenType eScreenTypeIdx, _float* fOutFovY);
	//콜하면 필름찾고 연출 들어감.
	void		Start_Film(const _tchar* pFilmName, CFilm::ScreenType eScreenTypeIdx);

	//For.CamNode
	//얘 부르면 카메라에서 매트릭스 다얻기전까지 return Helper_SeeCamNode
	void		SeeCamNode(CFilm::CamNode* pCamNode, CFilm::ScreenType eScreenTypeIdx);
	_fmatrix	Get_CamNodeMatrix(CTransform* pCamTransform, _double dTimeDelta, CFilm::ScreenType eScreenTypeIdx);

	//For.GetMatrix
	_fmatrix	Get_CamNodeMatrix(CFilm::CamNode* pCamNode1, CFilm::CamNode* pCamNode2, CFilm::CamNode* pCamNode3, CFilm::CamNode* pCamNode4, _double dTime,_bool* pIsFinishedNode);


	_fmatrix MakeViewMatrix(_float3 Eye, _float3 At);
	_float3 VectorLerp(_float3 vDst, _float3 vSrc,_float fT);
	_float3	 MakeBezier3(_float3& v1, _float3& v2, _float3& v3, _double dTime);
	_float3	 MakeBezier4(_float3& v1, _float3& v2, _float3& v3, _float3& v4, _double dTime);
private:
	//For.Film
	typedef unordered_map<const _tchar*,CFilm*> FILMS;
	FILMS m_Films;

	CFilm*				m_pCurFilm[CFilm::Screen_End] = { nullptr ,nullptr };
	_bool				m_bIsPlayingAct[CFilm::Screen_End] = { false ,false };
	_double				m_dTime[CFilm::Screen_End] = { 0.f,0.f };

	//For.CamNode
	_bool				m_bIsSeeCamNode[CFilm::Screen_End] = { false,false };
	_float				m_fCamNodeLerpTime[CFilm::Screen_End] = { 0.f,0.f };
	_float4x4			m_matCamNode[CFilm::Screen_End];
private:
	CFilm* Find_Film(const _tchar* pFilm);

private: //For.CamEffect
		 //카메라 효과는 필름이나 카메라상태하고는 상관관계없이 호출되면 끝날때까지 지속
	typedef unordered_map<const _tchar*, CCamEffect*> CAMEFFECTS;
	CAMEFFECTS m_CamEffects;

	CCamEffect*	m_pCurEffect[CFilm::Screen_End] = { nullptr,nullptr };
	_bool				m_bIsPlayingCamEffect[CFilm::Screen_End] = { false,false };

public:
	_bool			Get_IsCamEffectPlaying(CFilm::ScreenType eScreen) { return m_bIsPlayingCamEffect[eScreen]; }
	_bool			Tick_CamEffect(CFilm::ScreenType eScreen, _double dTimeDelta, _fmatrix matIn);
	HRESULT			Add_CamEffect(const _tchar* pCamEffectTag, CCamEffect* pCamEffect);
	HRESULT			Add_CamShakeCycleDesc(const _tchar* pCamEffectTag,CCamEffect::CamShakeCycleDesc* pDesc);
	HRESULT			Start_CamEffect(const _tchar* pEffectName, CFilm::ScreenType eScreen);
	_fmatrix		Get_CurApplyCamEffectMatrix(CFilm::ScreenType eScreen);

private:
	class CCamEffect* Find_CamEffect(const _tchar* pCamEffectName);

private: //For.System
	CamHelperState m_eState[CFilm::Screen_End] = { CamHelperState::Helper_End,CamHelperState::Helper_End };
	HRESULT Load_Film(const _tchar* pDataPath,_bool bMakeUpNodesTimeByfar = false);
	void	MakeUpNodesTimeByFar(CFilm* pFilm); //거리에 따른 시간정리
public:
	static CCam_Helper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	virtual CComponent* Clone_Component(void* pArg = nullptr)override;
	virtual void Free();

public:
	FILMS* Get_Films() { return &m_Films; }
};
END

