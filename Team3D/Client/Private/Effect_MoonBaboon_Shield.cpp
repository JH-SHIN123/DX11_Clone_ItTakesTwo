#include "stdafx.h"
#include "..\Public\Effect_MoonBaboon_Shield.h"
#include "DataStorage.h"
#include "RunningMoonBaboon.h"
#ifdef __TEST_JUNG
#include "Cody.h"
#endif // __TEST_JUNG


CEffect_MoonBaboon_Shield::CEffect_MoonBaboon_Shield(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_MoonBaboon_Shield::CEffect_MoonBaboon_Shield(const CEffect_MoonBaboon_Shield & rhs)
	: CInGameEffect_Model(rhs)
{
}


HRESULT CEffect_MoonBaboon_Shield::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_MoonBaboon_Shield::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.ModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_T_Ring"), TEXT("Com_Textures_Distortion"), (CComponent**)&m_pTexture_Distortion), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_T_Ring"), TEXT("Com_Textures_Distortion_Point"), (CComponent**)&m_pTexture_Distortion_Point), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Textures_ColorRamp"), (CComponent**)&m_pTexture_ColorRamp), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom"), TEXT("Com_Instancer"), (CComponent**)&m_pPointInstance), E_FAIL);

	Check_TargetMatrix();
	m_pTransformCom->Set_Scale(XMVectorSet(1.5f, 1.5f, 1.5f, 0.f));
	m_pInstanceBuffer = new VTXMATRIX_CUSTOM_ST;
	m_pInstanceBuffer->vSize = { 6.f, 6.f };
	m_pInstanceBuffer->vTextureUV = { 0.f, 0.f, 1.f, 1.f };
	m_pInstanceBuffer->vRight	= { 1.f, 0.f, 0.f, 0.f };
	m_pInstanceBuffer->vUp		= { 0.f, 1.f, 0.f, 0.f };
	m_pInstanceBuffer->vLook	= { 0.f, 0.f, 1.f, 0.f };

	return S_OK;
}

_int CEffect_MoonBaboon_Shield::Tick(_double TimeDelta)
{
	if (0.0 >= m_dLifeTime)
		return EVENT_DEAD;

	m_dLifeTime -= TimeDelta;
	if (2 > m_dLifeTime)
		m_fAlphaTime -= (_float)TimeDelta;

	m_fTime -= (_float)TimeDelta * 0.075f;
	if (0.f >= m_fTime)
		m_fTime = 1.f;

	if (0.f >= m_fAlphaTime)
		m_fAlphaTime = 0.f;

	Check_TargetMatrix();
	XMStoreFloat4(&m_pInstanceBuffer->vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pTransformCom->Set_Scale(XMVectorSet(1.9f, 1.9f, 1.9f, 0.f));
	m_pInstanceBuffer->vSize = { 9.75f, 9.75f };
	return _int();
}

_int CEffect_MoonBaboon_Shield::Late_Tick(_double TimeDelta)
{
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);

	return S_OK;
}

HRESULT CEffect_MoonBaboon_Shield::Render(RENDER_GROUP::Enum eGroup)
{
	_float fAlpha = m_fAlphaTime;
	if (1.f < fAlpha) fAlpha = 1.f;
	m_pModelCom->Set_Variable("g_vColor", &m_vColor, sizeof(_float4));
	m_pModelCom->Set_Variable("g_fTime", &m_fTime, sizeof(_float));
	m_pModelCom->Set_Variable("g_fAlpha", &fAlpha, sizeof(_float));
	m_pModelCom->Set_ShaderResourceView("g_ColorRampTexture", m_pTexture_ColorRamp->Get_ShaderResourceView(2));
	//m_pPointInstance->Set_ShaderResourceView("g_DiffuseTexture", m_pTexture_Distortion->Get_ShaderResourceView(1));
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(13);

	_int i = 1;
	m_pPointInstance->Set_DefaultVariables();
	m_pPointInstance->Set_Variable("g_IsBillBoard", &i, sizeof(_int));
	m_pPointInstance->Set_Variable("g_fAlpha", &fAlpha, sizeof(_float));
	m_pPointInstance->Set_ShaderResourceView("g_ColorTexture", m_pTexture_ColorRamp->Get_ShaderResourceView(2));
	m_pPointInstance->Set_ShaderResourceView("g_DiffuseTexture", m_pTexture_Distortion_Point->Get_ShaderResourceView(1));
	m_pPointInstance->Render(20, m_pInstanceBuffer, 1);

	return S_OK;
}

void CEffect_MoonBaboon_Shield::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

void CEffect_MoonBaboon_Shield::Check_TargetMatrix()
{
	_matrix WorldMatrix = static_cast<CRunningMoonBaboon*>(DATABASE->Get_RunningMoonBaboon())->Get_Transform()->Get_WorldMatrix();
	_matrix BoneMatrix = static_cast<CRunningMoonBaboon*>(DATABASE->Get_RunningMoonBaboon())->Get_Model()->Get_BoneMatrix("Spine1");

	WorldMatrix = BoneMatrix * WorldMatrix;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMatrix.r[3]);
}

CEffect_MoonBaboon_Shield * CEffect_MoonBaboon_Shield::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_MoonBaboon_Shield*	pInstance = new CEffect_MoonBaboon_Shield(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_MoonBaboon_Shield");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_MoonBaboon_Shield::Clone_GameObject(void * pArg)
{
	CEffect_MoonBaboon_Shield* pInstance = new CEffect_MoonBaboon_Shield(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_MoonBaboon_Shield");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_MoonBaboon_Shield::Free()
{
	Safe_Release(m_pTexture_Distortion_Point);
	Safe_Release(m_pTexture_Distortion);
	Safe_Release(m_pTexture_ColorRamp);
	Safe_Release(m_pPointInstance);

	Safe_Delete_Array(m_pInstanceBuffer);

	__super::Free();
}
