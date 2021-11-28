#pragma once

#include "Component_Manager.h"
#include "Frustum.h"
#include "GameObject_Manager.h"
#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Light_Manager.h"
#include "Pipeline.h"
#include "PhysX.h"
#include "RenderTarget_Manager.h"
#include "Sound_Manager.h"
#include "Timer_Manager.h"
#include "Shadow_Manager.h"
#include "PostFX.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	explicit CGameInstance();
	virtual ~CGameInstance() = default;

public:
#pragma region GameInstance
	HRESULT	Initialize(CGraphic_Device::WINMODE eWinMode, HWND hWnd, HINSTANCE hInst, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice = nullptr, ID3D11DeviceContext** ppDeviceContext = nullptr, PxSimulationEventCallback* pEventCallback = nullptr);
	HRESULT	Reserve_Container(_uint iLevelCount);
	_int	Tick(_double dTimeDelta);
	void	Clear_LevelResources(_uint iLevelIndex);
	void	Clear_LayerResources(_uint iLevelIndex, const _tchar* pLayerTag);
#pragma endregion

#pragma region Graphic_Device
	const D3D11_VIEWPORT	Get_ViewportInfo(_uint iViewportIndex) const;
	const _float4			Get_ViewportUVInfo(_uint iViewportIndex) const;
	void	Set_ViewportInfo(_fvector vMainViewportInfo, _fvector vSubViewportInfo);
	void	Set_GoalViewportInfo(_fvector vMainViewportInfo, _fvector vSubViewportInfo, _float fLerpSpeed = 1.f);
	HRESULT	Clear_BackBuffer(_float4 vClearColor);
	HRESULT	Clear_DepthStencilBuffer();
	HRESULT	Present();
#pragma endregion

#pragma region Input_Device
	_bool	Key_Up(_ubyte byKeyID);
	_bool	Key_Down(_ubyte byKeyID);
	_bool	Key_Pressing(_ubyte byKeyID);
	_bool	Mouse_Up(CInput_Device::MOUSE_KEYSTATE eMouseKeyState);
	_bool	Mouse_Down(CInput_Device::MOUSE_KEYSTATE eMouseKeyState);
	_bool	Mouse_Pressing(CInput_Device::MOUSE_KEYSTATE eMouseKeyState);
	_long	Mouse_Move(CInput_Device::MOUSE_MOVESTATE eMouseMoveState);
	_bool	Pad_Key_Up(_ubyte byPadKeyID);
	_bool	Pad_Key_Down(_ubyte byPadKeyID);
	_bool	Pad_Key_Pressing(_ubyte byPadKeyID);
	_long	Get_Pad_LStickX();
	_long	Get_Pad_LStickY();
	_long	Get_Pad_RStickX();
	_long	Get_Pad_RStickY();
#pragma endregion

#pragma region Timer_Manager
	HRESULT	Add_Timer(const _tchar* pTimerTag);
	_double	Compute_TimeDelta(const _tchar* pTimerTag);
#pragma endregion

#pragma region Sound_Manager
	void Play_Sound(TCHAR* pSoundKey, CHANNEL_TYPE eChannel, _float fVolume = 0.5f);
	void Play_BGM(TCHAR* pSoundKey, CHANNEL_TYPE eChannel);
	void Stop_Sound(CHANNEL_TYPE eChannel);
	void Stop_SoundAll();
	void Set_SoundVolume(CHANNEL_TYPE eChannel, _float fVolume);
#pragma endregion

#pragma region Level_Manager
	HRESULT	Change_CurrentLevel(class CLevel* pCurrentLevel);
	HRESULT	Render_Level();
#pragma endregion

#pragma region GameObject_Manager
	HRESULT	Add_GameObject_Prototype(_uint iPrototypeLevelIndex, const _tchar* pPrototypeTag, CGameObject* pPrototype);
	HRESULT	Add_GameObject_Clone(_uint iLayerLevelIndex, const _tchar* pLayerTag, _uint iPrototypeLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr, CGameObject** ppOut = nullptr);
	void	Bring_LayerGameObjects(_uint iLayerLevelIndex, const _tchar* pLayerTag, list<CGameObject*>& GameObjectList);
#pragma endregion

#pragma region Component_Manager
	HRESULT		Add_Component_Prototype(_uint iPrototypeLevelIndex, const _tchar* pPrototypeTag, CComponent* pComponent);
	CComponent*	Add_Component_Clone(_uint iPrototypeLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region Light_Manager
	HRESULT		Reserve_Container_Light(_uint iCount);
	HRESULT		Add_Light(const _tchar * pLightTag, const LIGHT_DESC& LightDesc, _bool isActive = true);
	LIGHT_DESC*	Get_LightDescPtr(const _tchar * pLightTag);
	HRESULT		TurnOn_Light(const _tchar * pLightTag);
	HRESULT		TurnOff_Light(const _tchar * pLightTag);
	void		Clear_Lights();
#pragma endregion

#pragma region PhysX
	PxMaterial*		Get_BasePxMaterial();
	PxTriangleMesh*	Create_PxMesh(MESHACTOR_DESC pMeshActorDesc);
	_bool			Raycast(const PxVec3& origin, const PxVec3& unitDir, const PxReal distance, PxRaycastCallback& hitCall, PxHitFlags hitFlags = PxHitFlags(PxHitFlag::eDEFAULT), const PxQueryFilterData& filterData = PxQueryFilterData(), PxQueryFilterCallback* filterCall = NULL, const PxQueryCache* cache = NULL);
#pragma endregion

#pragma region Pipeline_Manager
	_fmatrix		Get_Transform(CPipeline::TRANSFORM_STATETYPE eType) const;
	_fvector		Get_MainCamPosition() const;
	_fvector		Get_SubCamPosition() const;
	const _float	Get_MainCamFar() const;
	const _float	Get_SubCamFar() const;
#pragma endregion

#pragma region Frustum
	_bool	IsIn_WorldSpace_Main(_fvector vPosition, _float fRadius);
	_bool	IsIn_WorldSpace_Sub(_fvector vPosition, _float fRadius);
	_bool	IsIn_LocalSpace_Main(_fvector vPosition, _float fRadius);
	_bool	IsIn_LocalSpace_Sub(_fvector vPosition, _float fRadius);
#pragma endregion

private:
	CGraphic_Device*		m_pGraphic_Device		= nullptr;
	CInput_Device*			m_pInput_Device			= nullptr;
	CTimer_Manager*			m_pTimer_Manager		= nullptr;
	CSound_Manager*			m_pSound_Manager		= nullptr;
	CLevel_Manager*			m_pLevel_Manager		= nullptr;
	CGameObject_Manager*	m_pGameObject_Manager	= nullptr;
	CComponent_Manager*		m_pComponent_Manager	= nullptr;
	CLight_Manager*			m_pLight_Manager		= nullptr;
	CShadow_Manager*		m_pShadow_Manager		= nullptr;
	CPhysX*					m_pPhysX				= nullptr;
	CPipeline*				m_pPipeline				= nullptr;
	CFrustum*				m_pFrustum				= nullptr;
	CPostFX*				m_pPostFX				= nullptr;

public:
	static void Release_Engine();
	virtual void Free() override;
};

END