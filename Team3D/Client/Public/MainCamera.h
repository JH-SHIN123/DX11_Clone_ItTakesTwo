#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include"Cam_Helper.h"
#include"Cody.h"
BEGIN(Engine)
class CControllableActor;
END

BEGIN(Client)

class CMainCamera final : public CCamera
{
	enum CamRev {Rev_Holizontal,Rev_Prependicul,Rev_End};

	enum class CamMode{Cam_Free,Cam_AutoToFree,Cam_End};

	enum class CamEffect{ CamEffect_None,CamEffect_Shake,CamEffect_End };

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


private:
	CControllableActor* m_pActorCom = nullptr;
	CCam_Helper* m_pCamHelper = nullptr;
private:
	//For Free.
	_int	Tick_Cam_Free(_double dTimeDelta);				//자유이동
	_int	Tick_Cam_AutoToFree(_double dTimeDelta);		//연출 카메라 -> 자유이동시 보간
	//CamHelper State(현재 )
	_int	Tick_CamHelperNone(_double dTimeDelta);			//현재 아무것도 재생안함
	_int	Tick_CamHelper_Act(_double dTimeDelta);			//재생중
	_int	Tick_CamHelper_SeeCamNode(_double dTimeDelta);	//카메라노드를 처다봄



private:
	_int	ReSet_Cam_FreeToAuto();		//변수 초기화용
	_bool	OffSetPhsX(_double dTimeDelta,_fmatrix matRev,_vector * pOut);
	_fmatrix MakeViewMatrix(_float3 Eye, _float3 At);
public:
	CTransform* Get_Transform() { return m_pTransformCom; }
private:
	void	StopCamEffect();
	void	StartCamEffect_Shake(_float fDecaysec);
	_fmatrix	Tick_CamEffect_ShakeCamera(_double dTimeDelta);
private:
	CGameObject* m_pTargetObj = nullptr;
	_float m_fMouseRevSpeed[Rev_End] = { 2.5f,2.5f };
	_float m_fMouseRev[Rev_End] = { 0.0f,0.0f };
	
	
	_float m_fCamDist = 4.f;


	CamMode m_eCurCamMode = CamMode::Cam_End;

	_float4x4 m_matBeginWorld;
	_float4x4 m_matCurWorld;
	_float4x4 m_matPreRev;

	//if Free -> AutotoFree  m_fChangeCamModeTime=0.f
	_float m_fChangeCamModeTime = 0.f;
	_float m_fChangeCamModeLerpSpeed = 1.f;

	CCody::PLAYER_SIZE m_eCurPlayerSize;
	CCody::PLAYER_SIZE m_ePrePlayerSize;

	_bool m_bPhsXCollision = false;

	_float4x4	m_matPlayerSizeOffSetMatrix[CCody::PLAYER_SIZE::SIZE_END];

	CamEffect	m_eCurCamEffect = CamEffect::CamEffect_End;
	_double		m_dCamEffectTime = 0.f;

	//For.CameraShake
	_float		m_fDecaysec = 0.f;

};

END