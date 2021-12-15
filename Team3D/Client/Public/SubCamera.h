#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "Cam_Helper.h"

BEGIN(Client)

class CCameraActor;
class CSubCamera final : public CCamera
{
	enum CamRev { Rev_Holizontal, Rev_Prependicul, Rev_End };
	enum CamMode { Cam_Free, Cam_Auto, Cam_FreeToAuto, Cam_AutoToFree, Cam_End };
	//O CamFreeMove P FollowPlayer
	enum class CamFreeOption { Cam_Free_FollowPlayer, Cam_Free_FreeMove, Cam_Free_RidingSpaceShip_May,Cam_Free_End };
private:
	explicit CSubCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSubCamera(const CSubCamera& rhs);
	virtual ~CSubCamera() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;


	CTransform* Get_Transform() { return m_pTransformCom; }
	CCam_Helper* Get_CamHelper() { return m_pCamHelper; }
	HRESULT Start_Film(const _tchar* pFilmTag);
private:
	void	Check_Player(_double dTimeDelta);
	void	Set_Zoom(_float4 vEye,_float4 vAt,_float fZoomVal,_double dTimeDelta);
private:
	class CMay* m_pMay = nullptr;
	CCam_Helper* m_pCamHelper = nullptr;
private:
	//For Free.
	_int	Tick_Cam_Free(_double dTimeDelta);				//자유이동
	_int	Tick_Cam_AutoToFree(_double dTimeDelta);		//연출 카메라 -> 자유이동시 보간

	
	_int	Tick_Cam_Free_FollowPlayer(_double dTimeDelta);		//카메라가 플레이어를쫓아가며 이동(메인 카메라)
	_int	Tick_Cam_Free_FreeMode(_double dTimeDelta);			//카메라가 자유롭게 이동함
	_int	Tick_Cam_Free_RideSpaceShip_May(_double dTimeDelta);	//우주선 탓을때

															//CamHelper State(현재 )
	_int	Tick_CamHelperNone(_double dTimeDelta);			//현재 아무것도 재생안함
	_int	Tick_CamHelper_Act(_double dTimeDelta);			//재생중
	_int	Tick_CamHelper_SeeCamNode(_double dTimeDelta);	//카메라노드를 처다봄


															//For.Debug
	
private:
	_int	ReSet_Cam_FreeToAuto();		//변수 초기화용
	_bool	OffSetPhsX(_fmatrix matWorld,_double dTimeDelta, _vector * pOut);

	_fmatrix MakeViewMatrixByUp(_float4 Eye, _float4 At,_fvector vUp);
	_fmatrix MakeViewMatrixByUp(_fvector vEye, _fvector vAt, _fvector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f));
	_fmatrix MakeLerpMatrix(_fmatrix matDst, _fmatrix matSour, _float fTime);

private:
	_bool m_bStart = false;
	_float m_fMouseRevSpeed[Rev_End] = { 2.5f,2.5f };
	_float m_fMouseRev[Rev_End] = { 0.0f,0.0f };
	_float m_fCurMouseRev[Rev_End] = { 0.f,0.f };

	CamMode m_eCurCamMode = CamMode::Cam_End;

	_float4x4 m_matBeginWorld;
	_float4x4 m_matCurWorld;

	_float4 m_vStartEye = { 0.f,0.f, 0.f, 1.f };
	_float4 m_vStartAt = { 0.f,0.f, 0.f, 1.f };


	//if Free -> AutotoFree  m_fChangeCamModeTime=0.f
	_float m_fChangeCamModeTime = 0.f;
	_float m_fChangeCamModeLerpSpeed = 1.f;

	CamFreeOption m_eCurCamFreeOption = CamFreeOption::Cam_Free_FollowPlayer;

	//For.SoftMove
	_float4		m_vPlayerPos = { 0.f,0.f,0.f,1.f };
	_float4		m_vPlayerUp = { 0.f,1.f,0.f,0.f };

	//For.Zoom
	_float m_fCamZoomVal = 0.f;

	WORLDMATRIX	m_PreWorld;
	//For.RidingUFO
	_float4 m_vStartRidingUFOEye = {0.f,0.f,0.f,1.f};
	_float4 m_vStartRidingUFOAt = { 0.f,0.f,0.f,1.f };
public:
	static CSubCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr) override;
	virtual void Free() override;

};

END