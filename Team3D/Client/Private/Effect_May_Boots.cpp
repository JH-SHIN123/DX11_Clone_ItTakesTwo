#include "stdafx.h"
#include "..\Public\Effect_May_Boots.h"
#include "GameInstance.h"
#include "Effect_May_Boots_Walking_Particle.h"

CEffect_May_Boots::CEffect_May_Boots(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_May_Boots::CEffect_May_Boots(const CEffect_May_Boots & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_May_Boots::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	m_EffectDesc_Prototype.iInstanceCount = m_iInstanceCount_Smoke;
	m_EffectDesc_Prototype.fLifeTime = 0.f;

	return S_OK;
}

HRESULT CEffect_May_Boots::NativeConstruct(void * pArg)
{
	Ready_Component(pArg);

	return S_OK;
}

_int CEffect_May_Boots::Tick(_double TimeDelta)
{
	//상황은 네가지
	// 1. 중력발판을 이제 밟았다 (0번부터 다시 뿌리쟝)
	// 2. 중력발판을 밟고 있다 (계속 뿌림)
	// 3. 중력발판에서 나왔다 (리스폰해야 하는 대상을 제외한 나머지 애들만 뿌린다)
	// 4. 안밟았다 (렌더링X)

	m_IsRendering = false;
	
	XMStoreFloat4x4(&m_BoneMatrix_Left,		m_pTargetModel->Get_BoneMatrix("LeftToeBase"));
	XMStoreFloat4x4(&m_BoneMatrix_Right,	m_pTargetModel->Get_BoneMatrix("RightToeBase"));

	Update_Effect(TimeDelta);


	return NO_EVENT;
}

_int CEffect_May_Boots::Late_Tick(_double TimeDelta)
{
	m_IsActivateStart = false;

	m_IsActivate = false;

	if (false == m_IsRendering)
	{
		Reset_UpdateTime();
		return NO_EVENT;
	}

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_ALPHA, this);
}

HRESULT CEffect_May_Boots::Render(RENDER_GROUP::Enum eGroup)
{
	Set_Shader_Data();

	_float4 vColor = { 0.529411793f, 0.807843208f, 0.980392218f, 1.000000000f }; //LightSkyBlue
	m_pPointInstanceCom_STT->Set_Variable("g_vColor", &vColor, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Render(1, m_pInstanceBuffer_Smoke, m_iInstanceCount_Smoke);

	return S_OK;
}

void CEffect_May_Boots::Set_Model(CModel * pModel)
{
	m_pTargetModel = pModel;
	Safe_AddRef(m_pTargetModel);

	Ready_Instance();
}

void CEffect_May_Boots::Update_Matrix(_fmatrix WorldMatrix)
{
	_matrix NormalizeMatrix = WorldMatrix;
	for (_int i = 0; i < 3; ++i)
		NormalizeMatrix.r[i] = XMVector3Normalize(NormalizeMatrix.r[i]);

	m_pTransformCom->Set_WorldMatrix(NormalizeMatrix);
}

void CEffect_May_Boots::Set_IsActivate_GravityBoots()
{
	if(false == m_IsActivate && false == m_IsRendering)
		m_IsActivateStart = true;

	m_IsActivate = true;
}

void CEffect_May_Boots::Add_WalkingParticle(_bool IsRightFoot)
{
	EFFECT_DESC_CLONE Clone_Data;
	_matrix ParentMatrix = XMMatrixIdentity();

	if (false == IsRightFoot)
		ParentMatrix = XMLoadFloat4x4(&m_BoneMatrix_Left) * m_pTransformCom->Get_WorldMatrix();
	else
		ParentMatrix = XMLoadFloat4x4(&m_BoneMatrix_Right) * m_pTransformCom->Get_WorldMatrix();


	XMStoreFloat4x4(&Clone_Data.WorldMatrix, ParentMatrix);

	m_pGameInstance->Add_GameObject_Clone(1, TEXT("Layer_Effect"), 1, TEXT("GameObject_2D_May_Boots_Walking_Particle"), &Clone_Data);
}

void CEffect_May_Boots::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_May_Boots::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_May_Boots::Instance_UV(_float TimeDelta, _int iIndex)
{
}

void CEffect_May_Boots::Update_Effect(_double TimeDelta)
{
	_int iInstance_Bone = m_iInstanceCount_Smoke >> 1;
	Update_Smoke(TimeDelta, 0, iInstance_Bone, m_BoneMatrix_Left);
	Update_Smoke(TimeDelta, iInstance_Bone, m_iInstanceCount_Smoke, m_BoneMatrix_Right);

	Update_Particle(TimeDelta);

}

void CEffect_May_Boots::Update_Smoke(_double TimeDelta, _int iStartIndex, _int iEndIndex, _float4x4 ParentMatrix4x4)
{
	_matrix ParentMatrix = XMLoadFloat4x4(&ParentMatrix4x4) * m_pTransformCom->Get_WorldMatrix();

	for (_int iIndex = iStartIndex; iIndex < iEndIndex; ++iIndex)
	{
		if (0.0 < m_pInstance_RenderTerm[iIndex])
		{
			m_pInstance_RenderTerm[iIndex] -= TimeDelta;
			m_pInstanceBuffer_Smoke[iIndex].fTime = 0.f;

			if(false == m_IsActivateStart)
				continue;
		}

		m_pInstance_Pos_UpdateTime[iIndex]	-= TimeDelta;
		if (0 >= m_pInstance_Pos_UpdateTime[iIndex])
		{
			if(EVENT_DEAD == Respawn_Smoke(iIndex))
				continue;
		}

		_double dTime = m_dRespawnTime_Smoke * 0.5f;
		if (dTime <= m_pInstance_Pos_UpdateTime[iIndex])
			m_pInstanceBuffer_Smoke[iIndex].fTime += (_float)TimeDelta * 0.5f;
		else
			m_pInstanceBuffer_Smoke[iIndex].fTime -= (_float)TimeDelta * 0.5f;


		if (0.0001f >= m_pInstanceBuffer_Smoke[iIndex].vSize.x)
			m_pInstanceBuffer_Smoke[iIndex].vSize = m_vDefaultSize_Smoke;

		_vector vPos = XMLoadFloat4(&m_pInstance_LocalPos[iIndex]);
		_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);
		vPos -= vDir * 20.f * (_float)TimeDelta;

		XMStoreFloat4(&m_pInstance_LocalPos[iIndex], vPos);
		vPos = XMVector3Transform(vPos, ParentMatrix);
		XMStoreFloat4(&m_pInstanceBuffer_Smoke[iIndex].vPosition, vPos);

		m_IsRendering = true;
	}
}

void CEffect_May_Boots::Update_Particle(_double TimeDelta)
{
}

_int CEffect_May_Boots::Respawn_Smoke(_int iIndex)
{
	if (true == m_IsActivate)
	{
		m_pInstance_LocalPos[iIndex] = { 0.f, 0.f, 0.f, 1.f };
		m_pInstance_Pos_UpdateTime[iIndex]	= m_dRespawnTime_Smoke;
		m_pInstance_RenderTerm[iIndex] = 0.0;
		m_pInstanceBuffer_Smoke[iIndex].fTime = 0.0f;
		XMStoreFloat3(&m_pInstance_Dir[iIndex], Get_RandDir());
		return NO_EVENT;
	}
	else
	{
		m_pInstance_LocalPos[iIndex] = { 0.f, 0.f, 0.f, 1.f };

 		m_pInstance_RenderTerm[iIndex] = m_dRespawnTime_Smoke; 	
		m_pInstanceBuffer_Smoke[iIndex].fTime = 0.0f;
		m_pInstanceBuffer_Smoke[iIndex].vSize = { 0.f, 0.f };
		return EVENT_DEAD;
	}
}

_vector CEffect_May_Boots::Get_RandDir()
{
	_float fRandDir[3] = { 0.f, 0.f, 0.f };

	for (_int i = 0; i < 3; ++i)
	{
		fRandDir[i] = _float(rand() % 10 + 1);
		if (rand() % 2 == 0)
			fRandDir[i] *= -1.f;
	}
	
	_vector vRandDir =  XMVectorSet(fRandDir[0] , fRandDir[1] , fRandDir[2], 0.f);

	return XMVector3Normalize(vRandDir);
}

void CEffect_May_Boots::Set_Shader_Data()
{
	CPipeline* pPipeline = CPipeline::GetInstance();
	if (nullptr == pPipeline)
		return;

	m_pPointInstanceCom_STT->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pPointInstanceCom_STT->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINVIEW)), sizeof(_matrix));
	m_pPointInstanceCom_STT->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINPROJ)), sizeof(_matrix));
	m_pPointInstanceCom_STT->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBVIEW)), sizeof(_matrix));
	m_pPointInstanceCom_STT->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBPROJ)), sizeof(_matrix));

	_float	fMainCamFar = pPipeline->Get_MainCamFar();
	_float	fSubCamFar = pPipeline->Get_SubCamFar();
	m_pPointInstanceCom_STT->Set_Variable("g_fMainCamFar", &fMainCamFar, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_fSubCamFar", &fSubCamFar, sizeof(_float));

	_vector vMainCamPosition = pPipeline->Get_MainCamPosition();
	_vector vSubCamPosition = pPipeline->Get_SubCamPosition();
	m_pPointInstanceCom_STT->Set_Variable("g_vMainCamPosition", &vMainCamPosition, sizeof(_vector));
	m_pPointInstanceCom_STT->Set_Variable("g_vSubCamPosition", &vSubCamPosition, sizeof(_vector));

	return;
}

void CEffect_May_Boots::Reset_UpdateTime()
{
	_float fInstanceCountHalf = m_iInstanceCount_Smoke * 0.5f;
	for (_int iIndex = 0; iIndex < m_iInstanceCount_Smoke; ++iIndex)
	{
// 		m_pInstance_Pos_UpdateTime[iIndex] = m_dRespawnTime_Smoke * (_float)iIndex;
// 		if (iIndex >= m_iInstanceCount_Smoke >> 1)
// 			m_pInstance_Pos_UpdateTime[iIndex] = m_dRespawnTime_Smoke * _float(iIndex - (m_iInstanceCount_Smoke >> 1));

		m_pInstanceBuffer_Smoke[iIndex].vPosition	= { 0.f, 0.f, 0.f, 1.f };
		m_pInstanceBuffer_Smoke[iIndex].vSize		= m_vDefaultSize_Smoke;
		m_pInstanceBuffer_Smoke[iIndex].vTextureUV	= { 0.f, 0.f, 1.f, 1.f };
		m_pInstanceBuffer_Smoke[iIndex].fTime		= 0.0f;
		m_pInstance_LocalPos[iIndex]				= { 0.f, 0.f, 0.f, 1.f };
		m_pInstance_Pos_UpdateTime[iIndex]			= m_dRespawnTime_Smoke;
		m_pInstance_RenderTerm[iIndex]				= m_dRespawnTime_Smoke / fInstanceCountHalf * _float(iIndex - fInstanceCountHalf) + 0.001;
		if (0 >= m_pInstance_RenderTerm[iIndex])
			m_pInstance_RenderTerm[iIndex] *= -1.f;
	}
}

HRESULT CEffect_May_Boots::Ready_Component(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.TextureName, TEXT("Com_Textrue"), (CComponent**)&m_pTexturesCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom_STT"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom_STT), E_FAIL);


	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);

	for (_int i = 0; i < 3; ++i)
		WolrdMatrix.r[i] = XMVector3Normalize(WolrdMatrix.r[i]);

	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);

	return S_OK;
}

HRESULT CEffect_May_Boots::Ready_Instance()
{
	if (nullptr == m_pPointInstanceCom_STT)
		return E_FAIL;
	// Smoke

	_int iInstanceCount			= m_iInstanceCount_Smoke;
	m_pInstanceBuffer_Smoke		= new VTXMATRIX_CUSTOM_STT[iInstanceCount];
	m_pInstance_Dir				= new _float3[iInstanceCount];
	m_pInstance_LocalPos		= new _float4[iInstanceCount];
	m_pInstance_Pos_UpdateTime	= new _double[iInstanceCount];
	m_pInstance_RenderTerm		= new _double[iInstanceCount];

	_float4 vPos		= { 0.f, 0.f, 0.f, 1.f };
	_vector vDir		= XMLoadFloat3(&m_EffectDesc_Clone.vDir);
	_float2 vSize		= { m_EffectDesc_Prototype.vSize.x ,m_EffectDesc_Prototype.vSize.y };
	_double	dUpdateTerm = 0.0;
	_float fInstanceCountHalf = m_iInstanceCount_Smoke * 0.5f;

	for (_int i = 0; i < iInstanceCount; ++i)
	{
		dUpdateTerm = /*0.25 **/ (_double)i;
		if (i >= iInstanceCount >> 1)
			dUpdateTerm = _double(i - (iInstanceCount >> 1));

		m_pInstanceBuffer_Smoke[i].vRight		= { 1.f, 0.f, 0.f, 0.f };
		m_pInstanceBuffer_Smoke[i].vUp			= { 0.f, 1.f, 0.f, 0.f };
		m_pInstanceBuffer_Smoke[i].vLook		= { 0.f, 0.f, 1.f, 0.f };
		m_pInstanceBuffer_Smoke[i].vPosition	= vPos;
		m_pInstanceBuffer_Smoke[i].vSize		= m_vDefaultSize_Smoke;
		m_pInstanceBuffer_Smoke[i].vTextureUV	= { 0.f, 0.f, 1.f, 1.f };
		m_pInstanceBuffer_Smoke[i].fTime		= 0.0f;
		m_pInstance_LocalPos[i]					= vPos;
		m_pInstance_Pos_UpdateTime[i]			= 0.0;
		XMStoreFloat3(&m_pInstance_Dir[i], Get_RandDir());

		m_pInstance_RenderTerm[i] = m_dRespawnTime_Smoke / fInstanceCountHalf * _float(i - fInstanceCountHalf) + 0.001;
		if (0 >= m_pInstance_RenderTerm[i])
			m_pInstance_RenderTerm[i] *= -1.f;
	}

	return S_OK;
}

CEffect_May_Boots * CEffect_May_Boots::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_May_Boots*	pInstance = new CEffect_May_Boots(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_May_Boots");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_May_Boots::Clone_GameObject(void * pArg)
{
	CEffect_May_Boots* pInstance = new CEffect_May_Boots(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_May_Boots");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_May_Boots::Free()
{
	Safe_Delete_Array(m_pInstance_Pos_UpdateTime);
	Safe_Delete_Array(m_pInstanceBuffer_Smoke);
	Safe_Delete_Array(m_pInstance_LocalPos);

	Safe_Release(m_pPointInstanceCom_STT);
	Safe_Release(m_pTargetModel);

	__super::Free();
}
