#pragma once

#include "Client_Defines.h"
#include "Camera.h"



BEGIN(Client)

class CMainCamera final : public CCamera
{
	enum CamRev {Rev_Holizontal,Rev_Prependicul,Rev_End};
	enum CamMode{Cam_Free,Cam_Auto,Cam_FreeToAuto,Cam_AutoToFree,Cam_End};

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


private:
	//return EventFlag
	_int	Tick_Cam_Free(_double dTimeDelta); 
	_int	Tick_Cam_Auto(_double dTimeDelta);
	_int	Tick_Cam_AutoToFree(_double dTimeDelta);
	_int	Tick_Cam_FreeToAuto(_double dTimeDelta);
private:
	CGameObject* m_pTargetObj = nullptr;
	_float m_fMouseRevSpeed[Rev_End] = { 2.5f,2.5f };
	_float m_fMouseRev[Rev_End] = { 0.0f,0.0f };
	_float m_fCamDist = 4.f;

	CamMode m_eCurCamMode = Cam_End;
	CamMode m_ePreCamMode = Cam_End;

	_float4x4 m_matBeginWorld;
	_float4x4 m_matPreRev;

	_float m_fChangeCamModeTime = 0.f;
};

END