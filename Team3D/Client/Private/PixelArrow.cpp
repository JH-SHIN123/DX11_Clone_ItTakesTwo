#include "stdafx.h"
#include "..\Public\PixelArrow.h"
#include "GameInstance.h"
#include "Cody.h"
#include "May.h"
#include "PixelUFO.h"
#include "PixelBaboon.h"

CPixelArrow::CPixelArrow(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CGameObject(pDevice, pDeviceContext)
{
}

CPixelArrow::CPixelArrow(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPixelArrow::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPixelArrow::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_ArcadePixel"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_PixelArrow"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);

	if (nullptr != pArg)
	{
		_vector* vPosition = (_vector*)pArg;
		m_vFirstPosition = *vPosition;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, *vPosition);
		m_pTransformCom->Set_Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 0.f));
	}
	return S_OK;
}

_int CPixelArrow::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	Rotate_Arrow(dTimeDelta);

	return _int();
}

_int CPixelArrow::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (m_bRender == true)
		return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
	else
		return _int();
}

HRESULT CPixelArrow::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	m_pVIBufferCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(0));

	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);
	m_pVIBufferCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pVIBufferCom->Render(1);

	return S_OK;
}

void CPixelArrow::Rotate_Arrow(_double dTimeDelta)
{
	if (nullptr == ((CPixelBaboon*)DATABASE->Get_PixelBaboon()))
		return;

	//Angle = PixelUFO의 Right와 CrossHair <-> PixelUFO 향하는 방향벡터와의 각도가 되겠다.
	_vector vPixelUFOPos = ((CPixelUFO*)DATABASE->Get_PixelUFO())->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vPixelBaboonPos = ((CPixelBaboon*)DATABASE->Get_PixelBaboon())->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_vector vDir = vPixelBaboonPos - vPixelUFOPos;

	_vector vUFOToBaboon = XMVector3Normalize(vDir);
	_vector vUFORight = XMVector3Normalize(((CPixelUFO*)DATABASE->Get_PixelUFO())->Get_Transform()->Get_State(CTransform::STATE_RIGHT));

	_float fAngle = XMVectorGetX(XMVector3AngleBetweenNormals(vUFOToBaboon, vUFORight));

	if (vPixelUFOPos.m128_f32[1] > vPixelBaboonPos.m128_f32[1])
		fAngle *= -1.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vFirstPosition + (XMVector3Normalize(vDir) * 0.35f));
	m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), fAngle);
}

CPixelArrow * CPixelArrow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPixelArrow* pInstance = new CPixelArrow(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create PixelArrow Prototype, Error to CPixelArrow::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPixelArrow::Clone_GameObject(void * pArg)
{
	CPixelArrow* pClone = new CPixelArrow(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CPixelArrow, Error to CPixelArrow::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPixelArrow::Free()
{
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);

	CGameObject::Free();
}
