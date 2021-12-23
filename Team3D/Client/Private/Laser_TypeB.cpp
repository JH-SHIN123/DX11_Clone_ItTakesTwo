#include "stdafx.h"
#include "..\Public\Laser_TypeB.h"

#include "DataStorage.h"
#include "Cody.h"
#include "May.h"

CLaser_TypeB::CLaser_TypeB(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CLaser(pDevice, pDeviceContext)
{
}

CLaser_TypeB::CLaser_TypeB(const CLaser_TypeB & rhs)
	: CLaser(rhs)
{
}

HRESULT CLaser_TypeB::NativeConstruct_Prototype()
{
	CLaser::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CLaser_TypeB::NativeConstruct(void * pArg)
{
	CLaser::NativeConstruct(pArg);

	m_fShootSpeed = 30.f;

#ifdef __TEST_SE
	m_fShootSpeed = 30.f;
	m_vStartPoint = _float4(64.f, 1.f, 30.f, 1.f);
	m_vEndPoint = m_vStartPoint;

	XMStoreFloat4(&m_vLaserDir, XMVector3Normalize(XMVectorSet(1.f, 0.f, 0.f, 0.f)));
#endif

	return S_OK;
}

_int CLaser_TypeB::Tick(_double dTimeDelta)
{
	CLaser::Tick(dTimeDelta);

	/* 레이저 생성 시 */
	if (!m_isDead)
	{
		_float fAngle = 0.f;
		fAngle += (_float)dTimeDelta * m_fRotateSpeed;

		XMStoreFloat4x4(&m_matRotY, XMMatrixRotationY(XMConvertToRadians(fAngle)));

		_vector vDir = XMVector3TransformNormal(XMLoadFloat4(&m_vLaserDir), XMLoadFloat4x4(&m_matRotY));
		XMStoreFloat4(&m_vLaserDir, XMVector3Normalize(vDir));

		if (m_fLaserSizeY > 0)
			m_pGameInstance->Raycast(MH_PxVec3(XMLoadFloat4(&m_vStartPoint)), MH_PxVec3(XMLoadFloat4(&m_vLaserDir)), m_fLaserMaxY, m_RaycastBuffer, PxHitFlag::eDISTANCE | PxHitFlag::ePOSITION);

		if (m_RaycastBuffer.getNbAnyHits() > 0)
		{
			USERDATA* pUserData = (USERDATA*)m_RaycastBuffer.getAnyHit(0).actor->userData;

			if (nullptr != pUserData)
			{
				/* 코디 타격 */
				if (pUserData->eID == GameID::eCODY)
				{
					m_dDamagingDelay_Cody -= dTimeDelta;

					/* 지속 타격 데미지*/
					if (m_isHitCody)
					{
						if (m_dDamagingDelay_Cody <= 0.0)
						{
							// 데미지를 주는 함수
							((CCody*)DATABASE->GetCody())->Set_HpBarReduction(30);
							// 데미지 주기 초기화
							m_dDamagingDelay_Cody = 0.5;
						}
					}
					/* 첫 타격 데미지 */
					else
					{		
						// 데미지를 주는 함수
						((CCody*)DATABASE->GetCody())->Set_HpBarReduction(30);
						// 데미지 주기 초기화
						m_dDamagingDelay_Cody = 0.5;
						m_isHitCody = true;
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
					m_dDamagingDelay_May -= dTimeDelta;

					/* 지속 타격 데미지*/
					if (m_isHitMay)
					{
						if (m_dDamagingDelay_May <= 0.0)
						{
							// 데미지를 주는 함수
							((CMay*)DATABASE->GetMay())->Set_HpBarReduction(30);
							// 데미지 주기 초기화
							m_dDamagingDelay_May = 0.5;
						}
					}
					/* 첫 타격 데미지 */
					else
					{
						// 데미지를 주는 함수
						((CMay*)DATABASE->GetMay())->Set_HpBarReduction(30);
						// 데미지 주기 초기화
						m_dDamagingDelay_May = 0.5;
						m_isHitMay = true;
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
		m_fLaserSizeY -= 30.f * (_float)dTimeDelta;
		XMStoreFloat4(&m_vEndPoint, XMLoadFloat4(&m_vStartPoint) + XMLoadFloat4(&m_vLaserDir) * m_fLaserSizeY);

		if (m_fLaserSizeY < 0.f)
		{
			DATABASE->Set_LaserTypeB_Recovery(true);
			return EVENT_DEAD;
		}
	}

#ifdef __TEST_SE
	if (m_pGameInstance->Key_Down(DIK_N))
		m_isDead = true;
#endif
	
	Adjust_OutsideAlpha(dTimeDelta);
	Set_LaserMatices();

	GoUp(dTimeDelta);

	return NO_EVENT;
}

_int CLaser_TypeB::Late_Tick(_double dTimeDelta)
{
	CLaser::Late_Tick(dTimeDelta);

	m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);

	return NO_EVENT;
}

HRESULT CLaser_TypeB::Render(RENDER_GROUP::Enum eRender)
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

HRESULT CLaser_TypeB::Render_ShadowDepth()
{
	CLaser::Render_ShadowDepth();

	return S_OK;
}

void CLaser_TypeB::Set_StartPoint(_float4 vStartPoint)
{
	m_vStartPoint = vStartPoint;
}

void CLaser_TypeB::Set_LaserDir(_float4 vLaserDir)
{
	m_vLaserDir = vLaserDir;
}

void CLaser_TypeB::SetUp_MainLaserDirection(_uint iOption)
{
	if (0 == iOption)
		m_vLaserDir = { 0.f, 0.f, 1.f, 0.f };
	else if (1 == iOption)
		m_vLaserDir = { 0.5f, 0.f, 0.5f, 0.f };
	else if (2 == iOption)
		m_vLaserDir = { 1.f, 0.f, 0.f, 0.f };
	else if (3 == iOption)
		m_vLaserDir = { 0.5f, 0.f, -0.5f, 0.f };
	else if (4 == iOption)
		m_vLaserDir = { 0.f, 0.f, -1.f, 0.f };
	else if (5 == iOption)
		m_vLaserDir = { -0.5f, 0.f, -0.5f, 0.f };
	else if (6 == iOption)
		m_vLaserDir = { -1.f, 0.f, 0.f, 0.f };
	else if (7 == iOption)
		m_vLaserDir = { -0.5f, 0.f, 0.5f, 0.f };
}

void CLaser_TypeB::SetUp_SubLaserDirection(_uint iOption)
{
	if (0 == iOption)
		m_vLaserDir = { 0.5f, 0.f, 0.5f, 0.f };
	else if (1 == iOption)
		m_vLaserDir = { 0.5f, 0.f, -0.5f, 0.f };
	else if (2 == iOption)
		m_vLaserDir = { -0.5f, 0.f, -0.5f, 0.f };
	else if (3 == iOption)
		m_vLaserDir = { -0.5f, 0.f, 0.5f, 0.f };
}

void CLaser_TypeB::Set_RotateSpeed(_float fSpeed)
{
	m_fRotateSpeed = fSpeed;
}

void CLaser_TypeB::Set_LaserTypeBUp(_float fMaxDistance, _float fSpeed)
{
	XMStoreFloat3(&m_vMaxPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_vMaxPos.y += fMaxDistance;

	m_fMaxY = fMaxDistance;
	m_IsGoUp = true;
	m_fUpSpeed = fSpeed;
}

void CLaser_TypeB::GoUp(_double dTimeDelta)
{
	if (false == m_IsGoUp)
		return;

	_float fDist = (_float)dTimeDelta * m_fUpSpeed;
	m_fDistance += fDist;

	if (m_fMaxY <= m_fDistance)
	{
		m_fMaxY = 0.f;
		m_IsGoUp = false;
		m_fDistance = 0.f;
		return;
	}

	m_vStartPoint.y += m_fUpSpeed * (_float)dTimeDelta;
}

CLaser_TypeB * CLaser_TypeB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLaser_TypeB* pInstance = new CLaser_TypeB(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CLaser_TypeB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLaser_TypeB::Clone_GameObject(void * pArg)
{
	CLaser_TypeB* pInstance = new CLaser_TypeB(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CLaser_TypeB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLaser_TypeB::Free()
{
	CLaser::Free();
}