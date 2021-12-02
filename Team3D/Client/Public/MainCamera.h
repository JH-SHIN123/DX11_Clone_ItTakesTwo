#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include"Cam_Helper.h"
#include"Cody.h"

BEGIN(Client)
class CMainCamera final : public CCamera
{
	enum CamRev {Rev_Holizontal,Rev_Prependicul,Rev_End};

	enum class CamMode{Cam_Free,Cam_AutoToFree,Cam_End};
	//O CamFreeMove P FollowPlayer
	enum class CamFreeOption { Cam_Free_FollowPlayer, Cam_Free_FreeMove, Cam_Free_End };

private:
	explicit CMainCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMainCamera(const CMainCamera& rhs);
	virtual ~CMainCamera() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;

public:
	static CMainCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr) override;
	virtual void Free() override;

	void Check_Player(_double dTimeDelta);

	HRESULT Start_Film(const _tchar* pFilmTag);
private:
	class CCameraActor* m_pActorCom = nullptr;
	CGameObject* m_pTargetObj = nullptr;
	CCam_Helper* m_pCamHelper = nullptr;
private:
	//For Free.
	_int	Tick_Cam_Free(_double dTimeDelta);				//자유이동
	_int	Tick_Cam_AutoToFree(_double dTimeDelta);		//연출 카메라 -> 자유이동시 보간
	_int	Tick_Cam_Free_FollowPlayer(_double dTimeDelta);	//카메라가 플레이어를쫓아가며 이동(메인 카메라)
	_int	Tick_Cam_Free_FreeMode(_double dTimeDelta);		//카메라가 자유롭게 이동함

	//CamHelper State(현재 )
	_int	Tick_CamHelperNone(_double dTimeDelta);			//현재 아무것도 재생안함
	_int	Tick_CamHelper_Act(_double dTimeDelta);			//재생중
	_int	Tick_CamHelper_SeeCamNode(_double dTimeDelta);	//카메라노드를 처다봄


	//For.Debug
	void KeyCheck(_double dTimeDelta);
private:
	_int	ReSet_Cam_FreeToAuto();		//변수 초기화용
	_bool	OffSetPhsX(_double dTimeDelta,_vector * pOut);
	_fmatrix MakeViewMatrix(_float3 Eye, _float3 At, _float3 vAxisY);
	_fvector MakeBlockPos(PxRaycastBuffer tBuffer);
public:
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	_bool m_bStart = false;
	_float m_fMouseRevSpeed[Rev_End] = { 2.5f,2.5f };
	_float m_fMouseRev[Rev_End] = { 0.0f,0.0f };
	_float m_fCurMouseRev[Rev_End] = { 0.f,0.f };

	CamMode m_eCurCamMode = CamMode::Cam_End;

	_float4x4 m_matBeginWorld;
	_float4x4 m_matCurWorld;
	_float4x4 m_matPreRev;

	//if Free -> AutotoFree  m_fChangeCamModeTime=0.f
	_float m_fChangeCamModeTime = 0.f;
	_float m_fChangeCamModeLerpSpeed = 1.f;

	CCody::PLAYER_SIZE m_eCurPlayerSize;
	CCody::PLAYER_SIZE m_ePrePlayerSize;


	_float4x4	m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_END];

	CamFreeOption m_eCurCamFreeOption = CamFreeOption::Cam_Free_FollowPlayer;

	//For.SpringCamera
	_bool m_bIsCollision = false;
	_float4x4 m_matBeforeSpringCam;
	//For.SoftMove
	_float3 m_vPlayerPos = { 0.f,0.f,0.f };
	//회전 보간용
	_float4x4 m_matQuternionRev;
	//플레이어 업 체크용
	_float m_fTargetCalculateUp = 0.f;
	_float m_fCurCalculateUp = 0.f;

	_float m_fTargetCalculateRight = 0.f;
	_float m_fCurCalculateRight = 0.f;

	_float m_fTargetCalculateLook = 0.f;
	_float m_fCurCalculateLook = 0.f;



};

END