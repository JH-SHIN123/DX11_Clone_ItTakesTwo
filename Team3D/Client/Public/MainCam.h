#pragma once

//#include "Client_Defines.h"
//#include "Camera.h"
//#include "Cam_Helper.h"
//#include "Cody.h"
//
//BEGIN(Client)
//
//class CMainCam final : public CCamera
//{
//private:
//	explicit CMainCam(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
//	explicit CMainCam(const CMainCam& rhs);
//	virtual ~CMainCam() = default;
//
//public: /* Enum */
//	enum CamRev			{ Rev_Holizontal, Rev_Prependicul, Rev_End };
//	enum CamMode		{ Cam_Free, Cam_AutoToFree, Cam_End };
//	enum CamFreeOption	{ Cam_Free_FollowPlayer, Cam_Free_FreeMove, Cam_Free_OnRail, Cam_Free_Warp_WormHole, Cam_Free_End };
//
//public: /* Getter */
//	CTransform* Get_Transform() { return m_pTransformCom; }
//
//public:
//	virtual HRESULT	NativeConstruct_Prototype() override;
//	virtual HRESULT NativeConstruct(void* pArg) override;
//	virtual _int	Tick(_double dTimeDelta) override;
//	virtual _int	Late_Tick(_double dTimeDelta) override;
//	HRESULT			Start_Film(const _tchar* pFilmTag);
//
//private:
//	//For Free.
//	_int	Tick_Cam_Free(_double dTimeDelta);					//자유이동
//	_int	Tick_Cam_AutoToFree(_double dTimeDelta);			//연출 카메라 -> 자유이동시 보간
//
//	_int	Tick_Cam_Free_FollowPlayer(_double dTimeDelta);		//카메라가 플레이어를쫓아가며 이동(메인 카메라)
//	_int	Tick_Cam_Free_FreeMode(_double dTimeDelta);			//카메라가 자유롭게 이동함
//	_int	Tick_Cam_Free_OnRail(_double dTimeDelta);			//레일
//	_int	Tick_Cam_Free_Warp_WormHole(_double dTimeDelta);	//웜홀
//																//CamHelper State(현재 )
//	_int	Tick_CamHelperNone(_double dTimeDelta);			//현재 아무것도 재생안함
//	_int	Tick_CamHelper_Act(_double dTimeDelta);			//재생중
//	_int	Tick_CamHelper_SeeCamNode(_double dTimeDelta);	//카메라노드를 처다봄
//private:
//	void	Check_Player(_double dTimeDelta);
//	void	Set_Zoom(_float fZoomVal, _double dTimeDelta);
//	_bool	LerpToCurSize(CCody::PLAYER_SIZE eSize, _double dTimeDelta);
//};
//
