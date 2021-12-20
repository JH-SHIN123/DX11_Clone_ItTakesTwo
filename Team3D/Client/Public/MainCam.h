#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "Cody.h"

BEGIN(Client)

#define DISTANCE_PLAYER_TO_CAM 10.f

class CMainCam final : public CCamera
{
private:
	explicit CMainCam(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMainCam(const CMainCam& rhs);
	virtual ~CMainCam() = default;

public: /* Enum */
	enum CAM_REV		{ REV_HORIZONTAL, REV_VERTICAL, REV_END };
	enum CAM_MODE		{ CAM_FREE, CAM_MOVETOFREE, CAM_PRESSBUTTON_BRIDGE, CAM_WARP_WORMHOLE, CAM_END };
	enum CAM_FREEOPTION	{ OPTION_FOLLOWING_PLAYER, OPTION_ON_RAIL, OPTION_BOSS_MINIROOM, OPTION_END };

public: /* Getter */
	CTransform* Get_Transform() { return m_pTransformCom; }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	//HRESULT			Start_Film(const _tchar* pFilmTag);

private:
	/* For.Component */
	class CCam_Helper*	m_pCamHelper = nullptr;
	/* For.Target */
	CCody*				m_pCody = nullptr;
	CCody::PLAYER_SIZE	m_eCurCodySize = CCody::PLAYER_SIZE::SIZE_MEDIUM;
	CCody::PLAYER_SIZE	m_ePreCodySize = CCody::PLAYER_SIZE::SIZE_MEDIUM;
	_float4				m_vDirAccordingToCodySize[CCody::PLAYER_SIZE::SIZE_END];
	/* For.CameraOption */
	CAM_MODE			m_eCamMode = CAM_MODE::CAM_END;
	CAM_FREEOPTION		m_ePreCamFreeOption = CAM_FREEOPTION::OPTION_END;
	CAM_FREEOPTION		m_eCurCamFreeOption = CAM_FREEOPTION::OPTION_END;
	/* For.MoveToFree */
	_float				m_fChangeCamModeTime = 0.f;
	_float				m_fChangeCamModeLerpSpeed = 0.f;
	_float4x4			m_MatrixBuffer_MoveToFree;
	/* For.Raycast */
	PxRaycastBuffer		m_RaycastBuffer;
	/* For.DirectionInfo */
	_float4				m_vBaseDir;
	_float4				m_vTargetDir;
	_bool				m_bLerpDir = false;	
	_double				m_dDirLerpTime = 0.0;
	
	_float4				m_vQuat = _float4(0.f, 0.f, 0.f, 0.f);
	_float4				m_vRotatedDir = _float4(0.f, 0.f, 0.f, 0.f);


private:
	void	Set_CamMode_MoveToFree();
	void	Update_ByPlayerState(_double dTimeDelta);										// 플레이어 상태 체크
	_bool	DirLerp(_double dTimeDelta);
private:
	/* For.Update_CamHelper */
	HRESULT	Update_CamHelper(_double dTimeDelta);
	HRESULT Update_CamHelper_None(_double dTimeDelta);					// 현재 아무것도 재생안함
	HRESULT Update_CamHelper_Act(_double dTimeDelta);					// 재생중
	HRESULT Update_CamHelper_SeeCamNode(_double dTimeDelta);			// 카메라노드를 처다봄
	/* For.Update_Cam_ByMode */
	HRESULT	Update_Cam_ByMode(_double dTimeDelta);
	HRESULT	Update_Cam_ByMode_Free(_double dTimeDelta);					// CAMMODE::CAM_FREE
	HRESULT	Update_Cam_ByMode_MoveToFree(_double dTimeDelta);			// CAMMODE::CAM_MOVETOFREE
	HRESULT	Update_Cam_ByMode_PressBridgeButton(_double dTimeDelta);	// CAMMODE::CAM_PRESSBUTTON_BRIDGE
	HRESULT	Update_Cam_ByMode_WarpWormhole(_double dTimeDelta);			// CAMMODE::CAM_WARP_WORMHOLE
	/* For.Update_Cam_ByOption */
	HRESULT Update_Cam_ByOption_FollowingPlayer(_double dTimeDelta);	// CAM_FREEOPTION::OPTION_FOLLOWING_PLAYER
	HRESULT Update_Cam_ByOption_OnRail(_double dTimeDelta);				// CAM_FREEOPTION::OPTION_ON_RAIL
	HRESULT Update_Cam_ByOption_BossMiniRoom(_double dTimeDelta);		// CAM_FREEOPTION::OPTION_BOSS_MINIROOM


public:
	static CMainCam* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr) override;
	virtual void Free() override;
};

END