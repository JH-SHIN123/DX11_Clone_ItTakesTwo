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
	//FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT(""), TEXT("Com_Textures_Distortion"), (CComponent**)&m_pTexture_Distortion), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Textures_ColorRamp"), (CComponent**)&m_pTexture_ColorRamp), E_FAIL);

	Check_TargetMatrix();

	return S_OK;
}

_int CEffect_MoonBaboon_Shield::Tick(_double TimeDelta)
{
	if (0.0 >= m_dLifeTime)
		return EVENT_DEAD;

	m_dLifeTime -= TimeDelta;
	if (4 > m_dLifeTime)
		m_fAlphaTime -= (_float)TimeDelta * 0.333333f;

	m_fTime -= (_float)TimeDelta * 0.1f;
	if (0.f >= m_fTime)
		m_fTime = 1.f;

	if (0.f >= m_fAlphaTime)
		m_fAlphaTime = 0.f;

	Check_TargetMatrix();

	return _int();
}

_int CEffect_MoonBaboon_Shield::Late_Tick(_double TimeDelta)
{
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
	return S_OK;
}

HRESULT CEffect_MoonBaboon_Shield::Render(RENDER_GROUP::Enum eGroup)
{
	_float fAlpha = m_fAlphaTime;
	if (1.f < fAlpha) fAlpha = 1.f;

	m_pModelCom->Set_Variable("g_vColor", &m_vColor, sizeof(_float4));
	m_pModelCom->Set_Variable("g_fTime", &m_fTime, sizeof(_float));
	m_pModelCom->Set_Variable("g_fAlpha", &fAlpha, sizeof(_float));

	m_pModelCom->Set_ShaderResourceView("g_ColorRampTexture", m_pTexturesCom->Get_ShaderResourceView(2));
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(13);

	return S_OK;
}

void CEffect_MoonBaboon_Shield::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

void CEffect_MoonBaboon_Shield::Check_TargetMatrix()
{
	_matrix WorldMatrix = static_cast<CCody*>(DATABASE->GetCody())->Get_Transform()->Get_WorldMatrix();//static_cast<CRunningMoonBaboon*>(DATABASE->Get_RunningMoonBaboon())->Get_Transform()->Get_WorldMatrix();
	//_matrix BoneMatrix = static_cast<CRunningMoonBaboon*>(DATABASE->Get_RunningMoonBaboon())->Get_Model()->Get_BoneMatrix("Spine1");

	//WorldMatrix = BoneMatrix * WorldMatrix;
	//for (_int i = 0; i < 3; ++i)
	//	WorldMatrix.r[i] = XMVector3Normalize(WorldMatrix.r[i]);

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
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
	Safe_Release(m_pTexture_Distortion);
	Safe_Release(m_pTexture_ColorRamp);

	__super::Free();
}
