#include "stdafx.h"
#include "..\public\Performer.h"
#include "GameInstance.h"
#include "DataStorage.h"
#include"CutScenePlayer.h"
#ifdef _AFX
#include"PerformerOption.h"
#include"MainForm.h"
#endif
CPerformer::CPerformer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CPerformer::CPerformer(const CPerformer & rhs)
	: CGameObject(rhs)
{

}

HRESULT CPerformer::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPerformer::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);


	memcpy(&m_tDesc, pArg, sizeof(PERFORMERDESC));
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_tDesc.strModelTag.c_str(), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	

	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_tDesc.vScale));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_tDesc.vPosition), 1.f));

	m_pModelTag = new wstring(m_tDesc.strModelTag);
	

	m_pModelCom->Set_Animation(0);
	return S_OK;
}

_int CPerformer::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);



	if (m_bIsOnParentBone)
	{
		if (*m_pModelTag == TEXT("Component_Model_SizeBeltCutScene1"))
		{
#ifdef _DEBUG
			_float3 vPos,vRot;
			TCHAR szBuff[256] = L"";
			GetPrivateProfileString(L"Section_1", L"Key_1", L"0", szBuff, 256, L"../test.ini");
			_float x = _ttof(szBuff);

			GetPrivateProfileString(L"Section_1", L"Key_2", L"0", szBuff, 256, L"../test.ini");
			_float y = _ttof(szBuff);

			GetPrivateProfileString(L"Section_1", L"Key_3", L"0", szBuff, 256, L"../test.ini");
			_float z = _ttof(szBuff);

			GetPrivateProfileString(L"Section_1", L"Key_7", L"0", szBuff, 256, L"../test.ini");
			vRot.x = _ttof(szBuff);

			GetPrivateProfileString(L"Section_1", L"Key_8", L"0", szBuff, 256, L"../test.ini");
			vRot.y = _ttof(szBuff);

			GetPrivateProfileString(L"Section_1", L"Key_9", L"0", szBuff, 256, L"../test.ini");
			vRot.z = _ttof(szBuff);

			vPos = { x, y, z };
#endif
#ifdef __TEST_JUN
			if (m_pGameInstance->Key_Pressing(DIK_W))
				m_vPos.z += dTimeDelta;
			if (m_pGameInstance->Key_Pressing(DIK_S))
				m_vPos.z -= dTimeDelta;
			if (m_pGameInstance->Key_Pressing(DIK_A))
				m_vPos.x -= dTimeDelta;
			if (m_pGameInstance->Key_Pressing(DIK_D))
				m_vPos.x += dTimeDelta;
			if (m_pGameInstance->Key_Pressing(DIK_Q))
				m_vPos.y += dTimeDelta;
			if (m_pGameInstance->Key_Pressing(DIK_E))
				m_vPos.y -= dTimeDelta;
			if (m_pGameInstance->Key_Pressing(DIK_Z))
				m_vRot.x += dTimeDelta;
			if (m_pGameInstance->Key_Pressing(DIK_X))
				m_vRot.x -= dTimeDelta;
			if (m_pGameInstance->Key_Pressing(DIK_C))
				m_vRot.y += dTimeDelta;
			if (m_pGameInstance->Key_Pressing(DIK_V))
				m_vRot.y -= dTimeDelta;
			if (m_pGameInstance->Key_Pressing(DIK_B))
				m_vRot.z += dTimeDelta;
			if (m_pGameInstance->Key_Pressing(DIK_N))
				m_vRot.z -= dTimeDelta;

			_matrix matPar = m_pParentModel->Get_BoneMatrix(m_szParentBoneTag);
			_vector vParPos = m_pParentTransform->Get_State(CTransform::STATE_POSITION);
			matPar.r[3] = vParPos + XMVectorSet(m_vPos.x, m_vPos.y, m_vPos.z,0.f);
			m_pTransformCom->Set_WorldMatrix(XMMatrixScaling(100.f*m_pParentTransform->Get_Scale(CTransform::STATE_RIGHT),
				100.f*m_pParentTransform->Get_Scale(CTransform::STATE_UP),
				100.f*m_pParentTransform->Get_Scale(CTransform::STATE_LOOK)) *
				XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_vRot.z), 
					XMConvertToRadians(m_vRot.x), 
					XMConvertToRadians(m_vRot.y))* XMMatrixTranslation(m_vPos.x, m_vPos.y,m_vPos.z) * matPar);
		
#endif
		}
	}
	else
	{
		_matrix matWorld = XMMatrixIdentity();

		matWorld = XMMatrixScaling(m_tDesc.vScale.x, m_tDesc.vScale.y, m_tDesc.vScale.z) *
			XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_tDesc.vRot.z), XMConvertToRadians(m_tDesc.vRot.x), XMConvertToRadians(m_tDesc.vRot.y))*
			XMMatrixTranslation(m_tDesc.vPosition.x, m_tDesc.vPosition.y, m_tDesc.vPosition.z);
		m_pTransformCom->Set_WorldMatrix(matWorld);
	}
	
#ifdef __TEST_JUN
	if (m_pGameInstance->Key_Down(DIK_NUMPADENTER))
		m_bStartAnim = !m_bStartAnim;
#endif
	
	m_pModelCom->Update_Animation(m_bStartAnim ? dTimeDelta : 0.0/*CCutScenePlayer::GetInstance()->Get_TimeDelta()*/);

	return NO_EVENT;
}

_int CPerformer::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 30.f))
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
	return NO_EVENT;
}






HRESULT CPerformer::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0);

	return S_OK;
}

void CPerformer::Set_TransformToParentBone(CTransform* pParentTransform, CModel * pParentModel, const char* szBoneTag)
{
	m_bStartAnim = false;
	m_pParentTransform = pParentTransform;
	m_bIsOnParentBone = true;
	m_pParentModel = pParentModel;
	strcpy_s(m_szParentBoneTag, szBoneTag);
}

void CPerformer::Start_Perform(_uint iAnimIdx, _double dAnimTime)
{
	m_pModelCom->Set_Animation(iAnimIdx,dAnimTime);
	m_pParentModel = nullptr;
	m_pParentTransform = nullptr;
	m_bIsOnParentBone = false;

	m_bStartAnim = true;

}

void CPerformer::Finish_Perform()
{
	m_bStartAnim = false;
}


CPerformer * CPerformer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPerformer* pInstance = new CPerformer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPerformer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPerformer::Clone_GameObject(void * pArg)
{
	CPerformer* pInstance = new CPerformer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPerformer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPerformer::Free()
{
	Safe_Delete(m_pModelTag);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}