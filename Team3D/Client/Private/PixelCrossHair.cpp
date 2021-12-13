#include "stdafx.h"
#include "..\Public\PixelCrossHair.h"
#include "GameInstance.h"
#include "Cody.h"
#include "May.h"

CPixelCrossHair::CPixelCrossHair(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CGameObject(pDevice, pDeviceContext)
{
}

CPixelCrossHair::CPixelCrossHair(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPixelCrossHair::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPixelCrossHair::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_ArcadePixel"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom), E_FAIL);
	CTransform::TRANSFORM_DESC TransformDesc;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.f);
	TransformDesc.dSpeedPerSec = 1.f;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_PixelCrossHair"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);

	if (nullptr != pArg)
	{
		_vector* vPosition = (_vector*)pArg;
		// 최초 X세팅
		// 최초 Y세팅
		m_fInitialX = (*vPosition).m128_f32[0];
		m_fInitialY = (*vPosition).m128_f32[1];

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, *vPosition);
		m_pTransformCom->Set_Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 0.f));
	}

	DATABASE->Set_PixelCrossHair(this);

	return S_OK;
}

_int CPixelCrossHair::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (((CCody*)DATABASE->GetCody())->Get_IsInArcadeJoyStick() == true)
	{
		Movement(dTimeDelta);
	}
	

	return NO_EVENT;
}

_int CPixelCrossHair::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
}

HRESULT CPixelCrossHair::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	m_pVIBufferCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(0));

	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);
	m_pVIBufferCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pVIBufferCom->Render(0);

	return S_OK;
}

void CPixelCrossHair::Movement(_double dTimeDelta)
{
	_vector vDir = XMVectorZero();

	_vector vInitialPos = XMVectorSet(m_fInitialX, m_fInitialY, 0.f, 1.f);

	if (m_pGameInstance->Key_Pressing(DIK_W) && m_pGameInstance->Key_Pressing(DIK_A))
		vDir = XMVectorSet(1.f, 1.f, 0.f, 0.f);
	else if (m_pGameInstance->Key_Pressing(DIK_W) && m_pGameInstance->Key_Pressing(DIK_D))
		vDir = XMVectorSet(-1.f, 1.f, 0.f, 0.f);
	else if (m_pGameInstance->Key_Pressing(DIK_S) && m_pGameInstance->Key_Pressing(DIK_A))
		vDir = XMVectorSet(1.f, -1.f, 0.f, 0.f);
	else if (m_pGameInstance->Key_Pressing(DIK_S) && m_pGameInstance->Key_Pressing(DIK_D))
		vDir = XMVectorSet(-1.f, -1.f, 0.f, 0.f);
	else if (m_pGameInstance->Key_Pressing(DIK_W))
		vDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	else if (m_pGameInstance->Key_Pressing(DIK_A))
		vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	else if (m_pGameInstance->Key_Pressing(DIK_S))
		vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	else if (m_pGameInstance->Key_Pressing(DIK_D))
		vDir = XMVectorSet(-1.f, 0.f, 0.f, 0.f);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector NextPos = vPos += vDir * (_float)dTimeDelta * 0.5f;
	_vector vChanged = vInitialPos - NextPos;
	_float r = XMVectorGetX(XMVector2Length(vChanged));

	if (r <= 0.38f) // 자유롭게 이동
		m_pTransformCom->MoveToDir(vDir, dTimeDelta * 0.5f);
	else // 슬라이딩 벡터 구하기.
	{
	}
}

CPixelCrossHair * CPixelCrossHair::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPixelCrossHair* pInstance = new CPixelCrossHair(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create PixelCrossHair Prototype, Error to CPixelCrossHair::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPixelCrossHair::Clone_GameObject(void * pArg)
{
	CPixelCrossHair* pClone = new CPixelCrossHair(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CPixelCrossHair, Error to CPixelCrossHair::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPixelCrossHair::Free()
{
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);

	CGameObject::Free();
}
