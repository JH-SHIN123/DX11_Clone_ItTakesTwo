#include "stdafx.h"
#include "..\Public\Effect_StarBuddy_Move.h"
#include "Effect_StarBuddy_Move_Particle.h"

CEffect_StarBuddy_Move::CEffect_StarBuddy_Move(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEffect_StarBuddy_Move::CEffect_StarBuddy_Move(const CEffect_StarBuddy_Move & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_StarBuddy_Move::NativeConstruct_Prototype()
{
	__super::NativeConstruct_Prototype();
	return S_OK;
}

HRESULT CEffect_StarBuddy_Move::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	/* ¸¸µé¾î */

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);

	m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_StarBuddy_Effect"), Level::LEVEL_STAGE, TEXT("GameObject_2D_StarBuddy_Move_Particle"), nullptr, (CGameObject**)&m_pParticle_1);
	m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_StarBuddy_Effect"), Level::LEVEL_STAGE, TEXT("GameObject_2D_StarBuddy_Move_Particle"), nullptr, (CGameObject**)&m_pParticle_2);
	m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_StarBuddy_Effect"), Level::LEVEL_STAGE, TEXT("GameObject_2D_StarBuddy_Move_Particle"), nullptr, (CGameObject**)&m_pParticle_3);
	m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_StarBuddy_Effect"), Level::LEVEL_STAGE, TEXT("GameObject_2D_StarBuddy_Move_Particle"), nullptr, (CGameObject**)&m_pParticle_4);
	m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_StarBuddy_Effect"), Level::LEVEL_STAGE, TEXT("GameObject_2D_StarBuddy_Move_Particle"), nullptr, (CGameObject**)&m_pParticle_5);

	m_IsLaunch = true;
	return S_OK;
}

_int CEffect_StarBuddy_Move::Tick(_double TimeDelta)
{
	if (true == m_isDead)
		return Dead_Func();

	if(true == m_IsLaunch)
		Set_Particle_Matrix();

	return _int();
}

_int CEffect_StarBuddy_Move::Late_Tick(_double TimeDelta)
{
	return _int();
}

HRESULT CEffect_StarBuddy_Move::Render(RENDER_GROUP::Enum eGroup)
{
	return S_OK;
}

void CEffect_StarBuddy_Move::Set_Particle_Matrix()
{
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_vector vPos = WorldMatrix.r[3];
	WorldMatrix.r[3] = vPos + WorldMatrix.r[1] * 0.8f;
	m_pParticle_1->Set_WorldMatrix(WorldMatrix);
	m_pParticle_1->Set_IsActivate(true);

	m_pTransformCom->RotateRoll_Angle(72.0);
	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[3] = vPos + WorldMatrix.r[1] * 0.8f;
	m_pParticle_2->Set_WorldMatrix(WorldMatrix);
	m_pParticle_2->Set_IsActivate(true);

	m_pTransformCom->RotateRoll_Angle(72.0);
	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[3] = vPos + WorldMatrix.r[1] * 0.8f;
	m_pParticle_3->Set_WorldMatrix(WorldMatrix);
	m_pParticle_3->Set_IsActivate(true);

	m_pTransformCom->RotateRoll_Angle(72.0);
	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[3] = vPos + WorldMatrix.r[1] * 0.8f;
	m_pParticle_4->Set_WorldMatrix(WorldMatrix);
	m_pParticle_4->Set_IsActivate(true);

	m_pTransformCom->RotateRoll_Angle(72.0);
	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[3] = vPos + WorldMatrix.r[1] * 0.8f;
	m_pParticle_5->Set_WorldMatrix(WorldMatrix);
	m_pParticle_5->Set_IsActivate(true);
}

_int CEffect_StarBuddy_Move::Dead_Func()
{
	m_pParticle_1->Set_Dead();
	m_pParticle_2->Set_Dead();
	m_pParticle_3->Set_Dead();
	m_pParticle_4->Set_Dead();
	m_pParticle_5->Set_Dead();

	return EVENT_DEAD;
}

void CEffect_StarBuddy_Move::Set_WorldMatrix(_fmatrix WorldMatrix)
{
	_matrix World = WorldMatrix;
	for (_int i = 0; i < 3; ++i)
		World.r[i] = XMVector3Normalize(World.r[i]);
	m_pTransformCom->Set_WorldMatrix(World);
}

void CEffect_StarBuddy_Move::Set_Launch(_bool IsLaunch)
{
	m_IsLaunch = IsLaunch;

	Set_Particle_Matrix();
}

void CEffect_StarBuddy_Move::Set_ColorIndex(_int iColorIndex)
{
	m_pParticle_1->Set_ColorIndex(iColorIndex);
	m_pParticle_2->Set_ColorIndex(iColorIndex);
	m_pParticle_3->Set_ColorIndex(iColorIndex);
	m_pParticle_4->Set_ColorIndex(iColorIndex);
	m_pParticle_5->Set_ColorIndex(iColorIndex);
}

CEffect_StarBuddy_Move * CEffect_StarBuddy_Move::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffect_StarBuddy_Move* pInstance = new CEffect_StarBuddy_Move(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CEffect_StarBuddy_Move");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_StarBuddy_Move::Clone_GameObject(void * pArg)
{
	CEffect_StarBuddy_Move* pInstance = new CEffect_StarBuddy_Move(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_StarBuddy_Move");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_StarBuddy_Move::Free()
{
	Safe_Release(m_pParticle_1);
	Safe_Release(m_pParticle_2);
	Safe_Release(m_pParticle_3);
	Safe_Release(m_pParticle_4);
	Safe_Release(m_pParticle_5);
	Safe_Release(m_pTransformCom);

	__super::Free();
}
