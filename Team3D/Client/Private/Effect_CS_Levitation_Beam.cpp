#include "stdafx.h"
#include "Effect_CS_Levitation_Beam.h"
#include "Effect_Generator.h"
#include "Cody.h"
#include "May.h"
#include "DataStorage.h"
#include "Effect_CS_Levitation_Beam_Particle.h"

CEffect_CS_Levitation_Beam::CEffect_CS_Levitation_Beam(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_CS_Levitation_Beam::CEffect_CS_Levitation_Beam(const CEffect_CS_Levitation_Beam & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_CS_Levitation_Beam::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_CS_Levitation_Beam::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));
	//
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_WarpGates_Cable"), TEXT("Com_Tex"), (CComponent**)&m_pTexturesCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_Rect_TripleUV"), TEXT("Com_VIBuffer_Rect"), (CComponent**)&m_pBufferRectCom), E_FAIL);


	Check_WorldMatrix();

	m_EffectDesc_Prototype.fLifeTime = 0.f;


	EFFECT_DESC_CLONE Data;
	XMStoreFloat4x4(&Data.WorldMatrix, m_pTransformCom->Get_WorldMatrix());
	m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Effect_CutScene"), Level::LEVEL_STAGE, TEXT("GameObject_2DCS_Levitation_Beam_Particle"), &Data, (CGameObject**)&m_pParticle);


	return S_OK;
}

_int CEffect_CS_Levitation_Beam::Tick(_double TimeDelta)
{
 	if (false == m_IsActivate && 0.f >= m_fTime)
 		return EVENT_DEAD;

	m_EffectDesc_Prototype.fLifeTime += (_float)TimeDelta;

 	if (10.f < m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	if (true == m_IsActivate)
	{
		m_fTime += (_float)TimeDelta * 0.5f;
		if (0.75f < m_fTime)
			m_fTime = 0.75f;
	}
	else
	{
		m_fTime -= (_float)TimeDelta * 0.25f;
		if (0.0f > m_fTime)
			m_fTime = 0.0f;
	}


	return _int();
}

_int CEffect_CS_Levitation_Beam::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_CS_Levitation_Beam::Render(RENDER_GROUP::Enum eGroup)
{
	_int i = 0;
	m_pBufferRectCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pBufferRectCom->Set_Variable("g_vColor", &m_vColor, sizeof(_float4));
	m_pBufferRectCom->Set_Variable("g_fAlpha", &m_fTime, sizeof(_float));
	m_pBufferRectCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));

	m_pBufferRectCom->Render(4);

	return S_OK;
}

void CEffect_CS_Levitation_Beam::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_CS_Levitation_Beam::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_CS_Levitation_Beam::Instance_UV(_float TimeDelta, _int iIndex)
{
}

HRESULT CEffect_CS_Levitation_Beam::Ready_Instance()
{
	return S_OK;
}

void CEffect_CS_Levitation_Beam::Check_WorldMatrix()
{
	_matrix WorldMatrix_Cody = static_cast<CCody*>(DATABASE->GetCody())->Get_Transform()->Get_WorldMatrix();
	_matrix BoneMatrix_Cody = static_cast<CCody*>(DATABASE->GetCody())->Get_Model()->Get_BoneMatrix("Hips");
	WorldMatrix_Cody = BoneMatrix_Cody * WorldMatrix_Cody;
	_vector vPos_Cody = WorldMatrix_Cody.r[3];

	_matrix WorldMatrix_May = static_cast<CMay*>(DATABASE->GetMay())->Get_Transform()->Get_WorldMatrix();
	_matrix BoneMatrix_May = static_cast<CMay*>(DATABASE->GetMay())->Get_Model()->Get_BoneMatrix("Hips");
	WorldMatrix_May = BoneMatrix_May * WorldMatrix_May;
	_vector vPos_May = WorldMatrix_May.r[3];

	_vector vLength = XMVector3Length(vPos_Cody - vPos_May) * 0.5f;
	_vector vDir = XMVector3Normalize(vPos_Cody - vPos_May);

	_vector vPos_Center = vPos_May + vDir * vLength;

	_matrix WorldMatrix = XMMatrixIdentity();


	if (0 == m_EffectDesc_Clone.iCutSceneTake)
	{
		_vector vPos_Cam = DATABASE->Get_SubCam()->Get_Position();
		_vector vDir_Cam = XMVector3Normalize(vPos_Center - vPos_Cam);
		vPos_Center -= vDir_Cam * 1.5f;
		WorldMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vRight = XMVector3Cross(WorldMatrix.r[1], vDir_Cam);
		WorldMatrix.r[0] = vRight;
		WorldMatrix.r[2] = vDir_Cam;
	}
	else if ( 1 == m_EffectDesc_Clone.iCutSceneTake)
	{
		vLength = XMVector3Length(vPos_Cody - vPos_May) * 3.2f;
		vDir = XMVector3Normalize(vPos_Cody - vPos_May);	
		vPos_Center = vPos_May + vDir * vLength;	

		_vector vPos_Moon = DATABASE->Get_Mooon()->Get_Position();
		_vector vDir_Moon = XMVector3Normalize(vPos_Center - vPos_Moon);
		WorldMatrix.r[1] = vDir_Moon;

		_vector vPos_Cam = DATABASE->Get_SubCam()->Get_Position();
		_vector vDir_Cam = XMVector3Normalize(vPos_Center - vPos_Cam);

		_vector vRight = XMVector3Cross(vDir_Cam, WorldMatrix.r[1]);
		WorldMatrix.r[0] = vRight;

		_vector vLook = XMVector3Cross(WorldMatrix.r[1], vRight);
		WorldMatrix.r[2] = vLook;
	}

	WorldMatrix.r[0] *= m_vSize.x;
	WorldMatrix.r[2] *= m_vSize.y;

	WorldMatrix.r[3] = vPos_Center;
	WorldMatrix.r[3].m128_f32[1] += m_vSize.y * 0.3f;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	m_pTransformCom->RotatePitch_Angle(90.f);
}

void CEffect_CS_Levitation_Beam::Check_RotateUV(_double TimeDelta)
{
	m_dRotateTime += TimeDelta * 30;
	if (360.0 < m_dRotateTime)
		m_dRotateTime = 0.0;

	_float fRadian = XMConvertToRadians((_float)m_dRotateTime);
}

CEffect_CS_Levitation_Beam * CEffect_CS_Levitation_Beam::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_CS_Levitation_Beam*	pInstance = new CEffect_CS_Levitation_Beam(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_CS_Levitation_Beam");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_CS_Levitation_Beam::Clone_GameObject(void * pArg)
{
	CEffect_CS_Levitation_Beam* pInstance = new CEffect_CS_Levitation_Beam(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_CS_Levitation_Beam");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_CS_Levitation_Beam::Free()
{
	Safe_Release(m_pBufferRectCom);
	Safe_Release(m_pParticle);

	__super::Free();
}
