#include "..\public\GameInstance.h"
#include "Model_Loader.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device		(CGraphic_Device::GetInstance())
	, m_pInput_Device		(CInput_Device::GetInstance())
	, m_pTimer_Manager		(CTimer_Manager::GetInstance())
	, m_pSound_Manager		(CSound_Manager::GetInstance())
	, m_pLevel_Manager		(CLevel_Manager::GetInstance())
	, m_pGameObject_Manager	(CGameObject_Manager::GetInstance())
	, m_pComponent_Manager	(CComponent_Manager::GetInstance())
	, m_pLight_Manager		(CLight_Manager::GetInstance())
	, m_pPhysX				(CPhysX::GetInstance())
	, m_pPipeline			(CPipeline::GetInstance())
	, m_pFrustum			(CFrustum::GetInstance())
	, m_pShadow_Manager		(CShadow_Manager::GetInstance())
	, m_pPostFX				(CPostFX::GetInstance())
	, m_pBlur				(CBlur::GetInstance())
	, m_pSSAO				(CSSAO::GetInstance())
{
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pSound_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pGameObject_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPhysX);
	Safe_AddRef(m_pPipeline);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pShadow_Manager);
	Safe_AddRef(m_pPostFX);
	Safe_AddRef(m_pBlur);
	Safe_AddRef(m_pSSAO);
}

#pragma region GameInstance
HRESULT CGameInstance::Initialize(CGraphic_Device::WINMODE eWinMode, HWND hWnd, HINSTANCE hInst, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContext, PxSimulationEventCallback* pEventCallback)
{
	NULL_CHECK_RETURN(m_pGraphic_Device, E_FAIL);
	NULL_CHECK_RETURN(m_pInput_Device, E_FAIL);
	NULL_CHECK_RETURN(m_pTimer_Manager, E_FAIL);
	NULL_CHECK_RETURN(m_pSound_Manager, E_FAIL);
	NULL_CHECK_RETURN(m_pLight_Manager, E_FAIL);
	NULL_CHECK_RETURN(m_pPhysX, E_FAIL);
	NULL_CHECK_RETURN(m_pFrustum, E_FAIL);
	NULL_CHECK_RETURN(m_pShadow_Manager, E_FAIL);
	NULL_CHECK_RETURN(m_pPostFX, E_FAIL);
	NULL_CHECK_RETURN(m_pBlur, E_FAIL);
	NULL_CHECK_RETURN(m_pSSAO, E_FAIL);

	FAILED_CHECK_RETURN(m_pGraphic_Device->Ready_GraphicDevice(eWinMode, hWnd, iWinSizeX, iWinSizeY, ppDevice, ppDeviceContext), E_FAIL);
	FAILED_CHECK_RETURN(m_pInput_Device->Ready_InputDevice(hInst, hWnd), E_FAIL);
	//FAILED_CHECK_RETURN(m_pSound_Manager->Ready_SoundManager(), E_FAIL);
	FAILED_CHECK_RETURN(m_pLight_Manager->Ready_LightManager(*ppDevice, *ppDeviceContext, (_float)iWinSizeX, (_float)iWinSizeY), E_FAIL);
	FAILED_CHECK_RETURN(m_pPhysX->Ready_PhysX(pEventCallback), E_FAIL);
	FAILED_CHECK_RETURN(m_pFrustum->Ready_Frustum(), E_FAIL);
	FAILED_CHECK_RETURN(m_pShadow_Manager->Ready_ShadowManager(*ppDevice, *ppDeviceContext), E_FAIL);
	FAILED_CHECK_RETURN(m_pPostFX->Ready_PostFX(*ppDevice, *ppDeviceContext, (_float)iWinSizeX, (_float)iWinSizeY), E_FAIL);
	FAILED_CHECK_RETURN(m_pBlur->Ready_Blur(*ppDevice, *ppDeviceContext, (_float)iWinSizeX, (_float)iWinSizeY), E_FAIL);
	FAILED_CHECK_RETURN(m_pSSAO->Ready_SSAO(*ppDevice, *ppDeviceContext, (_float)iWinSizeX, (_float)iWinSizeY), E_FAIL);

	return S_OK;
}
HRESULT CGameInstance::Reserve_Container(_uint iLevelCount)
{
	NULL_CHECK_RETURN(m_pGameObject_Manager, E_FAIL);
	NULL_CHECK_RETURN(m_pComponent_Manager, E_FAIL);

	FAILED_CHECK_RETURN(m_pGameObject_Manager->Reserve_Container(iLevelCount), E_FAIL);
	FAILED_CHECK_RETURN(m_pComponent_Manager->Reserve_Container(iLevelCount), E_FAIL);

	return S_OK;
}
_int CGameInstance::Tick(_double dTimeDelta, _bool bWndActivate)
{
	NULL_CHECK_RETURN(m_pGraphic_Device, EVENT_ERROR);
	NULL_CHECK_RETURN(m_pInput_Device, EVENT_ERROR);
	NULL_CHECK_RETURN(m_pSound_Manager, EVENT_ERROR);
	NULL_CHECK_RETURN(m_pLevel_Manager, EVENT_ERROR);
	NULL_CHECK_RETURN(m_pGameObject_Manager, EVENT_ERROR);
	NULL_CHECK_RETURN(m_pFrustum, EVENT_ERROR);

	m_pGraphic_Device->Tick(dTimeDelta);
	m_pInput_Device->Tick(bWndActivate);
	m_pPhysX->Tick();

	if (m_pGameObject_Manager->Tick(dTimeDelta) < 0)
		return EVENT_ERROR;

	m_pFrustum->Transform_ToWorldSpace();
	
	if (m_pGameObject_Manager->Late_Tick(dTimeDelta) < 0)
		return EVENT_ERROR;

	/* Shadow View / Proj 생성 - FullScreen 기준 */
	m_pShadow_Manager->Update_CascadeShadowTransform(CShadow_Manager::SHADOW_MAIN);
	m_pShadow_Manager->Update_CascadeShadowTransform(CShadow_Manager::SHADOW_SUB);

	return m_pLevel_Manager->Tick(dTimeDelta);
}
void CGameInstance::Clear_LevelResources(_uint iLevelIndex)
{
	NULL_CHECK(m_pComponent_Manager);
	NULL_CHECK(m_pGameObject_Manager);

	m_pComponent_Manager->Clear(iLevelIndex);
	m_pGameObject_Manager->Clear(iLevelIndex);
}
void CGameInstance::Clear_LayerResources(_uint iLevelIndex, const _tchar * pLayerTag)
{
	NULL_CHECK(m_pGameObject_Manager);
	m_pGameObject_Manager->Clear_Layer(iLevelIndex, pLayerTag);
}
#pragma endregion

#pragma region Graphic_Device
const D3D11_VIEWPORT CGameInstance::Get_ViewportInfo(_uint iViewportIndex) const
{
	NULL_CHECK_RETURN(m_pGraphic_Device, D3D11_VIEWPORT());
	return m_pGraphic_Device->Get_ViewportInfo(iViewportIndex);
}
const _float4 CGameInstance::Get_ViewportUVInfo(_uint iViewportIndex) const
{
	NULL_CHECK_RETURN(m_pGraphic_Device, _float4());
	return m_pGraphic_Device->Get_ViewportUVInfo(iViewportIndex);
}
void CGameInstance::Set_ViewportInfo(_fvector vMainViewportInfo, _fvector vSubViewportInfo)
{
	NULL_CHECK(m_pGraphic_Device);
	m_pGraphic_Device->Set_ViewportInfo(vMainViewportInfo, vSubViewportInfo);
}
void CGameInstance::Set_GoalViewportInfo(_fvector vMainViewportInfo, _fvector vSubViewportInfo, _float fLerpSpeed)
{
	NULL_CHECK(m_pGraphic_Device);
	m_pGraphic_Device->Set_GoalViewportInfo(vMainViewportInfo, vSubViewportInfo, fLerpSpeed);
}
HRESULT CGameInstance::Clear_BackBuffer(_float4 vClearColor)
{
	NULL_CHECK_RETURN(m_pGraphic_Device, E_FAIL);
	return m_pGraphic_Device->Clear_BackBuffer(vClearColor);
}
HRESULT CGameInstance::Clear_DepthStencilBuffer()
{
	NULL_CHECK_RETURN(m_pGraphic_Device, E_FAIL);
	return m_pGraphic_Device->Clear_DepthStencilBuffer();
}
HRESULT CGameInstance::Present()
{
	NULL_CHECK_RETURN(m_pGraphic_Device, E_FAIL);
	return m_pGraphic_Device->Present();
}
#pragma endregion 

#pragma region Input_Device
_bool CGameInstance::Key_Up(_ubyte byKeyID)
{
	NULL_CHECK_RETURN(m_pInput_Device, false);
	return m_pInput_Device->Key_Up(byKeyID);
}
_bool CGameInstance::Key_Down(_ubyte byKeyID)
{
	NULL_CHECK_RETURN(m_pInput_Device, false);
	return m_pInput_Device->Key_Down(byKeyID);
}
_bool CGameInstance::Key_Pressing(_ubyte byKeyID)
{
	NULL_CHECK_RETURN(m_pInput_Device, false);
	return m_pInput_Device->Key_Pressing(byKeyID);
}

_bool CGameInstance::Mouse_Up(CInput_Device::MOUSE_KEYSTATE eMouseKeyState)
{
	NULL_CHECK_RETURN(m_pInput_Device, false);
	return m_pInput_Device->Mouse_Up(eMouseKeyState);
}

_bool CGameInstance::Mouse_Down(CInput_Device::MOUSE_KEYSTATE eMouseKeyState)
{
	NULL_CHECK_RETURN(m_pInput_Device, false);
	return m_pInput_Device->Mouse_Down(eMouseKeyState);
}

_bool CGameInstance::Mouse_Pressing(CInput_Device::MOUSE_KEYSTATE eMouseKeyState)
{
	NULL_CHECK_RETURN(m_pInput_Device, false);
	return m_pInput_Device->Mouse_Pressing(eMouseKeyState);
}
_long CGameInstance::Mouse_Move(CInput_Device::MOUSE_MOVESTATE eMouseMoveState)
{
	NULL_CHECK_RETURN(m_pInput_Device, 0);
	return m_pInput_Device->Mouse_Move(eMouseMoveState);
}
_bool CGameInstance::Pad_Key_Up(_ubyte byPadKeyID)
{
	NULL_CHECK_RETURN(m_pInput_Device, false);
	return m_pInput_Device->Pad_Key_Up(byPadKeyID);
}
_bool CGameInstance::Pad_Key_Down(_ubyte byPadKeyID)
{
	NULL_CHECK_RETURN(m_pInput_Device, false);
	return m_pInput_Device->Pad_Key_Down(byPadKeyID);
}
_bool CGameInstance::Pad_Key_Pressing(_ubyte byPadKeyID)
{
	NULL_CHECK_RETURN(m_pInput_Device, false);
	return m_pInput_Device->Pad_Key_Pressing(byPadKeyID);
}
_long CGameInstance::Get_Pad_LStickX()
{
	NULL_CHECK_RETURN(m_pInput_Device, 0);
	return m_pInput_Device->Get_Pad_LStickX();
}
_long CGameInstance::Get_Pad_LStickY()
{
	NULL_CHECK_RETURN(m_pInput_Device, 0);
	return m_pInput_Device->Get_Pad_LStickY();
}
_long CGameInstance::Get_Pad_RStickX()
{
	NULL_CHECK_RETURN(m_pInput_Device, 0);
	return m_pInput_Device->Get_Pad_RStickX();
}
_long CGameInstance::Get_Pad_RStickY()
{
	NULL_CHECK_RETURN(m_pInput_Device, 0);
	return m_pInput_Device->Get_Pad_RStickY();
}
#pragma endregion 

#pragma region Timer_Manager
HRESULT CGameInstance::Add_Timer(const _tchar * pTimerTag)
{
	NULL_CHECK_RETURN(m_pTimer_Manager, E_FAIL);
	return m_pTimer_Manager->Add_Timer(pTimerTag);
}
_double CGameInstance::Compute_TimeDelta(const _tchar * pTimerTag)
{
	NULL_CHECK_RETURN(m_pTimer_Manager, 0.0);
	return m_pTimer_Manager->Compute_TimeDelta(pTimerTag);
}
#pragma endregion 

#pragma region Sound_Manager
void CGameInstance::Play_Sound(TCHAR * pSoundKey, CHANNEL_TYPE eChannel, _float fVolume)
{
	NULL_CHECK(m_pSound_Manager);
	m_pSound_Manager->Play_Sound(pSoundKey, eChannel, fVolume);
}
void CGameInstance::Play_BGM(TCHAR * pSoundKey, CHANNEL_TYPE eChannel)
{
	NULL_CHECK(m_pSound_Manager);
	m_pSound_Manager->Play_BGM(pSoundKey, eChannel);
}
void CGameInstance::Stop_Sound(CHANNEL_TYPE eChannel)
{
	NULL_CHECK(m_pSound_Manager);
	m_pSound_Manager->Stop_Sound(eChannel);
}
void CGameInstance::Stop_SoundAll()
{
	NULL_CHECK(m_pSound_Manager);
	m_pSound_Manager->Stop_SoundAll();
}
void CGameInstance::Set_SoundVolume(CHANNEL_TYPE eChannel, _float fVolume)
{
	NULL_CHECK(m_pSound_Manager);
	m_pSound_Manager->Set_SoundVolume(eChannel, fVolume);
}
#pragma endregion 

#pragma region Level_Manager
HRESULT CGameInstance::Change_CurrentLevel(CLevel * pCurrentLevel)
{
	NULL_CHECK_RETURN(m_pLevel_Manager, E_FAIL);
	return m_pLevel_Manager->Change_CurrentLevel(pCurrentLevel);
}
HRESULT CGameInstance::Render_Level()
{
	NULL_CHECK_RETURN(m_pLevel_Manager, E_FAIL);
	return m_pLevel_Manager->Render();
}
#pragma endregion 

#pragma region GameObjcet_Manager
HRESULT CGameInstance::Add_GameObject_Prototype(_uint iPrototypeLevelIndex, const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	NULL_CHECK_RETURN(m_pGameObject_Manager, E_FAIL);
	return m_pGameObject_Manager->Add_GameObject_Prototype(iPrototypeLevelIndex, pPrototypeTag, pPrototype);
}
HRESULT CGameInstance::Add_GameObject_Clone(_uint iLayerLevelIndex, const _tchar * pLayerTag, _uint iPrototypeLevelIndex, const _tchar * pPrototypeTag, void * pArg, CGameObject** ppOut)
{
	NULL_CHECK_RETURN(m_pGameObject_Manager, E_FAIL);
	return m_pGameObject_Manager->Add_GameObject_Clone(iLayerLevelIndex, pLayerTag, iPrototypeLevelIndex, pPrototypeTag, pArg, ppOut);
}
void CGameInstance::Bring_LayerGameObjects(_uint iLayerLevelIndex, const _tchar * pLayerTag, list<CGameObject*>& GameObjectList)
{
	NULL_CHECK(m_pGameObject_Manager);
	m_pGameObject_Manager->Bring_LayerGameObjects(iLayerLevelIndex, pLayerTag, GameObjectList);
}
#pragma endregion 

#pragma region Component_Manager
HRESULT CGameInstance::Add_Component_Prototype(_uint iPrototypeLevelIndex, const _tchar * pPrototypeTag, CComponent * pComponent)
{
	NULL_CHECK_RETURN(m_pComponent_Manager, E_FAIL);
	return m_pComponent_Manager->Add_Component_Prototype(iPrototypeLevelIndex, pPrototypeTag, pComponent);
}
CComponent * CGameInstance::Add_Component_Clone(_uint iPrototypeLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	NULL_CHECK_RETURN(m_pComponent_Manager, nullptr);
	return m_pComponent_Manager->Add_Component_Clone(iPrototypeLevelIndex, pPrototypeTag, pArg);
}
#pragma endregion 

#pragma region Light_Manager
HRESULT CGameInstance::Reserve_Container_Light(_uint iCount)
{
	NULL_CHECK_RETURN(m_pLight_Manager, E_FAIL);
	return m_pLight_Manager->Reserve_Container(iCount);
}
HRESULT CGameInstance::Add_Light(const _tchar* pLightTag, const LIGHT_DESC & LightDesc, _bool isActive)
{
	NULL_CHECK_RETURN(m_pLight_Manager, E_FAIL);
	return m_pLight_Manager->Add_Light(pLightTag, LightDesc, isActive);
}
LIGHT_DESC * CGameInstance::Get_LightDescPtr(const _tchar* pLightTag)
{
	NULL_CHECK_RETURN(m_pLight_Manager, nullptr);
	return m_pLight_Manager->Get_LightDescPtr(pLightTag);
}
HRESULT CGameInstance::TurnOn_Light(const _tchar* pLightTag)
{
	NULL_CHECK_RETURN(m_pLight_Manager, E_FAIL);
	return m_pLight_Manager->TurnOn_Light(pLightTag);
}
HRESULT CGameInstance::TurnOff_Light(const _tchar* pLightTag)
{
	NULL_CHECK_RETURN(m_pLight_Manager, E_FAIL);
	return m_pLight_Manager->TurnOff_Light(pLightTag);
}
void CGameInstance::Clear_Lights()
{
	NULL_CHECK(m_pLight_Manager);
	m_pLight_Manager->Clear_Lights();
}
#pragma endregion

#pragma region PhysX
PxMaterial * CGameInstance::Get_BasePxMaterial()
{
	NULL_CHECK_RETURN(m_pPhysX, nullptr);
	return m_pPhysX->Get_BaseMaterial();
}
PxTriangleMesh * CGameInstance::Create_PxMesh(MESHACTOR_DESC pMeshActorDesc)
{
	NULL_CHECK_RETURN(m_pPhysX, nullptr);
	return m_pPhysX->Create_Mesh(pMeshActorDesc);
}
_bool CGameInstance::Raycast(const PxVec3 & origin, const PxVec3 & unitDir, const PxReal distance, PxRaycastCallback & hitCall, PxHitFlags hitFlags, const PxQueryFilterData & filterData, PxQueryFilterCallback * filterCall, const PxQueryCache * cache)
{
	NULL_CHECK_RETURN(m_pPhysX, false);
	return m_pPhysX->Raycast(origin, unitDir, distance, hitCall, hitFlags, filterData, filterCall, cache);
}
#pragma endregion 

#pragma region Pipeline_Manager
_fmatrix CGameInstance::Get_Transform(CPipeline::TRANSFORM_STATETYPE eType) const
{
	NULL_CHECK_RETURN(m_pPipeline, XMMatrixIdentity());
	return m_pPipeline->Get_Transform(eType);
}
_fvector CGameInstance::Get_MainCamPosition() const
{
	NULL_CHECK_RETURN(m_pPipeline, XMVectorZero());
	return m_pPipeline->Get_MainCamPosition();
}
_fvector CGameInstance::Get_SubCamPosition() const
{
	NULL_CHECK_RETURN(m_pPipeline, XMVectorZero());
	return m_pPipeline->Get_SubCamPosition();
}
const _float CGameInstance::Get_MainCamFar() const
{
	NULL_CHECK_RETURN(m_pPipeline, 0.f);
	return m_pPipeline->Get_MainCamFar();
}
const _float CGameInstance::Get_SubCamFar() const
{
	NULL_CHECK_RETURN(m_pPipeline, 0.f);
	return m_pPipeline->Get_SubCamFar();
}
#pragma endregion

#pragma region Frustum
_bool CGameInstance::IsIn_WorldSpace_Main(_fvector vPosition, _float fRadius)
{
	NULL_CHECK_RETURN(m_pFrustum, false);
	return m_pFrustum->IsIn_WorldSpace_Main(vPosition, fRadius);
}
_bool CGameInstance::IsIn_WorldSpace_Sub(_fvector vPosition, _float fRadius)
{
	NULL_CHECK_RETURN(m_pFrustum, false);
	return m_pFrustum->IsIn_WorldSpace_Sub(vPosition, fRadius);
}
_bool CGameInstance::IsIn_LocalSpace_Main(_fvector vPosition, _float fRadius)
{
	NULL_CHECK_RETURN(m_pFrustum, false);
	return m_pFrustum->IsIn_LocalSpace_Main(vPosition, fRadius);
}
_bool CGameInstance::IsIn_LocalSpace_Sub(_fvector vPosition, _float fRadius)
{
	NULL_CHECK_RETURN(m_pFrustum, false);
	return m_pFrustum->IsIn_LocalSpace_Sub(vPosition, fRadius);
}
#pragma endregion

void CGameInstance::Release_Engine()
{
	CGameObject_Manager::GetInstance()->Clear_All();
	CComponent_Manager::GetInstance()->Clear_All();
	CLight_Manager::GetInstance()->Clear_Buffer();
	CLevel_Manager::GetInstance()->Clear_Level();
	CPostFX::GetInstance()->Clear_Buffer();

#ifdef _DEBUG
	CSSAO::GetInstance()->Clear_Buffer();
	CBlur::GetInstance()->Clear_Buffer();
	CRenderTarget_Manager::GetInstance()->Clear_Buffers();
#endif

	if (CGameInstance::GetInstance()->DestroyInstance())
		MSG_BOX("Failed to Release CGameInstance.");
	if (CLevel_Manager::DestroyInstance())
		MSG_BOX("Failed to Release CLevel_Manager.");
	if (CGameObject_Manager::DestroyInstance())
		MSG_BOX("Failed to Release CGameObject_Manager.");
	if (CComponent_Manager::DestroyInstance())
		MSG_BOX("Failed to Release CComponent_Manager.");
	if (CModel_Loader::DestroyInstance())
		MSG_BOX("Failed to Release CModel_Loader.");
	if (CShadow_Manager::DestroyInstance())
		MSG_BOX("Failed to Release CShadow_Manager.");
	if (CLight_Manager::DestroyInstance())
		MSG_BOX("Failed to Release CLight_Manager.");
	if (CSSAO::DestroyInstance())
		MSG_BOX("Failed to Release CSSAO.");
	if (CPostFX::DestroyInstance())
		MSG_BOX("Failed to Release CPostFX.");
	if (CBlur::DestroyInstance())
		MSG_BOX("Failed to Release CBlur.");
	if (CPhysX::DestroyInstance())
		MSG_BOX("Failed to Release CPhysX.");
	if (CRenderTarget_Manager::DestroyInstance())
		MSG_BOX("Failed to Release CRenderTarget_Manager.");
	if (CFrustum::DestroyInstance())
		MSG_BOX("Failed to Release CFrustum.");
	if (CPipeline::DestroyInstance())
		MSG_BOX("Failed to Release CPipeline.");
	if (CSound_Manager::DestroyInstance())
		MSG_BOX("Failed to Release CSound_Manager.");
	if (CTimer_Manager::DestroyInstance())
		MSG_BOX("Failed to Release CTimer_Manager.");
	if (CInput_Device::DestroyInstance())
		MSG_BOX("Failed to Release CInput_Device.");
	if (CGraphic_Device::DestroyInstance())
		MSG_BOX("Failed to Release CGraphic_Device.");
}

void CGameInstance::Free()
{
	Safe_Release(m_pSSAO);
	Safe_Release(m_pBlur);
	Safe_Release(m_pPostFX);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pShadow_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPhysX);
	Safe_Release(m_pGameObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pPipeline);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
}
