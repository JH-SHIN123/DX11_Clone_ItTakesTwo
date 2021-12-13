#include "stdafx.h"
#include "..\Public\PixelBaboon.h"
#include "GameInstance.h"
#include "Cody.h"
#include "May.h"
#include "PixelHeart.h"
#include "PixelShield.h"

CPixelBaboon::CPixelBaboon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CGameObject(pDevice, pDeviceContext)
{
}

CPixelBaboon::CPixelBaboon(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPixelBaboon::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPixelBaboon::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_ArcadePixel"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom), E_FAIL);
	CTransform::TRANSFORM_DESC TransformDesc;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.f);
	TransformDesc.dSpeedPerSec = 1.f;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_PixelBaboon"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);

	if (nullptr != pArg)
	{
		_vector* vPosition = (_vector*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, *vPosition);
		m_pTransformCom->Set_Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 0.f));
	}

	CGameObject* pHeart[3] = { nullptr };


	_vector vHeartPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.04f, 0.f, 0.f);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_PixelHeart"), Level::LEVEL_STAGE, TEXT("GameObject_PixelHeart"), &vHeartPos, &pHeart[0]), E_FAIL);

	vHeartPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.04f, 0.02f, 0.f, 0.f);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_PixelHeart"), Level::LEVEL_STAGE, TEXT("GameObject_PixelHeart"), &vHeartPos, &pHeart[1]), E_FAIL);

	vHeartPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(-0.04f, 0.02f, 0.f, 0.f);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_PixelHeart"), Level::LEVEL_STAGE, TEXT("GameObject_PixelHeart"), &vHeartPos, &pHeart[2]), E_FAIL);

	m_pPixelHeart[0] = static_cast<CPixelHeart*>(pHeart[0]);
	m_pPixelHeart[1] = static_cast<CPixelHeart*>(pHeart[1]);
	m_pPixelHeart[2] = static_cast<CPixelHeart*>(pHeart[2]);

	CGameObject* pShield = nullptr;

	_vector vShieldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_PixelShield"), Level::LEVEL_STAGE, TEXT("GameObject_PixelShield"), &vShieldPos, &pShield), E_FAIL);

	m_pPixelShield = static_cast<CPixelShield*>(pShield);

	return S_OK;
}

_int CPixelBaboon::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_R))
	{
		m_bBlinking = true;
		m_iLifeCount -= 1;

		if (m_iLifeCount == 2)
			m_pPixelHeart[2]->Set_LifeCountRenderOff(true);
		else if (m_iLifeCount == 1)
			m_pPixelHeart[1]->Set_LifeCountRenderOff(true);
		else if (m_iLifeCount == 0)
			m_pPixelHeart[0]->Set_LifeCountRenderOff(true);    
	}

	Check_3DMoonBaboon_Position(dTimeDelta);
	Set_Hearts_Pos();


	return _int();
}

_int CPixelBaboon::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if(m_bBlinking == false)
		return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
	else
	{
		m_fBlinkingTime += (_float)dTimeDelta;
		if (m_fBlinkingTime > 0.1f)
		{
			m_fBlinkingTime = 0.f;
			++m_iBlinkingCount;
			m_bBlink = !m_bBlink;
		}
		else if (m_fBlinkingTime <= 0.1f && m_bBlink == false)
		{
			return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
		}

		if (m_iBlinkingCount == 8)
		{
			m_bBlinking = false;
			m_bBlink = false;
			m_iBlinkingCount = 0;
			m_pPixelShield->Set_Render_State(true);

			m_pPixelHeart[2]->Set_Render_Off(true);
			m_pPixelHeart[1]->Set_Render_Off(true);
			m_pPixelHeart[0]->Set_Render_Off(true);
		}
	}

	return _int();
}

HRESULT CPixelBaboon::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	m_pVIBufferCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(0));

	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);
	m_pVIBufferCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pVIBufferCom->Render(1);

	return S_OK;
}

void CPixelBaboon::Set_Hearts_Pos()
{
	for (_int i = 0; i < 3; ++i)
	{
		_vector vOffSet = XMVectorZero();
		if (i == 0)
			vOffSet = XMVectorSet(0.f, 0.04f, 0.f, 0.f);
		if (i == 1)
			vOffSet = XMVectorSet(0.04f, 0.02f, 0.f, 0.f);
		if (i == 2)
			vOffSet = XMVectorSet(-0.04f, 0.02f, 0.f, 0.f);

		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vOffSet;
		m_pPixelHeart[i]->Set_Position(vPosition);

		m_pPixelShield->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
}

void CPixelBaboon::Check_3DMoonBaboon_Position(_double dTimeDelta)
{
}


CPixelBaboon * CPixelBaboon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPixelBaboon* pInstance = new CPixelBaboon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create PixelBaboon Prototype, Error to CPixelBaboon::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPixelBaboon::Clone_GameObject(void * pArg)
{
	CPixelBaboon* pClone = new CPixelBaboon(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CPixelBaboon, Error to CPixelBaboon::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPixelBaboon::Free()
{
	for (_int i = 0; i < 3; ++i)
	{
		Safe_Release(m_pPixelHeart[i]);
	}
	Safe_Release(m_pPixelShield);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);

	CGameObject::Free();
}
