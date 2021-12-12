#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include"Cam_Helper.h"
#include"Cody.h"


BEGIN(Client)
class CMainCamera final : public CCamera
{
	enum CamRev {Rev_Holizontal,Rev_Prependicul,Rev_End};

	enum class CamMode{Cam_Free,Cam_AutoToFree,Cam_Warp_WormHole,Cam_PressButton_Bridge,Cam_End};
	//O CamFreeMove P FollowPlayer										//BossMiniRoom_Cody
	enum class CamFreeOption { Cam_Free_FollowPlayer, Cam_Free_FreeMove,Cam_Free_OnRail, Cam_Free_Boss_MiniRoom,Cam_Free_End };

private:
	explicit CMainCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMainCamera(const CMainCamera& rhs);
	virtual ~CMainCamera() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;


	CTransform* Get_Transform() { return m_pTransformCom; }
	CCam_Helper* Get_CamHelper() { return m_pCamHelper; }
	void		Set_StartPortalMatrix(_fmatrix matWorld) { XMStoreFloat4x4(&m_matStartPortal,matWorld); }

	HRESULT Start_Film(const _tchar* pFilmTag);
private:
	_int	Check_Player(_double dTimeDelta);
	void	Set_Zoom(_float fZoomVal, _double dTimeDelta);
	_bool	LerpToCurSize(CCody::PLAYER_SIZE eSize,_double dTimeDelta);
private:
	//For Free.
	_int	Tick_Cam_Free(_double dTimeDelta);					//자유이동
	_int	Tick_Cam_AutoToFree(_double dTimeDelta);			//연출 카메라 -> 자유이동시 보간
	_int	Tick_Cam_Warp_WormHole(_double dTimeDelta);			//웜홀
	_int	Tick_Cam_PressButton_Bridge(_double dTimeDelta);	//다리앞의 버튼


	_int	Tick_Cam_Free_FollowPlayer(_double dTimeDelta);		//카메라가 플레이어를쫓아가며 이동(메인 카메라)
	_int	Tick_Cam_Free_FreeMode(_double dTimeDelta);			//카메라가 자유롭게 이동함
	_int	Tick_Cam_Free_OnRail(_double dTimeDelta);			//미니코디 보스룸

	//CamHelper State(현재 )
	_int	Tick_CamHelperNone(_double dTimeDelta);			//현재 아무것도 재생안함
	_int	Tick_CamHelper_Act(_double dTimeDelta);			//재생중
	_int	Tick_CamHelper_SeeCamNode(_double dTimeDelta);	//카메라노드를 처다봄


	//For.Debug
	void KeyCheck(_double dTimeDelta);
private:
	_int	ReSet_Cam_FreeToAuto();		//카메라가 초기상태로 돌아옴
	_int	ReSet_Cam_Free_OnRail();
	_bool	OffSetPhsX(_fmatrix matWorld,_fvector vAt,_double dTimeDelta,_vector * pOut);

	_fmatrix MakeViewMatrixByUp(_float4 Eye, _float4 At, _fvector vUp = XMVectorSet(0.f,1.f,0.f,0.f));
	_fmatrix MakeViewMatrixByUp(_fvector vEye, _fvector vAt, _fvector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f));
	_fmatrix MakeLerpMatrix(_fmatrix matDst, _fmatrix matSour, _float fTime);


private:
	_bool m_bStart = false;
	_float m_fMouseRevSpeed[Rev_End] = { 2.5f,2.5f };
	_float m_fMouseRev[Rev_End] = { 0.0f,0.0f };
	_float m_fCurMouseRev[Rev_End] = { 0.f,0.f };

	CamMode m_eCurCamMode = CamMode::Cam_End;

	_float4x4 m_matBeginWorld;	//카메라 이펙트등등 tick돌아갈때쓸것 
	_float4x4 m_matCurWorld;	//연출후 돌아오기

	//if Free -> AutotoFree  m_fChangeCamModeTime=0.f
	_float m_fChangeCamModeTime = 0.f;
	_float m_fChangeCamModeLerpSpeed = 1.f;

	//For.PlayerSize
	_float4 m_vSizeEye[CCody::PLAYER_SIZE::SIZE_END];
	_float4 m_vSizeAt[CCody::PLAYER_SIZE::SIZE_END];

	CCody::PLAYER_SIZE m_eCurPlayerSize;
	CCody::PLAYER_SIZE m_ePrePlayerSize;

	_double m_dLerpToCurSizeTime = 0.f;
	_bool	m_bIsLerpToCurSize = false;


	CamFreeOption m_eCurCamFreeOption = CamFreeOption::Cam_Free_FollowPlayer;
	CamFreeOption m_ePreCamFreeOption = CamFreeOption::Cam_Free_FollowPlayer;

	//For.SoftMove
	_float4 m_vPlayerPos = { 0.f,0.f,0.f,1.f };
	_float4		m_vPlayerUp = { 0.f,1.f,0.f,0.f };

	//For.Zoom
	_float		m_fCamZoomVal = 0.f;
	//For.Portal
	_double m_dWarpTime = 0.0;
	_float4x4 m_matStartPortal;

	//For.BridgeUppend
	_bool	m_bStartBridgeUppendCam = false;
	_float	m_fBridgeUppendTime = 0.f;

	//For.RayCast
	PxRaycastBuffer m_RayCastBuffer;
	WORLDMATRIX	m_PreWorld;
	//For.OnRail
	vector<CFilm::CamNode*> m_CamNodes;
	_uint m_iNodeIdx[CFilm::BezierNode::Bezier_End] = { 0,0,0 };
private:
	CCody*			m_pCody = nullptr;
	CCam_Helper*	m_pCamHelper = nullptr;
public:
	static CMainCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr) override;
	virtual void Free() override;
};

END