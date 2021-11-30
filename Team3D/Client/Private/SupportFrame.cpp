#include "stdafx.h"
#include "..\public\SupportFrame.h"
#include "Cody.h"
#include "May.h"
#include "PressurePlateLock.h"

CSupportFrame::CSupportFrame(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSupportFrame::CSupportFrame(const CSupportFrame & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSupportFrame::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CSupportFrame::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_iOption, pArg, sizeof(_uint));
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_SupportFrame"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	/* Option 0 : BigFlate 프레임 / Option 1 : Plate 프레임 */
	if(1 == m_iOption)
		FAILED_CHECK_RETURN(Ready_Layer_PlateLock(TEXT("Layer_PressurePlateLock"), 4), E_FAIL);

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	return S_OK;
}

_int CSupportFrame::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	SetUp_DefaultPositionSetting();

	return NO_EVENT;
}

_int CSupportFrame::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CSupportFrame::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CSupportFrame::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{

}

void CSupportFrame::Set_Position(_vector vPosition)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

void CSupportFrame::Set_LockActive(_bool IsCheck)
{
	if (true == m_vecPressurePlateLock.empty())
		return;

	for (auto pPressurePlateLock : m_vecPressurePlateLock)
		pPressurePlateLock->Set_LockActive(IsCheck);
}

void CSupportFrame::SetUp_DefaultPositionSetting()
{
	if (true == m_vecPressurePlateLock.empty() || false == m_IsStart)
		return;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4 vConvertPos;

	XMStoreFloat4(&vConvertPos, vPos);
	vConvertPos.x += 1.f;
	vConvertPos.z += 1.f;
	m_vecPressurePlateLock[0]->Set_Position(XMLoadFloat4(&vConvertPos));

	XMStoreFloat4(&vConvertPos, vPos);
	vConvertPos.x -= 1.f;
	vConvertPos.z += 1.f;
	m_vecPressurePlateLock[1]->Set_Position(XMLoadFloat4(&vConvertPos));

	XMStoreFloat4(&vConvertPos, vPos);
	vConvertPos.x -= 1.f;
	vConvertPos.z -= 1.f;
	m_vecPressurePlateLock[2]->Set_Position(XMLoadFloat4(&vConvertPos));

	XMStoreFloat4(&vConvertPos, vPos);
	vConvertPos.x += 1.f;
	vConvertPos.z -= 1.f;
	m_vecPressurePlateLock[3]->Set_Position(XMLoadFloat4(&vConvertPos));

	m_IsStart = false;
}

HRESULT CSupportFrame::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

HRESULT CSupportFrame::Ready_Layer_PlateLock(const _tchar * pLayerTag, _uint iCount)
{
	CGameObject* pGameObject = nullptr;
	m_vecPressurePlateLock.reserve(iCount);
	_uint iOption = 1;

	for (_uint i = 0; i < iCount; ++i)
	{
		FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_PressurePlateLock"), &iOption, &pGameObject), E_FAIL);
		m_vecPressurePlateLock.emplace_back(static_cast<CPressurePlateLock*>(pGameObject));
	}

	return S_OK;
}


CSupportFrame * CSupportFrame::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSupportFrame* pInstance = new CSupportFrame(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CSupportFrame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSupportFrame::Clone_GameObject(void * pArg)
{
	CSupportFrame* pInstance = new CSupportFrame(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CSupportFrame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSupportFrame::Free()
{
	for (auto pPressurePlate : m_vecPressurePlateLock)
		Safe_Release(pPressurePlate);

	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}