#include "stdafx.h"
#include "..\Public\Effect_RespawnTunnel.h"

CEffect_RespawnTunnel::CEffect_RespawnTunnel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_RespawnTunnel::CEffect_RespawnTunnel(const CEffect_RespawnTunnel & rhs)
	: CInGameEffect_Model(rhs)
{
}

HRESULT CEffect_RespawnTunnel::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_RespawnTunnel::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.ModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_Color_Ramp", TEXT("Com_Texture_Color"), (CComponent**)&m_pTexturesCom_ColorRamp), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_Tilling_Noise", TEXT("Com_Texture_Distor"), (CComponent**)&m_pTexturesCom_Distortion), E_FAIL);

	//
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_VIBuffer_Rect_TripleUV", TEXT("Com_VIBuffer"), (CComponent**)&m_pBufferRectCom_Preview), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_Level_Preview", TEXT("Com_Texture_Preview"), (CComponent**)&m_pTexturesCom_Preview), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_Circle_Alpha", TEXT("Com_Texture_Mask"), (CComponent**)&m_pTexturesCom_Masking), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_T_Slime_Cloud", TEXT("Com_Texture_Dist"), (CComponent**)&m_pTexturesCom_Distortion_2), E_FAIL);
	//
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[0] *= -1.f;
	WorldMatrix.r[2] *= -1.f;
	WorldMatrix.r[3].m128_f32[1] += 7.f;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	m_pTransformCom->Set_Scale(XMVectorSet(0.4f, 0.4f, 0.4f, 0.f));

	return S_OK;
}

_int CEffect_RespawnTunnel::Tick(_double TimeDelta)
{
	m_fTime_UV += (_float)TimeDelta * 0.05f;
	if (1.f <= m_fTime_UV)
		m_fTime_UV = 0.f;

	m_fRadianAngle += (_float)TimeDelta * 10.f;
	if (360.f <= m_fRadianAngle)
		m_fRadianAngle = 0.f;


	return _int();
}

_int CEffect_RespawnTunnel::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_RespawnTunnel::Render(RENDER_GROUP::Enum eGroup)
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

// 	_float fRadian = XMConvertToRadians((_float)m_dAngle);
// 	//g_vColor
// 
// 	m_pModelCom->Set_Variable("g_vColorRamp_UV", &vColorRampUV, sizeof(_float2));
 	m_pModelCom->Set_Variable("g_fTime", &m_fTime_UV, sizeof(_float));
	m_pModelCom->Set_ShaderResourceView("g_DistortionTexture",	m_pTexturesCom_Distortion->Get_ShaderResourceView(2));
	m_pModelCom->Set_ShaderResourceView("g_ColorRampTexture",	m_pTexturesCom_ColorRamp->Get_ShaderResourceView(12));
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(7);



	_matrix LocalMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
	LocalMatrix.r[0] *= 3.f;
	LocalMatrix.r[2] *= 3.f;
	LocalMatrix.r[3] = XMVectorSet(0.f, 0.f, 12.5f, 1.f);
	XMStoreFloat4x4(&m_LocalMatrix_Preview, LocalMatrix);
	_matrix WorldMatrix = XMLoadFloat4x4(&m_LocalMatrix_Preview);
	WorldMatrix *= m_pTransformCom->Get_WorldMatrix();

	m_pBufferRectCom_Preview->Set_DefaultVariables_Perspective(WorldMatrix);
	
	_float Radian = XMConvertToRadians(m_fRadianAngle);
	m_pBufferRectCom_Preview->Set_Variable("g_fRadianAngle", &Radian, sizeof(_float));
	m_pBufferRectCom_Preview->Set_ShaderResourceView("g_DistortionTexture",		m_pTexturesCom_Distortion_2->Get_ShaderResourceView(0));
	m_pBufferRectCom_Preview->Set_ShaderResourceView("g_MaskTexture",		m_pTexturesCom_Masking->Get_ShaderResourceView(0));
	m_pBufferRectCom_Preview->Set_ShaderResourceView("g_DiffuseTexture",	m_pTexturesCom_Preview->Get_ShaderResourceView(2));
	m_pBufferRectCom_Preview->Render(1);
	return S_OK;
}

void CEffect_RespawnTunnel::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

_fmatrix CEffect_RespawnTunnel::Calcul_WorldMatrix(_float4 vLocalPos)
{
	_matrix vWolrdMatrix = m_pTransformCom->Get_WorldMatrix();
	for (_int i = 0; i < 3; ++i)
		vWolrdMatrix.r[i] = XMVector3Normalize(vWolrdMatrix.r[i]);

	vWolrdMatrix *= XMMatrixRotationAxis(vWolrdMatrix.r[0], 90.f);

//	vWolrdMatrix.r[3] = XMVector3Transform(XMLoadFloat4(&m_vLocalPosition_Preview), vWolrdMatrix);


	return vWolrdMatrix;
}

CEffect_RespawnTunnel * CEffect_RespawnTunnel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_RespawnTunnel*	pInstance = new CEffect_RespawnTunnel(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_RespawnTunnel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_RespawnTunnel::Clone_GameObject(void * pArg)
{
	CEffect_RespawnTunnel* pInstance = new CEffect_RespawnTunnel(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_RespawnTunnel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_RespawnTunnel::Free()
{
	Safe_Release(m_pTexturesCom_Distortion);
	Safe_Release(m_pTexturesCom_ColorRamp);

	Safe_Release(m_pTexturesCom_Distortion_2);
	Safe_Release(m_pBufferRectCom_Preview);
	Safe_Release(m_pTexturesCom_Preview);
	Safe_Release(m_pTexturesCom_Masking);

	__super::Free();
}
