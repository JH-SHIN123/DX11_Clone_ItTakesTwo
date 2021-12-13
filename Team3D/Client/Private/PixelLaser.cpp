#include "stdafx.h"
#include "..\Public\PixelLaser.h"
#include "GameInstance.h"
#include "Cody.h"
#include "May.h"
#include "PixelUFO.h"
#include "PixelCrossHair.h"

CPixelLaser::CPixelLaser(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CGameObject(pDevice, pDeviceContext)
{
}

CPixelLaser::CPixelLaser(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPixelLaser::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPixelLaser::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_ArcadePixel"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom), E_FAIL);
	CTransform::TRANSFORM_DESC TransformDesc;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.f);
	TransformDesc.dSpeedPerSec = 1.f;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_PixelLaser"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);

	if (nullptr != pArg)
	{
		_vector* vPosition = (_vector*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, *vPosition);
		m_pTransformCom->Set_Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 0.f));
	}

	DATABASE->Set_PixelLaser(this);

	return S_OK;
}

_int CPixelLaser::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (nullptr == DATABASE->Get_PixelUFO()
		|| nullptr == DATABASE->Get_PixelCrossHair())
		return S_OK;


	if (m_bRender == true)
	{
		m_fRenderTime += (_float)dTimeDelta;
		if (m_fRenderTime > 0.5f)
		{
			m_bRender = false;
			m_fRenderTime = 0.f;
		}
	}
	else if (m_bRender == false)
		Activate_Laser(dTimeDelta);


	return _int();
}

_int CPixelLaser::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (m_bRender == true)
		return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
	else
		return _int();

}

HRESULT CPixelLaser::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	m_pVIBufferCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(0));

	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);
	m_pVIBufferCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pVIBufferCom->Render(1);

	return S_OK;
}

void CPixelLaser::Activate_Laser(_double dTimeDelta)
{
	//Angle = PixelUFO의 Right와 CrossHair <-> PixelUFO 향하는 방향벡터와의 각도가 되겠다.
	_vector vPixelUFOPos = ((CPixelUFO*)DATABASE->Get_PixelUFO())->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vPixelCrossHairPos = ((CPixelCrossHair*)DATABASE->Get_PixelCrossHair())->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_vector vDir = vPixelCrossHairPos - vPixelUFOPos;

	_vector vUFOToCrossHair = XMVector3Normalize(vDir);
	_vector vUFORight = XMVector3Normalize(((CPixelUFO*)DATABASE->Get_PixelUFO())->Get_Transform()->Get_State(CTransform::STATE_RIGHT));

	_float fAngle = XMVectorGetX(XMVector3AngleBetweenNormals(vUFOToCrossHair, vUFORight));

	if (vPixelUFOPos.m128_f32[1] > vPixelCrossHairPos.m128_f32[1])
		fAngle *= -1.f;

	m_pTransformCom->Set_RotateAxis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), fAngle);

	_float fDist = XMVectorGetX(XMVector3Length(vDir));

	if (fDist > 0.1f)
		m_pTransformCom->Set_Scale(XMVectorSet(fDist * 2.f, 0.1f, 0.1f, 0.f));
}

CPixelLaser * CPixelLaser::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPixelLaser* pInstance = new CPixelLaser(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create PixelLaser Prototype, Error to CPixelLaser::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPixelLaser::Clone_GameObject(void * pArg)
{
	CPixelLaser* pClone = new CPixelLaser(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CPixelLaser, Error to CPixelLaser::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPixelLaser::Free()
{
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);

	CGameObject::Free();
}
