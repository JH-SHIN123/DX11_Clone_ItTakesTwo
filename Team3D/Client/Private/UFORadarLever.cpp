#include "stdafx.h"
#include "..\public\UFORadarLever.h"
#include "Effect_Generator.h"

CUFORadarLever::CUFORadarLever(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUFORadarSet(pDevice, pDeviceContext)
{
}

CUFORadarLever::CUFORadarLever(const CUFORadarSet & rhs)
	: CUFORadarSet(rhs)
{
}

HRESULT CUFORadarLever::NativeConstruct_Prototype()
{
	CUFORadarSet::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CUFORadarLever::NativeConstruct(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_UFORadarLever"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	
	if (nullptr != pArg)
		memcpy(&m_tRadarLeverDesc, (RADARDESC*)pArg, sizeof(RADARDESC));

	_vector vPosition = m_tRadarLeverDesc.vPosition;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_pModelCom->Set_Animation(1);
	return S_OK;
}

_int CUFORadarLever::Tick(_double dTimeDelta)
{
	CUFORadarSet::Tick(dTimeDelta);

	Rotate_JoyStick(dTimeDelta);

	m_pModelCom->Update_Animation(dTimeDelta);
	return NO_EVENT;
}

_int CUFORadarLever::Late_Tick(_double dTimeDelta)
{
	CUFORadarSet::Late_Tick(dTimeDelta);
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CUFORadarLever::Render(RENDER_GROUP::Enum eGroup)
{
	CUFORadarSet::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0);

	return S_OK;
}

HRESULT CUFORadarLever::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(2, 0, true);

	return S_OK;
}

void CUFORadarLever::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

void CUFORadarLever::Rotate_JoyStick(_double dTimeDelta)
{
	_matrix	 matPivot, matScale, matTrans, matRotX, matRotZ, matParent, matAnim = XMMatrixIdentity();
	
	if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		if (m_fAngleZ > -20.f)
			m_fAngleZ -= (_float)dTimeDelta * 100.f;
		

		matTrans = XMMatrixTranslation(0.f, 0.f, -60.f);
		matRotZ = XMMatrixRotationY(XMConvertToRadians(m_fAngleZ));
		matPivot = matRotZ * matTrans;

		matAnim = m_pModelCom->Get_AnimTransformation(10);
		matAnim = XMMatrixInverse(nullptr, matAnim);

		matPivot *= matAnim;
		m_pModelCom->Set_PivotTransformation(10, matPivot);
	}
	else if (m_pGameInstance->Key_Pressing(DIK_A))
	{

		if (m_fAngleX < 20.f)
			m_fAngleX += (_float)dTimeDelta * 100.f;

		matTrans = XMMatrixTranslation(0.f, 0.f, -60.f);
		matRotX = XMMatrixRotationX(XMConvertToRadians(m_fAngleX));
		matPivot = matRotX * matTrans;

		matAnim = m_pModelCom->Get_AnimTransformation(10);
		matAnim = XMMatrixInverse(nullptr, matAnim);

		matPivot *= matAnim;
		m_pModelCom->Set_PivotTransformation(10, matPivot);

	}
	else if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		if (m_fAngleZ < 20.f)
			m_fAngleZ += (_float)dTimeDelta * 100.f;

		matTrans = XMMatrixTranslation(0.f, 0.f, -60.f);
		matRotZ = XMMatrixRotationY(XMConvertToRadians(m_fAngleZ));
		matPivot = matRotZ * matTrans;

		matAnim = m_pModelCom->Get_AnimTransformation(10);
		matAnim = XMMatrixInverse(nullptr, matAnim);

		matPivot *= matAnim;
		m_pModelCom->Set_PivotTransformation(10, matPivot);
	}
	else if (m_pGameInstance->Key_Pressing(DIK_D))
	{
		if (m_fAngleX > -20.f)
			m_fAngleX -= (_float)dTimeDelta * 100.f;

		matTrans = XMMatrixTranslation(0.f, 0.f, -60.f);
		matRotX = XMMatrixRotationX(XMConvertToRadians(m_fAngleX));
		matPivot = matRotX * matTrans;

		matAnim = m_pModelCom->Get_AnimTransformation(10);
		matAnim = XMMatrixInverse(nullptr, matAnim);

		matPivot *= matAnim;
		m_pModelCom->Set_PivotTransformation(10, matPivot);

	}
	else
	{
		// X
		if (m_fAngleX > 2.f)
		{
			m_fAngleX -= (_float)dTimeDelta * 100.f;
			matTrans = XMMatrixTranslation(0.f, 0.f, -60.f);
			matRotX = XMMatrixRotationX(XMConvertToRadians(m_fAngleX));
			matPivot = matRotX * matTrans;

			matAnim = m_pModelCom->Get_AnimTransformation(10);
			matAnim = XMMatrixInverse(nullptr, matAnim);

			matPivot *= matAnim;
			m_pModelCom->Set_PivotTransformation(10, matPivot);
		}
		else if (m_fAngleX < -2.f)
		{
			m_fAngleX += (_float)dTimeDelta * 100.f;
			matTrans = XMMatrixTranslation(0.f, 0.f, -60.f);
			matRotX = XMMatrixRotationX(XMConvertToRadians(m_fAngleX));
			matPivot = matRotX * matTrans;

			matAnim = m_pModelCom->Get_AnimTransformation(10);
			matAnim = XMMatrixInverse(nullptr, matAnim);

			matPivot *= matAnim;
			m_pModelCom->Set_PivotTransformation(10, matPivot);
		}
		else if (m_fAngleX >= -2.f && m_fAngleX <= 2.f && m_fAngleX != 0.f)
		{
			m_fAngleX = 0.f;
			matTrans = XMMatrixTranslation(0.f, 0.f, -60.f);
			matRotX = XMMatrixRotationX(XMConvertToRadians(m_fAngleX));
			matPivot = matRotX * matTrans;

			matAnim = m_pModelCom->Get_AnimTransformation(10);
			matAnim = XMMatrixInverse(nullptr, matAnim);

			matPivot *= matAnim;
			m_pModelCom->Set_PivotTransformation(10, matPivot);

		}
		// Z
		else if (m_fAngleZ > 2.f)
		{
			m_fAngleZ -= (_float)dTimeDelta * 100.f;
			matTrans = XMMatrixTranslation(0.f, 0.f, -60.f);
			matRotZ = XMMatrixRotationY(XMConvertToRadians(m_fAngleZ));
			matPivot = matRotZ * matTrans;

			matAnim = m_pModelCom->Get_AnimTransformation(10);
			matAnim = XMMatrixInverse(nullptr, matAnim);

			matPivot *= matAnim;
			m_pModelCom->Set_PivotTransformation(10, matPivot);
		}
		else if (m_fAngleZ < -2.f)
		{
			m_fAngleZ += (_float)dTimeDelta * 100.f;
			matTrans = XMMatrixTranslation(0.f, 0.f, -60.f);
			matRotZ = XMMatrixRotationY(XMConvertToRadians(m_fAngleZ));
			matPivot = matRotZ * matTrans;

			matAnim = m_pModelCom->Get_AnimTransformation(10);
			matAnim = XMMatrixInverse(nullptr, matAnim);

			matPivot *= matAnim;
			m_pModelCom->Set_PivotTransformation(10, matPivot);
		}
		else if (m_fAngleZ >= -2.f && m_fAngleZ <= 2.f && m_fAngleZ != 0.f)
		{
			m_fAngleZ = 0.f;
			matTrans = XMMatrixTranslation(0.f, 0.f, -60.f);
			matRotZ = XMMatrixRotationY(XMConvertToRadians(m_fAngleZ));
			matPivot = matRotZ * matTrans;

			matAnim = m_pModelCom->Get_AnimTransformation(10);
			matAnim = XMMatrixInverse(nullptr, matAnim);

			matPivot *= matAnim;
			m_pModelCom->Set_PivotTransformation(10, matPivot);
		}
	}
}

CUFORadarLever * CUFORadarLever::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUFORadarLever* pInstance = new CUFORadarLever(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CUFORadarLever");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUFORadarLever::Clone_GameObject(void * pArg)
{
	CUFORadarLever* pInstance = new CUFORadarLever(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CUFORadarLever");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUFORadarLever::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}