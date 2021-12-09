#include "stdafx.h"
#include "..\Public\Laser_TypeA.h"

#include "UFO.h"
#include "DataStorage.h"

CLaser_TypeA::CLaser_TypeA(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CLaser(pDevice, pDeviceContext)
{
}

CLaser_TypeA::CLaser_TypeA(const CLaser_TypeA & rhs)
	: CLaser(rhs)
{
}

HRESULT CLaser_TypeA::NativeConstruct_Prototype()
{
	CLaser::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CLaser_TypeA::NativeConstruct(void * pArg)
{
	CLaser::NativeConstruct(pArg);

	m_pBossUFO = (CUFO*)DATABASE->Get_BossUFO();
	NULL_CHECK_RETURN(m_pBossUFO, E_FAIL);
	Safe_AddRef(m_pBossUFO);

	m_dChargingTime = 3.0;
	m_fShootSpeed = 100.f;

#ifdef __TEST_SE
	m_dChargingTime = 3.0;
	m_fShootSpeed = 100.f;
	m_vStartPoint = _float4(64.f, 10.f, 30.f, 1.f);
	m_vEndPoint = m_vStartPoint;

	XMStoreFloat4(&m_vLaserDir, XMVector3Normalize(XMVectorSet(1.f, -1.f, 0.f, 0.f)));
#endif

	return S_OK;
}

_int CLaser_TypeA::Tick(_double dTimeDelta)
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
			}

			return NO_EVENT;
		}

		if (m_fLaserSizeY > 0)
		{
			m_vStartPoint = m_pBossUFO->Get_LaserStartPos();
			m_vLaserDir = m_pBossUFO->Get_LaserDir();
			m_pGameInstance->Raycast(MH_PxVec3(XMLoadFloat4(&m_vStartPoint)), MH_PxVec3(XMLoadFloat4(&m_vLaserDir)), m_fLaserMaxY, m_RaycastBuffer, PxHitFlag::eDISTANCE | PxHitFlag::ePOSITION);
		}

		if (m_RaycastBuffer.getNbAnyHits() > 0)
		{
			USERDATA* pUserData = (USERDATA*)m_RaycastBuffer.getAnyHit(0).actor->userData;

			if (nullptr != pUserData)
			{
				/* 코디 타격 */
				if (pUserData->eID == GameID::eCODY)
				{
					/* 지속 타격 데미지*/
					if (m_isHitCody)
					{
						if (m_dDamagingDelay_Cody <= 0.0)
						{
							// 데미지를 주는 함수

							// 데미지 주기 초기화
							m_dDamagingDelay_Cody = 0.3;
						}
					}
					/* 첫 타격 데미지 */
					else
					{
						// 데미지를 주는 함수

						// 데미지 주기 초기화
						m_dDamagingDelay_Cody = 0.3;
					}
				}
				else
				{
					m_isHitCody = false;
					m_dDamagingDelay_Cody = 0.0;
				}

				/* 메이 타격 */
				if (pUserData->eID == GameID::eMAY)
				{
					/* 지속 타격 데미지*/
					if (m_isHitMay)
					{
						if (m_dDamagingDelay_May <= 0.0)
						{
							// 데미지를 주는 함수

							// 데미지 주기 초기화
							m_dDamagingDelay_May = 0.3;
						}
					}
					/* 첫 타격 데미지 */
					else
					{
						// 데미지를 주는 함수

						// 데미지 주기 초기화
						m_dDamagingDelay_May = 0.3;
					}
				}
				else
				{
					m_isHitMay = false;
					m_dDamagingDelay_May = 0.0;
				}
			}

			m_fLaserSizeY = m_RaycastBuffer.getAnyHit(0).distance;
			m_vEndPoint = MH_XMFloat4(m_RaycastBuffer.getAnyHit(0).position, 1.f);
			m_isCollided = true;

			/* 충돌 시 이펙트 생성 */
			if (m_dCreateEffectDelay <= 0.0)
			{
				// 충돌 시 생성할 이펙트

				// 이펙트 생성 주기
				m_dCreateEffectDelay = m_dCreateEffectCycle;
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
		if (m_fLaserSizeX < 3.f)
			m_fLaserSizeX += m_fShootSpeed * 0.2f * (_float)dTimeDelta;
	}
	/* 레이저 종료 시*/
	else
	{
		m_fLaserSizeX -= 6.f * (_float)dTimeDelta;

		if (m_fLaserSizeX < 0.f)
			return EVENT_DEAD;
	}

#ifdef __TEST_SE
	if (m_pGameInstance->Key_Down(DIK_N))
		m_isDead = true;
#endif
	
	Adjust_OutsideAlpha(dTimeDelta);
	Set_LaserMatices();

	return NO_EVENT;
}

_int CLaser_TypeA::Late_Tick(_double dTimeDelta)
{
	CLaser::Late_Tick(dTimeDelta);

	m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);

	return NO_EVENT;
}

HRESULT CLaser_TypeA::Render(RENDER_GROUP::Enum eRender)
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

HRESULT CLaser_TypeA::Render_ShadowDepth()
{
	CLaser::Render_ShadowDepth();

	return S_OK;
}

CLaser_TypeA * CLaser_TypeA::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLaser_TypeA* pInstance = new CLaser_TypeA(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CLaser_TypeA");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLaser_TypeA::Clone_GameObject(void * pArg)
{
	CLaser_TypeA* pInstance = new CLaser_TypeA(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CLaser_TypeA");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLaser_TypeA::Free()
{
	Safe_Release(m_pBossUFO);

	CLaser::Free();
}