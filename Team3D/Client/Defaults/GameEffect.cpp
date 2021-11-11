#include "stdafx.h"
#include "GameEffect.h"
#include "GameInstance.h"

CGameEffect::CGameEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CGameEffect::CGameEffect(const CGameEffect & rhs)
	: CGameObject(rhs)
	, m_EffectDesc_Prototype(rhs.m_EffectDesc_Prototype)
{
}

HRESULT CGameEffect::NativeConstruct_Prototype(void* pArg)
{
	memcpy(&m_EffectDesc_Prototype, pArg, sizeof(EFFECT_DESC_PROTO));
	
	return S_OK;
}

HRESULT CGameEffect::NativeConstruct(void * pArg)
{
	Ready_Component(pArg);
	Check_ChangeData();
	Ready_InstanceBuffer();

	return S_OK;
}

_int CGameEffect::Tick(_double TimeDelta)
{
	return _int();
}

_int CGameEffect::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CGameEffect::Render()
{
	_double TimeDelta = m_pGameInstance->Compute_TimeDelta(TEXT("Timer_60"));
	

	Check_Color(TimeDelta);
	Check_Gravity(TimeDelta);
	Control_InstanceBuffer(TimeDelta);

	SetUp_Shader_Data();

	m_pPointInstanceCom->Render(0, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);


	return S_OK;
}

void CGameEffect::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CGameEffect::Instance_Pos(_float TimeDelta, _int iIndex)
{
}


HRESULT CGameEffect::Copy_Prototype_Desc(void * pArg)
{


	return S_OK;
}

HRESULT CGameEffect::Ready_Component(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	for (_int i = 0; i < RESOURCE_END; ++i)
		m_IsResourceName[i] = false;

	if (1 < lstrlen(m_EffectDesc_Prototype.TextureName))
		m_IsResourceName[RESOURCE_TEXTURE] = true;
	if (1 < lstrlen(m_EffectDesc_Prototype.TextureName_Second))
		m_IsResourceName[RESOURCE_TEXTURE_SECOND] = true;
	if (1 < lstrlen(m_EffectDesc_Prototype.ModelName))
		m_IsResourceName[RESOURCE_MESH] = true;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	if (true == m_IsResourceName[RESOURCE_TEXTURE])
		FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.TextureName, TEXT("Com_Textrue"), (CComponent**)&m_pTexturesCom), E_FAIL);
	if (true == m_IsResourceName[RESOURCE_TEXTURE_SECOND])
		FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.TextureName_Second, TEXT("Com_Textrue_Second"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	if (true == m_IsResourceName[RESOURCE_MESH])
		FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.ModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	if (false == m_IsResourceName[RESOURCE_MESH])
		FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom), E_FAIL);



	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_EffectDesc_Clone.vPos));


	return S_OK;
}

void CGameEffect::Check_ChangeData()
{


}

HRESULT CGameEffect::Ready_InstanceBuffer(_bool IsRenderTerm)
{
	if (nullptr == m_pPointInstanceCom)
		return S_OK;

	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer			= new VTXMATRIX_CUSTOM_ST[iInstanceCount];

	m_pInstance_Dir				= new _float3[iInstanceCount];
	m_pInstance_Pos_UpdateTime	= new _double[iInstanceCount];
	if(0.f != m_EffectDesc_Prototype.fRenderTerm)	m_pInstance_RenderTerm		= new _double[iInstanceCount];
	m_pInstance_UVCount			= new _float2[iInstanceCount];

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float2 vSize = { m_EffectDesc_Prototype.vSize.x ,m_EffectDesc_Prototype.vSize.y };

	_vector vDir = XMLoadFloat3(&m_EffectDesc_Clone.vDir);

	_bool IsRandomDir = false;
	if (0.f != XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_EffectDesc_Clone.vRandDirPower))))
		IsRandomDir = true;

	memcpy(m_pInstanceBuffer, m_pPointInstanceCom->Get_InstanceBuffer(), sizeof(VTXMATRIX_CUSTOM_ST) * iInstanceCount);

	_double fRenderTerm = 0.f;
	m_UVTime = m_EffectDesc_Clone.UVTime;

	for (_int i = 0; i < iInstanceCount; ++i)
	{
		XMStoreFloat4(&m_pInstanceBuffer[i].vPosition, vPos);
		m_pInstanceBuffer[i].vSize = vSize;

		if(nullptr != m_pInstance_RenderTerm) m_pInstance_RenderTerm[i] = fRenderTerm;

		if (true == m_EffectDesc_Prototype.IsRePosAll)
			m_pInstance_Pos_UpdateTime[i] = m_EffectDesc_Prototype.fInstancePosUpdateTerm * i;
		else
			m_pInstance_Pos_UpdateTime[i] = m_EffectDesc_Prototype.fInstancePosUpdateTerm;


		m_pInstance_UVCount[i] = { _float(rand() % m_EffectDesc_Prototype.iTextureCount_U), _float(rand() % m_EffectDesc_Prototype.iTextureCount_V) };
		m_pInstanceBuffer[i].vTextureUV = Set_UV(i);

		m_pInstance_Dir[i] = _float3(0.f, 0.f, 0.f);
		if (true == IsRandomDir)
		{
			_vector vRandDir = XMVectorZero();

			_int iRandPower[3] = { (_int)m_EffectDesc_Clone.vRandDirPower.x , (_int)m_EffectDesc_Clone.vRandDirPower.y, (_int)m_EffectDesc_Clone.vRandDirPower.z };
			_int iDir[3] = { 0, 0, 0 };

			for (_int j = 0; j < 3; ++j)
			{
				if (0 != iRandPower[j])
				{
					iDir[j] = rand() % iRandPower[j] + 1;
					if (rand() % 2 == 0)
						iDir[j] *= -1;
				}

				vRandDir.m128_f32[j] = (_float)iDir[j];
			}

			vRandDir = XMVector3Normalize(vRandDir);

			XMStoreFloat3(&m_pInstance_Dir[i], vRandDir);
		}

		if(true == IsRenderTerm)
			fRenderTerm += (_float)m_EffectDesc_Prototype.fRenderTerm;
	}

	m_pInstance_Dir[0] = m_EffectDesc_Clone.vDir;

	return S_OK;
}

void CGameEffect::Check_Color(_double TimeDelta)
{
//	if (0.f != m_EffectDesc_Clone.fColorPower)
//	{
//		_vector vColor = XMLoadFloat4(&m_EffectDesc_Clone.vColorChange);
//		_vector vColorNow = XMLoadFloat4(&m_EffectDesc_Clone.vColor);
//
//		_vector vTerm = vColor - vColorNow;
//
//		//if (false == m_EffectDesc_Prototype.Is)
//		vColorNow += vTerm * _float((TimeDelta * m_EffectDesc_Clone.fColorPower));
//		//
//		//else
//		//	vColorNow += XMVector3Normalize(vTerm) * (TimeDelta * m_EffectDesc_Prototype.fColorChangePower);
//
//		XMStoreFloat4(&m_EffectDesc_Clone.vColor, vColorNow);
//
//		m_pPointInstanceCom->Set_Variable("g_vColor", &m_EffectDesc_Clone.vColor, sizeof(_float4));
//	}
}

void CGameEffect::Check_Gravity(_double TimeDelta)
{
// 	if (false == m_EffectDesc_Prototype.IsGravity)
// 		return;
}

_float4 CGameEffect::Check_UV(_double TimeDelta, _int iIndex)
{
	_float4 vUV = m_pInstanceBuffer[iIndex].vTextureUV;

	if (-1.f == m_EffectDesc_Clone.UVTime)
		return vUV;

	m_UVTime -= TimeDelta;

	if (m_EffectDesc_Clone.UVTime <= 0.f)
	{
		m_UVTime = m_EffectDesc_Clone.UVTime;

		if (m_pInstance_UVCount[iIndex].x >= m_EffectDesc_Prototype.iTextureCount_U - 1)
		{
			m_pInstance_UVCount[iIndex].x = 0;

			m_pInstance_UVCount[iIndex].y += 1.f;
		}
		else
			m_pInstance_UVCount[iIndex].x += 1.f;

		if (m_pInstance_UVCount[iIndex].y >= m_EffectDesc_Prototype.iTextureCount_V)
			m_pInstance_UVCount[iIndex].y = 0;

		_float fLeft = (1.f / m_EffectDesc_Prototype.iTextureCount_U) *  m_pInstance_UVCount[iIndex].x;
		_float fTop = (1.f / m_EffectDesc_Prototype.iTextureCount_V) *  m_pInstance_UVCount[iIndex].y;
		_float fRight = (1.f / m_EffectDesc_Prototype.iTextureCount_U) * (m_pInstance_UVCount[iIndex].x + 1.f);
		_float fBottom = (1.f / m_EffectDesc_Prototype.iTextureCount_V) * (m_pInstance_UVCount[iIndex].y + 1.f);

		vUV = { fLeft, fTop, fRight, fBottom };
	}

	return vUV;
}

_float2 CGameEffect::Check_Size(_double TimeDelta, _int iIndex)
{
	//	if (0.f != m_EffectDesc_Prototype.fSizeChangePower)
	//	{
	//		m_pInstance_vSizeTime[iIndex] -= (_float)TimeDelta;////////////////////////////////////////////
	//
	//		// m_fSizeTime
	//		if (0.f >= m_pInstance_vSizeTime[iIndex])
	//			return m_pInstanceBuffer[iIndex].vSize;
	//
	//		_vector vSize = XMLoadFloat3(&m_EffectDesc_Prototype.vSizeChange);
	//		_vector vSizeNow = XMLoadFloat2(&m_pInstanceBuffer[iIndex].vSize);
	//
	//		if (true == m_EffectDesc_Prototype.IsResizeContinue)
	//			vSizeNow += vSize * _float(TimeDelta * m_EffectDesc_Prototype.fSizeChangePower);
	//
	//		else
	//		{
	//			_vector vTerm = vSize - vSizeNow;
	//
	//			if (0.5f >= vTerm.m128_f32[0])
	//				vTerm.m128_f32[0] = 0.f;
	//
	//			if (0.5f >= vTerm.m128_f32[1])
	//				vTerm.m128_f32[1] = 0.f;
	//
	//			vTerm.m128_f32[2] = 0.f;
	//
	//			vSizeNow += XMVector3Normalize(vTerm) * _float(TimeDelta * m_EffectDesc_Prototype.fSizeChangePower);
	//		}
	//
	//		_float2 vReSize;
	//		XMStoreFloat2(&vReSize, vSizeNow);
	//
	//		return vReSize;
	//	}
	//
	return m_pInstanceBuffer[iIndex].vSize;
}

_float4 CGameEffect::Check_Move(_double TimeDelta, _int iIndex)
{
	_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer[iIndex].vPosition);

	if (0 != iIndex  && -1.f == m_pInstance_Pos_UpdateTime[iIndex])
	{
		// 인스턴스 각자의 움직임을 부여
		return m_pInstanceBuffer[iIndex].vPosition;
	}


	m_pInstance_Pos_UpdateTime[iIndex] -= TimeDelta;

	if (0.f != XMVectorGetX(XMVector3Length(vDir)))
	{
		if (0 == iIndex)
			vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		vPos += vDir * _float(TimeDelta * m_EffectDesc_Prototype.fDirMoveSpeed);
	}

	if (0.f >= m_pInstance_Pos_UpdateTime[iIndex])
	{
		m_pInstance_Pos_UpdateTime[iIndex] = m_EffectDesc_Prototype.fInstancePosUpdateTerm;

		if (true == m_EffectDesc_Prototype.IsRePosAll)
			m_pInstance_Pos_UpdateTime[iIndex] = m_EffectDesc_Prototype.fInstancePosUpdateTerm * (m_EffectDesc_Prototype.iInstanceCount - 1);

		if (0 != iIndex)
		{
			//if (false == m_EffectDesc_Prototype.IsRePosAll)
			//	vPos = XMLoadFloat4(&m_pInstanceBuffer[iIndex - 1].vPosition);
			//else
				vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		}
	}

	if (0 == iIndex)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	_float4 vInstancePos;
	XMStoreFloat4(&vInstancePos, vPos);

	return vInstancePos;
}

void CGameEffect::Control_InstanceBuffer(_double TimeDelta)
{
	if (nullptr == m_pInstance_RenderTerm)
		return;

	for (_int i = 0; i < m_EffectDesc_Prototype.iInstanceCount; ++i)
	{
		m_pInstance_RenderTerm[i] -= TimeDelta;
		if (0.f >= m_pInstance_RenderTerm[i])
		{
			m_iInstance_RenderCount = i + 1;

			m_pInstance_RenderTerm[i] = 0.f;

		}

		m_pInstanceBuffer[i].vSize = Check_Size(TimeDelta, i);
		m_pInstanceBuffer[i].vPosition = Check_Move(TimeDelta, i);//m_pMovementCom->Get_State(EState::Position);
		m_pInstanceBuffer[i].vTextureUV = Check_UV(TimeDelta, i);
	}
}

_float4 CGameEffect::Set_UV(_int iIndex)
{
	_float fLeft	= (1.f / m_EffectDesc_Prototype.iTextureCount_U) *  m_pInstance_UVCount[iIndex].x;
	_float fTop		= (1.f / m_EffectDesc_Prototype.iTextureCount_V) *  m_pInstance_UVCount[iIndex].y;
	_float fRight	= (1.f / m_EffectDesc_Prototype.iTextureCount_U) * (m_pInstance_UVCount[iIndex].x + 1.f);
	_float fBottom	= (1.f / m_EffectDesc_Prototype.iTextureCount_V) * (m_pInstance_UVCount[iIndex].y + 1.f);

	_float4 vUV = { fLeft, fTop, fRight, fBottom };

	return vUV;
}

void CGameEffect::SetUp_Shader_Data()
{
	CPipeline* pPipeline = CPipeline::GetInstance();
	if (nullptr == pPipeline)
		return;

	if (true == m_IsResourceName[RESOURCE_TEXTURE])
	{
		m_pPointInstanceCom->Set_Variable("g_WorldMatrix",		&XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
		m_pPointInstanceCom->Set_Variable("g_MainViewMatrix",	&XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINVIEW)), sizeof(_matrix));
		m_pPointInstanceCom->Set_Variable("g_MainProjMatrix",	&XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_MAINPROJ)), sizeof(_matrix));
		m_pPointInstanceCom->Set_Variable("g_SubViewMatrix",	&XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBVIEW)), sizeof(_matrix));
		m_pPointInstanceCom->Set_Variable("g_SubProjMatrix",	&XMMatrixTranspose(pPipeline->Get_Transform(CPipeline::TS_SUBPROJ)), sizeof(_matrix));
	
		_float	fMainCamFar = pPipeline->Get_MainCamFar();
		_float	fSubCamFar = pPipeline->Get_SubCamFar();
		_vector vMainCamPosition = pPipeline->Get_MainCamPosition();
		_vector vSubCamPosition = pPipeline->Get_SubCamPosition();
	
		m_pPointInstanceCom->Set_Variable("g_fMainCamFar", &fMainCamFar, sizeof(_float));
		m_pPointInstanceCom->Set_Variable("g_fSubCamFar", &fSubCamFar, sizeof(_float));
		m_pPointInstanceCom->Set_Variable("g_vMainCamPosition", &vMainCamPosition, sizeof(_vector));
		m_pPointInstanceCom->Set_Variable("g_vSubCamPosition", &vSubCamPosition, sizeof(_vector));
	
		//	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
		//
		//if (true == m_IsResourceName[RESOURCE_TEXTURE_SECOND])
		//	m_pPointInstanceCom->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom_Second->Get_ShaderResourceView(0));
	}

	if (true == m_IsResourceName[RESOURCE_MESH])
	{

	}

	return;
}

CGameEffect * CGameEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CGameEffect*	pInstance = new CGameEffect(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CGameEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGameEffect::Clone_GameObject(void * pArg)
{
	CGameEffect* pInstance = new CGameEffect(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CGameEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGameEffect::Free()
{
	Safe_Delete_Array(m_pInstanceBuffer);
	Safe_Delete_Array(m_pInstance_Dir);
	Safe_Delete_Array(m_pInstance_Pos_UpdateTime);
	Safe_Delete_Array(m_pInstance_RenderTerm);
	Safe_Delete_Array(m_pInstance_UVCount);


	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pPointInstanceCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pTexturesCom);
	Safe_Release(m_pTexturesCom_Second);

	__super::Free();
}

