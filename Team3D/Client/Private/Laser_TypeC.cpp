#include "stdafx.h"
#include "..\Public\Laser_TypeC.h"
#include "MoonUFO.h"
#include "DataStorage.h"
#include "Effect_MoonUFO_Laser_ColorSmoke.h"
#include "RunningMoonBaboon.h"
#include "Script.h"
#include "SubCamera.h"
#include "MainCamera.h"
CLaser_TypeC::CLaser_TypeC(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CLaser(pDevice, pDeviceContext)
{
}

CLaser_TypeC::CLaser_TypeC(const CLaser_TypeC & rhs)
	: CLaser(rhs)
{
}

HRESULT CLaser_TypeC::NativeConstruct_Prototype()
{
	CLaser::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CLaser_TypeC::NativeConstruct(void * pArg)
{
	CLaser::NativeConstruct(pArg);

	m_pMoonUFO = (CMoonUFO*)DATABASE->Get_MoonUFO();
	NULL_CHECK_RETURN(m_pMoonUFO, E_FAIL);
	Safe_AddRef(m_pMoonUFO);

	m_dChargingTime = 0.01;
	m_fShootSpeed = 900.f;

	DATABASE->Set_LaserTypeC(this);

	m_dCreateEffectCycle = 0.3;

#ifdef __TEST_SE
	m_dChargingTime = 3.0;
	m_fShootSpeed = 100.f;
	m_vStartPoint = _float4(64.f, 10.f, 30.f, 1.f);
	m_vEndPoint = m_vStartPoint;

	XMStoreFloat4(&m_vLaserDir, XMVector3Normalize(XMVectorSet(1.f, -1.f, 0.f, 0.f)));
#endif

	return S_OK;
}

_int CLaser_TypeC::Tick(_double dTimeDelta)
{
	CLaser::Tick(dTimeDelta);

	/* 레이저 생성 시 */
	if (!m_isDead)
	{
		/* 차지 시간 */
		if (m_dChargingTime > 0.0)
		{
			/* 차지 시작 시간, 에너지를 모으는 이펙트 생성 */
			if (m_dChargingTime == 3.0)
			{
			}

			m_dChargingTime -= dTimeDelta;

			/* 차지 시간이 다 채워진 순간 */
			/* 쏘는 순간 터트리는 이펙트 생성 */
			if (m_dChargingTime <= 0.0)
			{
				static_cast<CMainCamera*>(DATABASE->Get_MainCam())->Start_CamEffect(TEXT("Cam_Shake_ShootLaser_Cody"));
				static_cast<CSubCamera*>(DATABASE->Get_SubCam())->Start_CamEffect(TEXT("Cam_Shake_ShootLaser"));

			}

			return NO_EVENT;
		}

		if (m_fLaserSizeY > 0)
		{
			m_vStartPoint = m_pMoonUFO->Get_LaserStartPos();
			m_vLaserDir = m_pMoonUFO->Get_LaserDir();
			m_pGameInstance->Raycast(MH_PxVec3(XMLoadFloat4(&m_vStartPoint)), MH_PxVec3(XMLoadFloat4(&m_vLaserDir)), m_fLaserMaxY, m_RaycastBuffer, PxHitFlag::eDISTANCE | PxHitFlag::ePOSITION);
		}
		
		if (m_RaycastBuffer.getNbAnyHits() > 0)
		{
			USERDATA* pUserData = (USERDATA*)m_RaycastBuffer.getAnyHit(0).actor->userData;

			if (nullptr != pUserData)
			{
				if (pUserData->eID == GameID::eRUNNINGMOONBABOON)
				{
					((CRunningMoonBaboon*)DATABASE->Get_RunningMoonBaboon())->Set_LaserHit(true);
				}
				else
				{
					if (SCRIPT->Get_Script_Played(54) == false)
					{
						SCRIPT->VoiceFile_No54();
						SCRIPT->Set_Script_Played(54, true);
					}
					else if (SCRIPT->Get_Script_Played(55) == false)
					{
						SCRIPT->VoiceFile_No55();
						SCRIPT->Set_Script_Played(55, true);
					}
				}
			}

			m_fLaserSizeY = m_RaycastBuffer.getAnyHit(0).distance;
			m_vEndPoint = MH_XMFloat4(m_RaycastBuffer.getAnyHit(0).position, 1.f);
			m_isCollided = true;

			///* 충돌 시 이펙트 생성 */
			if (m_dCreateEffectDelay <= 0.0)
			{
				//충돌 시 생성할 이펙트

				//이펙트 생성 주기
				m_dCreateEffectCycle = m_dCreateEffectCycle;
			}
			else
				m_dCreateEffectCycle = 0.0;

			if (true == m_IsPaticleCreate && true == m_isCollided)
			{
				CGameObject* pGameObject = nullptr;
				m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Smoke"), Level::LEVEL_STAGE, TEXT("GameObject_2D_MoonUFO_Laser_ColorSmoke"), nullptr , &pGameObject);
				m_pLaserSmoke = static_cast<CEffect_MoonUFO_Laser_ColorSmoke*>(pGameObject);

				m_IsPaticleCreate = false;
			}
		}
		else
		{
			m_isHitCody = false;
			m_dDamagingDelay_Cody = 0.0;
			m_isHitMay = false;
			m_dDamagingDelay_May = 0.0;

			m_isCollided = false;
			XMStoreFloat4(&m_vEndPoint, XMLoadFloat4(&m_vStartPoint) + XMLoadFloat4(&m_vLaserDir) * m_fLaserSizeY);
			m_fLaserSizeY = m_fLaserMaxY;

		}

		/* 레이저 크기 */
		if (m_fLaserMaxY < 200.f)
			m_fLaserMaxY += m_fShootSpeed * (_float)dTimeDelta;
		if (m_fLaserSizeX < 5.f)
			m_fLaserSizeX += m_fShootSpeed * 0.5f * (_float)dTimeDelta;

		if (m_fLaserMaxY >= 200.f)
		{
			m_isDead = true;
			((CMoonUFO*)DATABASE->Get_MoonUFO())->Compensate_LaserDir(false);
		}
	}
	/* 레이저 종료 시*/
	else
	{
		m_fLaserSizeX -= 300.f * (_float)dTimeDelta;
		
		if (m_fLaserSizeX < 0.f)
		{
			if (nullptr != m_pLaserSmoke)
				m_pLaserSmoke->Set_Dead();

			return EVENT_DEAD;
		}
	}

	if (nullptr != m_pLaserSmoke)
		m_pLaserSmoke->Set_Pos(XMLoadFloat4(&m_vEndPoint));

#ifdef __TEST_SE
	if (m_pGameInstance->Key_Down(DIK_N))
		m_isDead = true;
#endif

	Adjust_OutsideAlpha(dTimeDelta);
	Set_LaserMatices();

	return NO_EVENT;
}

_int CLaser_TypeC::Late_Tick(_double dTimeDelta)
{
	CLaser::Late_Tick(dTimeDelta);

	m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);

	return NO_EVENT;
}

HRESULT CLaser_TypeC::Render(RENDER_GROUP::Enum eRender)
{
	CLaser::Render(eRender);

	_int i = 1;

	m_pVIBufferCom->Set_DefaultVariables_Perspective(XMMatrixIdentity());
	m_pVIBufferCom->Set_Variable("g_IsBillBoard", &i, sizeof(_int));
	m_pVIBufferCom->Set_Variable("g_fAlpha", &m_fOutsideAlpha, sizeof(_float));

	m_pVIBufferCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(0));
	m_pVIBufferCom->Render(13, &m_pLaserMatrices[3], 1);

	m_pVIBufferCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(3));
	m_pVIBufferCom->Render(12, &m_pLaserMatrices[1], 2);
	m_pVIBufferCom->Render(11, &m_pLaserMatrices[0], 1);

	return NO_EVENT;
}

HRESULT CLaser_TypeC::Render_ShadowDepth()
{
	CLaser::Render_ShadowDepth();

	return S_OK;
}

CLaser_TypeC * CLaser_TypeC::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLaser_TypeC* pInstance = new CLaser_TypeC(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CLaser_TypeC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLaser_TypeC::Clone_GameObject(void * pArg)
{
	CLaser_TypeC* pInstance = new CLaser_TypeC(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CLaser_TypeC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLaser_TypeC::Free()
{
	Safe_Release(m_pLaserSmoke);
	Safe_Release(m_pMoonUFO);

	CLaser::Free();
}