#include "stdafx.h"
#include "..\Public\Effect_Player_Revive.h"

CEffect_Player_Revive::CEffect_Player_Revive(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Player_Revive::CEffect_Player_Revive(const CEffect_Player_Revive & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Player_Revive::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);


	return S_OK;
}

HRESULT CEffect_Player_Revive::NativeConstruct(void * pArg)
{
	m_EffectDesc_Prototype.fLifeTime = 2.f;
	m_EffectDesc_Prototype.vSize = { 0.0625f, 0.0625f,0.f };

	__super::Ready_Component(pArg);

	m_pModelCom = static_cast<CModel*>(m_EffectDesc_Clone.pArg);
	Safe_AddRef(m_pModelCom);

	//if (EFFECT_DESC_CLONE::PV_CODY >= m_EffectDesc_Clone.iPlayerValue)
	//	m_EffectDesc_Prototype.iInstanceCount = 1500;
	//else if (EFFECT_DESC_CLONE::PV_CODY_S == m_EffectDesc_Clone.iPlayerValue)
	//	m_EffectDesc_Prototype.iInstanceCount = 240;
	//else if (EFFECT_DESC_CLONE::PV_CODY_L == m_EffectDesc_Clone.iPlayerValue)
	//	m_EffectDesc_Prototype.iInstanceCount = 4200;

	m_EffectDesc_Clone.UVTime = 0.01;
	m_EffectDesc_Clone.vRandDirPower = { 10.f,10.f,10.f };
	m_EffectDesc_Prototype.iInstanceCount = m_pModelCom->Get_VertexCount();
	m_fFarRatio = m_fFarRatio_Max / _float(m_EffectDesc_Prototype.iInstanceCount * 0.1f);

	Ready_Instance();

	return S_OK;
}

_int CEffect_Player_Revive::Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	m_EffectDesc_Prototype.fLifeTime -= (_float)TimeDelta;
	m_dTime -= TimeDelta * 2.f;

	//for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	//{
	//	Instance_Pos((_float)TimeDelta, iIndex);
	//	Instance_Size((_float)TimeDelta, iIndex);
	//}

	m_fTeseTime += (_float)TimeDelta;

	return _int();
}

_int CEffect_Player_Revive::Late_Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Player_Revive::Render(RENDER_GROUP::Enum eGroup)
{
	SetUp_Shader_Data();

	//_float4 vUV = { 0.f,0.f,1.f,1.f };
	//m_pPointInstanceCom->Set_Variable("g_vColorRamp_UV", &vUV, sizeof(_float4));
	//m_pPointInstanceCom->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	//m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom_Second->Get_ShaderResourceView(0));
	//
	//for (_int i = 0; i < 256; ++i)
	//{
	//
	//}
	//
	//m_pPointInstanceCom->Render(4, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);
	_float4 vTextureUV_LTRB = { 0.f,0.f,1.f,1.f };
	m_pModelCom->Set_Variable("g_fTime", &m_fTeseTime, sizeof(_float));

	m_pModelCom->Set_Variable("g_vTextureUV_LTRB", &_float4(0.f, 0.f, 1.f, 1.f), sizeof(_float4));
	
	//m_pModelCom->Set_ShaderResourceView("g_MaskingTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model_VERTEX(10, m_pInstanceBuffer);

	return S_OK;
}

void CEffect_Player_Revive::Instance_Size(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer[iIndex].vSize.x -= TimeDelta * 0.04f;
	m_pInstanceBuffer[iIndex].vSize.y -= TimeDelta * 0.04f;

	if (0.f >= m_pInstanceBuffer[iIndex].vSize.x)
	{
		m_pInstanceBuffer[iIndex].vSize.x = 0.f;
		m_pInstanceBuffer[iIndex].vSize.y = 0.f;
	}
}

void CEffect_Player_Revive::Instance_Pos(_float TimeDelta, _int iIndex)
{
	//_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer[iIndex].vPosition);
	_vector vTargetPos = XMLoadFloat4(&m_pInstance_TargetPos[iIndex]);	 

	_vector vTarget = vTargetPos - vPos;
	if (0.1f <= (XMVector3Length(vTarget)).m128_f32[0])
	{
		//vPos += vDir * TimeDelta * (_float)m_dTime;
		vPos += (vTarget) * TimeDelta * 5.f;
	}

	XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vPos);
}

void CEffect_Player_Revive::Instance_UV(_float TimeDelta, _int iIndex)
{

}

HRESULT CEffect_Player_Revive::Ready_Instance()
{
	if (nullptr == m_pPointInstanceCom)
		return S_OK;

	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pInstanceBuffer		= new VTXMATRIX_CUSTOM_ST[iInstanceCount];
	//m_pInstance_Dir			= new _float3[iInstanceCount];
	m_pInstance_UVCount		= new _float2[iInstanceCount];
	m_pInstance_TargetPos	= new _float4[iInstanceCount];

	VTXMESH* pVtx = m_pModelCom->Get_Vertices();
	_uint iVtxCount = m_pModelCom->Get_VertexCount();
	_uint iRandVtx	= rand() % iInstanceCount;
	_uint iAddVtx	= _int(iVtxCount / (_float)iInstanceCount);

	_float	fAdditinalDir = m_fFarRatio;
	_float2 vSize = { m_EffectDesc_Prototype.vSize.x ,m_EffectDesc_Prototype.vSize.y };
	_vector vDir = XMLoadFloat3(&m_EffectDesc_Clone.vDir);

	_float fIndecesRatio = 0.6f * iInstanceCount;
	_float fIndecesRatio2 = 0.4f * iInstanceCount;

	_float fScaleX = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
	_float fDis = 0.5f * fScaleX;

	_double fRenderTerm = 0.f;
	m_UVTime = m_EffectDesc_Clone.UVTime;

	_vector vPos = XMVectorZero();
	_matrix	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(-90.f)) * XMMatrixRotationZ(XMConvertToRadians(90.f));

	//memcpy(m_pInstanceBuffer, m_pPointInstanceCom->Get_InstanceBuffer(), sizeof(VTXMATRIX_CUSTOM_ST) * iInstanceCount);

	for (_int i = 0; i < iInstanceCount; ++i)
	{
		m_pInstanceBuffer[i].vPosition = { 0.f, 0.f, 0.f, 1.f };
		_int iRandSize = rand() % 3;
		if (0 == iRandSize)
			m_pInstanceBuffer[i].vSize = _float2(0.05f, 0.05f);
		else if (0 == iRandSize)
			m_pInstanceBuffer[i].vSize = _float2(0.0625f, 0.0625f);
		else
			m_pInstanceBuffer[i].vSize = _float2(0.075f, 0.075f);

		_vector vRandDir = XMVectorZero();

		_int iRandPower[3] = { (_int)m_EffectDesc_Clone.vRandDirPower.x , (_int)m_EffectDesc_Clone.vRandDirPower.y, (_int)m_EffectDesc_Clone.vRandDirPower.z };
		_int iDir[3] = { 0, 0, 0 };

		for (_int j = 0; j < 3; ++j)
		{
			if (0 != iRandPower[j])
			{
				iDir[j] = rand() % (iRandPower[j] + 1);
				if (true == m_EffectDesc_Clone.IsRandDirDown[j] && rand() % 2 == 0)
					iDir[j] *= -1;
			}

			vRandDir.m128_f32[j] = (_float)iDir[j];
		}

		vRandDir = XMVector3Normalize(vRandDir);		

		_vector vLocalPos = XMLoadFloat3(&pVtx[iRandVtx].vPosition);
		vLocalPos = XMVector3Transform(vLocalPos, PivotMatrix);
		vLocalPos = XMVector3Transform(vLocalPos, XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix));
		
		vPos = vLocalPos + vRandDir * fAdditinalDir;
		//_vector vWorldDir = XMVector3Normalize((vLocalPos - vPos));
		//_vector vDir = XMVector3Cross(vWorldDir, XMVectorSet(0.f, 1.f, 0.f, 0.f));

		XMStoreFloat4(&m_pInstanceBuffer[i].vPosition, vPos);
		XMStoreFloat4(&m_pInstance_TargetPos[i], vLocalPos);
	//	XMStoreFloat3(&m_pInstance_Dir[i], vDir);

		iRandVtx += iAddVtx;
		if (iRandVtx >= iVtxCount)
			iRandVtx = rand() % iInstanceCount;

		Set_VtxColor(i, iRandVtx);

		if(i > fIndecesRatio)
			fAdditinalDir += fDis;
		else if (i > fIndecesRatio2)
			fAdditinalDir += fDis * 0.5f;

		if (m_fFarRatio_Max <= fAdditinalDir)
			fAdditinalDir = 0.f;
	}

	return S_OK;
}

_float4 CEffect_Player_Revive::Set_particleUV(_int iIndex, _int U, _int V)
{
	_float fLeft	= (1.f / U) *  m_pInstance_UVCount[iIndex].x;
	_float fTop		= (1.f / V) *  m_pInstance_UVCount[iIndex].y;
	_float fRight	= (1.f / U) * (m_pInstance_UVCount[iIndex].x + 1.f);
	_float fBottom	= (1.f / V) * (m_pInstance_UVCount[iIndex].y + 1.f);

	return _float4(fLeft, fTop, fRight, fBottom);
}

void CEffect_Player_Revive::Set_VtxColor(_int iIndex, _uint iVtxIndex)
{
	if (EFFECT_DESC_CLONE::PV_MAY == m_EffectDesc_Clone.iPlayerValue)
	{
		if (0 <= iVtxIndex && iVtxIndex <= 92670) // ¹åÁÙ, ÆÈ¶Ò
			m_pInstance_UVCount[iIndex] = { 3.f, 0.f };
		else if (92670 <= iVtxIndex && iVtxIndex <= 135426) // ½º¿þÅÍ
			m_pInstance_UVCount[iIndex] = { 1.f, 0.f };
		else if (135426 <= iVtxIndex && iVtxIndex <= 140226) // ´«±ò
			m_pInstance_UVCount[iIndex] = { 3.f, 0.f };
		else if (140226 <= iVtxIndex && iVtxIndex <= 177645) // ´ë°¥Åë
			m_pInstance_UVCount[iIndex] = { 3.f, 0.f };
		else if (177645 <= iVtxIndex && iVtxIndex <= 291639) // ¸Ó¸®ÅÐ, Àå½Ä
			m_pInstance_UVCount[iIndex] = { 0.f, 0.f };
		else if (291639 <= iVtxIndex && iVtxIndex <= 347628) // ¹ÙÁö, ½Å¹ß
			m_pInstance_UVCount[iIndex] = { 2.f, 0.f };
		else
			m_pInstance_UVCount[iIndex] = { 1.f, 0.f };
	}
	else
	{
		if (0 <= iVtxIndex && iVtxIndex <= 41748) // ½º¿þÅÍ
			m_pInstance_UVCount[iIndex] = { 3.f, 2.f };
		else if (54924 <= iVtxIndex && iVtxIndex <= 96558) // ¸Ó¸®ÅÐ
			m_pInstance_UVCount[iIndex] = { 0.f, 2.f };
		else if (177171 <= iVtxIndex && iVtxIndex <= 209500) // ¹ÙÁö
			m_pInstance_UVCount[iIndex] = { 2.f, 2.f };
		else if (209500 <= iVtxIndex && iVtxIndex <= 259779) // ÆÈ¶Ò
			m_pInstance_UVCount[iIndex] = { 1.f, 2.f };
		else if (259779 <= iVtxIndex && iVtxIndex <= 301083) // ¶Ç´Ù¸¥ ¸Ó¸®ÅÐ
			m_pInstance_UVCount[iIndex] = { 0.f, 2.f };
		else
			m_pInstance_UVCount[iIndex] = { 1.f, 2.f };
	}

	m_pInstanceBuffer[iIndex].vTextureUV = Set_particleUV(iIndex, 4, 4);
}

CEffect_Player_Revive * CEffect_Player_Revive::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Player_Revive*	pInstance = new CEffect_Player_Revive(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Player_Revive");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Player_Revive::Clone_GameObject(void * pArg)
{
	CEffect_Player_Revive* pInstance = new CEffect_Player_Revive(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Player_Revive");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Player_Revive::Free()
{
	Safe_Delete_Array(m_pInstance_TargetPos);

	__super::Free();
}

