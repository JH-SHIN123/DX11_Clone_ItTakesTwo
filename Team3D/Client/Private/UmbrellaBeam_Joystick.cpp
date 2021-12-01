#include "stdafx.h"
#include "..\public\UmbrellaBeam_Base.h"
#include "Cody.h"
#include "May.h"
#include "DataStorage.h"

CUmbrellaBeam_Base::CUmbrellaBeam_Base(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CUmbrellaBeam_Base::CUmbrellaBeam_Base(const CUmbrellaBeam_Base & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUmbrellaBeam_Base::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CUmbrellaBeam_Base::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_UmbrellaBeam_Base"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-789.319824f, 766.982971f, 189.852661f, 1.f));

	m_UserData = USERDATA(GameID::eUMBRELLABEAMBASE, this);

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);


	return S_OK;
}

_int CUmbrellaBeam_Base::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	//CCody* pCody = (CCody*)DATABASE->GetCody();
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, pCody->Get_Position());

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-789.319824f, 766.982971f, 189.852661f, 1.f));
	if (m_pGameInstance->Key_Pressing(DIK_K))
	{
		m_fAngle += (_float)dTimeDelta * 5.f;
		m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-m_fAngle));
	}

	return NO_EVENT;
}

_int CUmbrellaBeam_Base::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CUmbrellaBeam_Base::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CUmbrellaBeam_Base::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{

}

HRESULT CUmbrellaBeam_Base::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

CUmbrellaBeam_Base * CUmbrellaBeam_Base::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUmbrellaBeam_Base* pInstance = new CUmbrellaBeam_Base(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CUmbrellaBeam_Base");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUmbrellaBeam_Base::Clone_GameObject(void * pArg)
{
	CUmbrellaBeam_Base* pInstance = new CUmbrellaBeam_Base(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CUmbrellaBeam_Base");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUmbrellaBeam_Base::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}