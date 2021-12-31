#include "stdafx.h"
#include "..\Public\Effect_Dead_Particle_Fire.h"

CEffect_Player_Dead_Particle_Fire::CEffect_Player_Dead_Particle_Fire(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Player_Dead_Particle_Fire::CEffect_Player_Dead_Particle_Fire(const CEffect_Player_Dead_Particle_Fire & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Player_Dead_Particle_Fire::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);


	return S_OK;
}

HRESULT CEffect_Player_Dead_Particle_Fire::NativeConstruct(void * pArg)
{
	m_EffectDesc_Prototype.fLifeTime = 4.f;
	m_EffectDesc_Prototype.vSize = { 0.0625f, 0.0625f,0.f };

	__super::Ready_Component(pArg);

	_float fScale = m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);
	m_EffectDesc_Prototype.vSize.x *= fScale;
	m_EffectDesc_Prototype.vSize.y *= fScale;
	m_vSize.x *= fScale;
	m_vSize.y *= fScale;

	if (EFFECT_DESC_CLONE::PV_CODY >= m_EffectDesc_Clone.iPlayerValue)
		m_EffectDesc_Prototype.iInstanceCount = 2000; // 500 100
	else if (EFFECT_DESC_CLONE::PV_CODY_S == m_EffectDesc_Clone.iPlayerValue)
		m_EffectDesc_Prototype.iInstanceCount = 120; // 100 20
	else if (EFFECT_DESC_CLONE::PV_CODY_L == m_EffectDesc_Clone.iPlayerValue)
		m_EffectDesc_Prototype.iInstanceCount = 8000; // 2500 500

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Circle_Alpha"), TEXT("Com_Texture_Particle_Mask"), (CComponent**)&m_pTexturesCom_Particle_Mask), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Instance(),	 E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Point(),		 E_FAIL);
	FAILED_CHECK_RETURN(Ready_Point_Small(), E_FAIL);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION)
		+ (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 0.5f)
		+ (XMVector3Normalize((m_pTransformCom->Get_State(CTransform::STATE_UP))) * 1.2f);

	XMStoreFloat3(&m_vTargetPos, vPos);

	m_vSize = _float2(0.0225f, 0.0225f);
	return S_OK;
}

_int CEffect_Player_Dead_Particle_Fire::Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	m_EffectDesc_Prototype.fLifeTime -= (_float)TimeDelta;
	m_fMoveTime += (_float)TimeDelta * 0.25f;

	m_vSize.x -= (_float)TimeDelta * 0.05f;
	if (0.f >= m_vSize.x)
		m_vSize.x = 0.f;
	m_vSize.y = m_vSize.x;

	for (_int iIndex = 0; iIndex < m_EffectDesc_Prototype.iInstanceCount; ++iIndex)
	{
		Instance_Pos((_float)TimeDelta, iIndex);
		Instance_Size((_float)TimeDelta, iIndex);
	}

	//Update_Point(TimeDelta);
	Update_Point_Small(TimeDelta);

	return _int();
}

_int CEffect_Player_Dead_Particle_Fire::Late_Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Player_Dead_Particle_Fire::Render(RENDER_GROUP::Enum eGroup)
{
	SetUp_Shader_Data();

	_float4 vUV = { 0.f,0.f,1.f,1.f };//g_SecondTexture
	m_pPointInstanceCom->Set_Variable("g_vColorRamp_UV", &vUV, sizeof(_float4));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom_Second->Get_ShaderResourceView(0));
	m_pPointInstanceCom->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom->Render(4, m_pInstanceBuffer, m_EffectDesc_Prototype.iInstanceCount);

	//
// 	m_pPointInstanceCom->Set_Variable("g_fTime", &m_fSmokeAlphaTime, sizeof(_float));
// 	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom_Point_Smoke->Get_ShaderResourceView(0));
// 	m_pPointInstanceCom->Render(7, m_pPointBuffer_Smoke, 1);

	m_pPointInstanceCom->Set_Variable("g_fTime", &m_fPointInstance_Small_Alpha, sizeof(_float));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom_Point_Diff->Get_ShaderResourceView(0));
	m_pPointInstanceCom->Set_ShaderResourceView("g_SecondTexture", m_pTexturesCom_Point_Sprite->Get_ShaderResourceView(0));
	m_pPointInstanceCom->Render(6, m_pPointBuffer_Smoke_Small, m_iPointInstanceCount_Small);

	_float4 vTextureUV_LTRB = { 0.f,0.f,1.f,1.f };
	_float4 vColorRamp = { 0.7843f, 0.f, 0.f, 1.f };
	m_pModelCom->Set_Variable("g_vPos", &m_vTargetPos, sizeof(_float4));
	m_pModelCom->Set_Variable("g_vColor", &vColorRamp, sizeof(_float4));
	m_pModelCom->Set_Variable("g_fTime", &m_fMoveTime, sizeof(_float));
	m_pModelCom->Set_Variable("g_vParticleSize", &m_vSize, sizeof(_float2));
	m_pModelCom->Set_Variable("g_vTextureUV_LTRB", &_float4(0.f, 0.f, 1.f, 1.f), sizeof(_float4));

	m_pModelCom->Set_ShaderResourceView("g_MaskingTexture", m_pTexturesCom_Particle_Mask->Get_ShaderResourceView(0));
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model_VERTEX(12);

	return S_OK;
}

void CEffect_Player_Dead_Particle_Fire::Instance_Size(_float TimeDelta, _int iIndex)
{
	m_pInstanceBuffer[iIndex].vSize.x -= TimeDelta * 0.035f;
	m_pInstanceBuffer[iIndex].vSize.y -= TimeDelta * 0.035f;

	if (0.f >= m_pInstanceBuffer[iIndex].vSize.x)
	{
		m_pInstanceBuffer[iIndex].vSize.x = 0.f;
		m_pInstanceBuffer[iIndex].vSize.y = 0.f;
	}
}

void CEffect_Player_Dead_Particle_Fire::Instance_Pos(_float TimeDelta, _int iIndex)
{
	_vector vDir = XMLoadFloat3(&m_pInstance_Dir[iIndex]);
	_vector vPos = XMLoadFloat4(&m_pInstanceBuffer[iIndex].vPosition);

	_float fSpeedPerSec = 0.5f;
	if (iIndex >= m_EffectDesc_Prototype.iInstanceCount - (m_EffectDesc_Prototype.iInstanceCount / 10))
		fSpeedPerSec = 1.2f;

	vPos += vDir * TimeDelta * fSpeedPerSec;

	XMStoreFloat4(&m_pInstanceBuffer[iIndex].vPosition, vPos);
}

void CEffect_Player_Dead_Particle_Fire::Instance_UV(_float TimeDelta, _int iIndex)
{

}

HRESULT CEffect_Player_Dead_Particle_Fire::Ready_Instance()
{
	if (nullptr == m_pPointInstanceCom)
		return S_OK;

	_int iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	m_pModelCom = static_cast<CModel*>(m_EffectDesc_Clone.pArg);
	Safe_AddRef(m_pModelCom);
	VTXMESH* pVtx	= m_pModelCom->Get_Vertices();
	_uint iVtxCount = m_pModelCom->Get_VertexCount();
	_uint iRandVtx	= rand() % iInstanceCount;
	_uint iAddVtx	= _int(iVtxCount / (_float)iInstanceCount);

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_matrix	PivotMatrix = XMMatrixScaling(0.011f, 0.011f, 0.011f);
	PivotMatrix *= XMMatrixRotationY(XMConvertToRadians(-90.f)) * XMMatrixRotationZ(XMConvertToRadians(90.f)) * WorldMatrix;

	_matrix NormalizeScale_Matrix = WorldMatrix;
	m_pInstanceBuffer	= new VTXMATRIX_CUSTOM_ST[iInstanceCount];
	m_pInstance_Dir		= new _float3[iInstanceCount];

	for (_int i = 0; i < iInstanceCount; ++i)
	{
		m_pInstanceBuffer[i].vSize	= Get_RandSize();

			m_pInstance_Dir[i] = Get_Dir(_int3(50, 100, 100), NormalizeScale_Matrix);
		//if (i >= m_EffectDesc_Prototype.iInstanceCount - (m_EffectDesc_Prototype.iInstanceCount / 10))
		//else
		//	m_pInstance_Dir[i] = Get_Dir_Defulat(_int3(30, 100, 30), NormalizeScale_Matrix);

		
		_vector vLocalPos = XMLoadFloat3(&pVtx[iRandVtx].vPosition);
		vLocalPos = XMVector3Transform(vLocalPos, PivotMatrix);
		XMStoreFloat4(&m_pInstanceBuffer[i].vPosition, vLocalPos);

		iRandVtx += iAddVtx;
		if (iRandVtx >= iVtxCount)
			iRandVtx = rand() % iInstanceCount;

		m_pInstanceBuffer[i].vTextureUV = { 0.f, 0.25f, 0.25f, 0.50f };
	}

	return S_OK;
}

// HRESULT CEffect_Player_Dead_Particle_Fire::Ready_Point()
// {
// 	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Explosion7x7"), TEXT("Com_Texture_Smoke"), (CComponent**)&m_pTexturesCom_Point_Smoke), E_FAIL);
// 
// 	_float fSizeX = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
// 	_float fSizeY = m_pTransformCom->Get_Scale(CTransform::STATE_UP);
// 
// 	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
// 	_float4 vWorldPos;
// 	XMStoreFloat4(&vWorldPos, WorldMatrix.r[3]);
// 	vWorldPos.y += fSizeY;
// 
// 	m_pPointBuffer_Smoke				= new VTXMATRIX_CUSTOM_ST[1];
// 	m_pPointBuffer_Smoke[0].vRight		= { 1.f, 0.f, 0.f, 0.f };
// 	m_pPointBuffer_Smoke[0].vUp			= { 0.f, 1.f, 0.f, 0.f };
// 	m_pPointBuffer_Smoke[0].vLook		= { 0.f, 0.f, 1.f, 0.f };
// 	m_pPointBuffer_Smoke[0].vPosition	= vWorldPos;
// 	m_pPointBuffer_Smoke[0].vSize		= { fSizeX, fSizeY };
// 	m_pPointBuffer_Smoke[0].vTextureUV	= Get_TexUV(7, 7, true);
// 
// 	return S_OK;
// }

HRESULT CEffect_Player_Dead_Particle_Fire::Ready_Point_Small()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_T_Fire_Tiled"), TEXT("Com_Texture_Smoke_Diff"), (CComponent**)&m_pTexturesCom_Point_Diff), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_SoftCLoud"), TEXT("Com_Texture_Smoke_Sprite"), (CComponent**)&m_pTexturesCom_Point_Sprite), E_FAIL);

	_float fSizeX = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
	_float fSizeY = m_pTransformCom->Get_Scale(CTransform::STATE_UP);

	m_pPointBuffer_Smoke_Small	= new VTXMATRIX_CUSTOM_ST[m_iPointInstanceCount_Small];
	m_vPointBuffer_Small_Dir	= new _float3[m_iPointInstanceCount_Small];

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float4 vWorldPos;
	XMStoreFloat4(&vWorldPos, WorldMatrix.r[3]);
	vWorldPos.y += fSizeY;

	for (_int iIndex = 0; iIndex < m_iPointInstanceCount_Small; ++iIndex)
	{
		m_pPointBuffer_Smoke_Small[iIndex].vRight		= { 1.f, 0.f, 0.f, 0.f };
		m_pPointBuffer_Smoke_Small[iIndex].vUp			= { 0.f, 1.f, 0.f, 0.f };
		m_pPointBuffer_Smoke_Small[iIndex].vLook		= { 0.f, 0.f, 1.f, 0.f };
		m_pPointBuffer_Smoke_Small[iIndex].vPosition	= vWorldPos;
		m_pPointBuffer_Smoke_Small[iIndex].vSize		= { fSizeX, fSizeY };
		m_pPointBuffer_Smoke_Small[iIndex].vTextureUV	= Get_TexUV_Rand(2, 2);
		m_vPointBuffer_Small_Dir[iIndex]				= Get_Dir_Random(_int3(10, 10, 10), WorldMatrix);
	}

	return S_OK;
}

_float4 CEffect_Player_Dead_Particle_Fire::Set_particleUV(_int iIndex, _int U, _int V)
{
	_float fLeft	= (1.f / U) *  m_pInstance_UVCount[iIndex].x;
	_float fTop		= (1.f / V) *  m_pInstance_UVCount[iIndex].y;
	_float fRight	= (1.f / U) * (m_pInstance_UVCount[iIndex].x + 1.f);
	_float fBottom	= (1.f / V) * (m_pInstance_UVCount[iIndex].y + 1.f);

	_float4 vUV = { fLeft, fTop, fRight, fBottom };

	return vUV;
}

_float2 CEffect_Player_Dead_Particle_Fire::Get_RandSize()
{
	_int iRandSize = rand() % 3;
	if (0 == iRandSize)
		return _float2(0.05f, 0.05f);
	else if (0 == iRandSize)
		return _float2(0.0625f, 0.0625f);
	else
		return _float2(0.075f, 0.075f);
}

_float3 CEffect_Player_Dead_Particle_Fire::Get_Dir_Defulat(_int3 vDirPower, _fmatrix WorldMatrix)
{
	_vector vRandDir = XMVectorZero();
	_vector vDir[3];
	_int	iRandPower[3] = { vDirPower.x , vDirPower.y, vDirPower.z };

	for (_int j = 0; j < 3; ++j)
	{
		vDir[j] = WorldMatrix.r[j];

		if (0 != iRandPower[j])
			vDir[j] *= _float(rand() % (iRandPower[j] + 1));

		if (rand() % 2 == 0 && 1 != j)
			vDir[j] *= -1.f;

		if(1 == j)
			vDir[j] *= -10.f;

		vRandDir += vDir[j];
	}

	_float3 vDirfloat3;
	XMStoreFloat3(&vDirfloat3, XMVector3Normalize(vRandDir));
	return vDirfloat3;
}

_float3 CEffect_Player_Dead_Particle_Fire::Get_Dir(_int3 vDirPower, _fmatrix WorldMatrix)
{
	_vector vRandDir = XMVectorZero();
	_vector vDir[3];
	_int	iRandPower[3] = { vDirPower.x , vDirPower.y, vDirPower.z };

	for (_int j = 0; j < 3; ++j)
	{
		vDir[j] = WorldMatrix.r[j];

		if (0 != iRandPower[j])
			vDir[j] *= _float(rand() % (iRandPower[j] + 1));

		if (rand() % 2 == 0 && 0 == j)
			vDir[j] *= -1.f;

		if (2 == j)
			vDir[j] *= -1.f;

		vRandDir += vDir[j];
	}

	_float3 vDirfloat3;
	XMStoreFloat3(&vDirfloat3, XMVector3Normalize(vRandDir));
	return vDirfloat3;
}

_float3 CEffect_Player_Dead_Particle_Fire::Get_Dir_Random(_int3 vDirPower, _fmatrix WorldMatrix)
{
	_vector vRandDir = XMVectorZero();
	_vector vDir[3];
	_int	iRandPower[3] = { vDirPower.x , vDirPower.y, vDirPower.z };

	for (_int j = 0; j < 3; ++j)
	{
		vDir[j] = WorldMatrix.r[j];

		if (0 != iRandPower[j])
			vDir[j] *= _float(rand() % (iRandPower[j] + 1));

		if (rand() % 2 == 0)
			vDir[j] *= -1.f;

		vRandDir += vDir[j];
	}

	_float3 vDirfloat3;
	XMStoreFloat3(&vDirfloat3, XMVector3Normalize(vRandDir));
	return vDirfloat3;
}

//void CEffect_Player_Dead_Particle_Fire::Update_Point(_double TimeDelta)
//{
// 	m_pPointBuffer_Smoke[0].vSize.x += (_float)TimeDelta * 0.25f;
// 	m_pPointBuffer_Smoke[0].vSize.y += (_float)TimeDelta * 0.25f;
// 
// 	m_fSmokeAlphaTime -= (_float)TimeDelta * 0.333f;
// 
// 	m_fUVCheckTime -= (_float)TimeDelta;
// 	if (0 <= m_fUVCheckTime)
// 		return;
// 
// 	m_fUVCheckTime = 0.02f;
// 	m_pPointBuffer_Smoke[0].vTextureUV = Check_UV(7, 7, &m_iPointInstance_Texture_U, &m_iPointInstance_Texture_V, true);
//}

void CEffect_Player_Dead_Particle_Fire::Update_Point_Small(_double TimeDelta)
{
	if (true == m_IsPointInstance_Small_Alpha_Add)
	{
		m_fPointInstance_Small_Alpha += (_float)TimeDelta * 1.75f;

		if (1.f <= m_fPointInstance_Small_Alpha)
		{
			m_fPointInstance_Small_Alpha = 1.f;
			m_IsPointInstance_Small_Alpha_Add = false;
		}
	}
	else
	{
		m_fPointInstance_Small_Alpha -= (_float)TimeDelta;
		if (0.0 > m_fPointInstance_Small_Alpha)
			m_fPointInstance_Small_Alpha = 0.0;
	}

	for (_int iIndex = 0; iIndex < m_iPointInstanceCount_Small; ++iIndex)
	{
		_vector vDir = XMLoadFloat3(&m_vPointBuffer_Small_Dir[iIndex]);
		_vector vPos = XMLoadFloat4(&m_pPointBuffer_Smoke_Small[iIndex].vPosition);
		vPos += vDir * (_float)TimeDelta * 1.25f;
		XMStoreFloat4(&m_pPointBuffer_Smoke_Small[iIndex].vPosition, vPos);

		m_pPointBuffer_Smoke_Small[iIndex].vSize.x += (_float)TimeDelta * 0.125f;
		m_pPointBuffer_Smoke_Small[iIndex].vSize.y += (_float)TimeDelta * 0.125f;
	}
}

CEffect_Player_Dead_Particle_Fire * CEffect_Player_Dead_Particle_Fire::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Player_Dead_Particle_Fire*	pInstance = new CEffect_Player_Dead_Particle_Fire(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Player_Dead_Particle_Fire");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Player_Dead_Particle_Fire::Clone_GameObject(void * pArg)
{
	CEffect_Player_Dead_Particle_Fire* pInstance = new CEffect_Player_Dead_Particle_Fire(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Player_Dead_Particle_Fire");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Player_Dead_Particle_Fire::Free()
{
	Safe_Release(m_pTexturesCom_Point_Diff);
	Safe_Release(m_pTexturesCom_Point_Sprite);


	Safe_Delete_Array(m_pInstance_LocalPos);
	Safe_Delete_Array(m_pPointBuffer_Smoke_Small);
	Safe_Delete_Array(m_vPointBuffer_Small_Dir);

 	Safe_Release(m_pTexturesCom_Particle_Mask);

	__super::Free();
}

