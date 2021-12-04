#include "stdafx.h"
#include "..\public\MainApp.h"
#include "Level_Loading.h"
#include "Effect_Generator.h"
#include "UI_Generator.h"
#include "Environment_Generator.h"
#include "PxEventCallback.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::NativeConstruct()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	m_pPxEventCallback = new CPxEventCallback;
	m_bMouseLock = true;

	FAILED_CHECK_RETURN(m_pGameInstance->Initialize(CGraphic_Device::TYPE_WINMODE, g_hWnd, g_hInst, g_iWinCX, g_iWinCY, &m_pDevice, &m_pDeviceContext, m_pPxEventCallback), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Reserve_Container(Level::LEVEL_END), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Timer(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype_ForStatic(), E_FAIL);
	UI_Generator->NativeConstruct(m_pDevice, m_pDeviceContext);
	FAILED_CHECK_RETURN(CEnvironment_Generator::GetInstance()->NativeConstruct_Environment_Generator(m_pDevice, m_pDeviceContext), E_FAIL);

	FAILED_CHECK_RETURN(Ready_DefaultLevel(Level::LEVEL_STAGE), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::Run_App()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	if (g_bWndActivate && m_bMouseLock)
		Lock_Mouse();

	m_dFrameAcc += m_pGameInstance->Compute_TimeDelta(TEXT("Timer_Default"));

	if (m_dFrameAcc >= 1.0 / 60.0)
	{
		m_dFrameAcc = 0.0;

		_double dTimeDelta = m_pGameInstance->Compute_TimeDelta(TEXT("Timer_60"));

		//m_dTimeDelta = dTimeDelta;
		m_dTimeDelta = 0.016666666666666666;

		if (Tick(dTimeDelta) & 0x80000000)
			return E_FAIL;

		if (FAILED(Render(dTimeDelta)))
			return E_FAIL;

		/* Mouse Lock */
		if (m_pGameInstance->Mouse_Down(CInput_Device::DIM_MB))
			m_bMouseLock = !m_bMouseLock;

//#ifdef _DEBUG
		Show_FPS(dTimeDelta);
//#endif

	}

	return S_OK;
}

void CMainApp::Lock_Mouse()
{
	ShowCursor(false);

	RECT rc;
	POINT p1, p2;

	GetClientRect(g_hWnd, &rc);
	p1.x = rc.left;
	p1.y = rc.top;
	p2.x = rc.right;
	p2.y = rc.bottom;

	ClientToScreen(g_hWnd, &p1);
	ClientToScreen(g_hWnd, &p2);

	SetCursorPos((p1.x + p2.x) / 2, (p1.y + p2.y) / 2);
}

_int CMainApp::Tick(_double dTimeDelta)
{
	NULL_CHECK_RETURN(m_pGameInstance, EVENT_ERROR);

	return m_pGameInstance->Tick(dTimeDelta, g_bWndActivate);
}

HRESULT CMainApp::Render(_double dTimeDelta)
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);
	NULL_CHECK_RETURN(m_pRenderer, E_FAIL);

	m_pGameInstance->Clear_BackBuffer(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencilBuffer();

	FAILED_CHECK_RETURN(m_pRenderer->Draw_Renderer(dTimeDelta), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Render_Level(), E_FAIL);

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Ready_Timer()
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Timer(TEXT("Timer_Default")), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Timer(TEXT("Timer_60")), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_ForStatic()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	/* For.Component */

	/* Renderer */
	CRenderer* pRenderer = CRenderer::Create(m_pDevice, m_pDeviceContext);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("Component_Renderer"), pRenderer), E_FAIL);
	m_pRenderer = static_cast<CRenderer*>(pRenderer->Clone_Component(nullptr));
	/* Transform */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("Component_Transform"), CTransform::Create(m_pDevice, m_pDeviceContext)), E_FAIL);
	/* VI_Buffer_Rect */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Default.hlsl"), "DefaultTechnique")), E_FAIL);
	/* VI_Buffer_Rect_UI */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_UI.hlsl"), "DefaultTechnique")), E_FAIL);
	/* VI_Buffer_Sprite */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Sprite"), CVIBuffer_Sprite::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Sprite.hlsl"), "DefaultTechnique")), E_FAIL);
	/* VI_Buffer_FontInstance */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component_Prototype(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_FontInstance"), CVIBuffer_FontInstance::Create(m_pDevice, m_pDeviceContext, 50, TEXT("../Bin/ShaderFiles/Shader_Font.hlsl"), "DefaultTechnique")), E_FAIL);

	/* For.GameObject */

	return S_OK;
}

HRESULT CMainApp::Ready_DefaultLevel(Level::ID eLevelID)
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Change_CurrentLevel(CLevel_Loading::Create(m_pDevice, m_pDeviceContext, Level::LEVEL_STATIC, eLevelID)), E_FAIL);

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp;

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Create Instance - CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);

	Safe_Delete(m_pPxEventCallback);

	CEffect_Generator::DestroyInstance(); // 이펙트 제어기
	CUI_Generator::DestroyInstance();
	CDataStorage::DestroyInstance();
	CEnvironment_Generator::DestroyInstance();

	CGameInstance::Release_Engine();
}

//#ifdef _DEBUG
void CMainApp::Show_FPS(_double dTimeDelta)
{
	++m_iRenderCount;
	m_dSecondAcc += dTimeDelta;

	if (1.0 <= m_dSecondAcc)
	{
		wsprintf(m_szFPS, TEXT("fps : %d"), m_iRenderCount);
		SetWindowText(g_hWnd, m_szFPS);
		m_dSecondAcc = 0.0;
		m_iRenderCount = 0;
	}
}
//#endif