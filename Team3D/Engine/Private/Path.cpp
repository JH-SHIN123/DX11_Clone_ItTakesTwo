#include "Path.h"
#include "Model_Loader.h"
#include "Anim.h"

CPath::CPath(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_pModel_Loader(CModel_Loader::GetInstance())
{
	Safe_AddRef(m_pModel_Loader);
}

CPath::CPath(const CPath& rhs)
	: CComponent(rhs),
	m_pPathAnim(rhs.m_pPathAnim),
	m_AnimTransformations(rhs.m_AnimTransformations)
{
	lstrcpy(m_szPathTag, rhs.m_szPathTag);

	Safe_AddRef(m_pPathAnim);
}

HRESULT CPath::NativeConstruct_Prototype(const _tchar* pFilePath, const _tchar* pPathTag)
{
	NULL_CHECK_RETURN(m_pModel_Loader, E_FAIL);
	NULL_CHECK_RETURN(pFilePath, E_FAIL);
	NULL_CHECK_RETURN(pPathTag, E_FAIL);

	CComponent::NativeConstruct_Prototype();

	lstrcpy(m_szPathTag, pPathTag);

	FAILED_CHECK_RETURN(m_pModel_Loader->Load_PathFromFile((void**)(&m_pPathAnim), pFilePath), E_FAIL);

	m_AnimTransformations.resize(m_pPathAnim->Get_ChannelCount());
	for (auto& pAnimTransfomation : m_AnimTransformations)
		pAnimTransfomation = MH_XMFloat4x4Identity();

	return S_OK;
}

HRESULT CPath::NativeConstruct(void* pArg)
{
	CComponent::NativeConstruct(pArg);

	return S_OK;
}

HRESULT CPath::Update_Animation(_double dTimeDelta, _matrix& WorldMatrix)
{
	NULL_CHECK_RETURN(m_pPathAnim, E_FAIL);

	_double dDuration = m_pPathAnim->Get_Duration();
	_double dTicksPerSecond = m_pPathAnim->Get_TicksPerSecond();

	m_dCurrentTime += fmod(dTicksPerSecond * dTimeDelta, dDuration);

	if (m_dCurrentTime >= dDuration) {
		m_dCurrentTime = 0.f;
		m_iCurAnimFrame = 0;;
	}

	/* Update_AnimTransformations */
	m_pPathAnim->Update_PathTransformation(m_dCurrentTime, m_iCurAnimFrame, m_AnimTransformations);
	/* Update_CombinedTransformations */
	_matrix WorldMatrixTemp;
	WorldMatrixTemp = XMLoadFloat4x4(&m_AnimTransformations[2]);
	//WorldMatrix *= XMLoadFloat4x4(&m_AnimTransformations[1]);

	_float3 fRotateAngle = Get_RotationAngles(m_AnimTransformations[1]);
	_matrix RotateMatrix = XMMatrixRotationX((fRotateAngle.x));
	RotateMatrix *= XMMatrixRotationY((-fRotateAngle.z));
	RotateMatrix *= XMMatrixRotationY(XMConvertToRadians(90.f));
	RotateMatrix *= XMMatrixRotationZ((fRotateAngle.y));

	WorldMatrixTemp *= RotateMatrix;
	WorldMatrixTemp *= XMLoadFloat4x4(&m_AnimTransformations[0]);
	WorldMatrix = WorldMatrixTemp;
	//for (auto& pAnimTransforamtion : m_AnimTransformations)
	//{
	//	WorldMatrix *= XMLoadFloat4x4(&pAnimTransforamtion);
	//}

	/* Update Progress */
	m_fProgressAnim = _float(m_dCurrentTime / dDuration);

	return S_OK;
}

_float3 CPath::Get_RotationAngles(const _float4x4& fRotateMatrix)
{
	/*
	Rotate Matrix
	| r11 r12 r13  0 |
	| r21 r22 r23  0 |
	| r31 r32 r33  0 |
	|  0   0   0   1 |
	*/

	return {
		atan2(fRotateMatrix._23, fRotateMatrix._33),
		atan2(-fRotateMatrix._13, sqrt(pow(fRotateMatrix._23, 2) + pow(fRotateMatrix._33, 2))),
		atan2(fRotateMatrix._12, fRotateMatrix._11)
	};
}

CPath* CPath::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pFilePath, const _tchar* pPathTag)
{
	CPath* pInstance = new CPath(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pFilePath, pPathTag)))
	{
		MSG_BOX("Failed to Create Instance - CPath");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CPath::Clone_Component(void* pArg)
{
	CPath* pInstance = new CPath(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPath");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPath::Free()
{
	Safe_Release(m_pPathAnim);

	if(false == m_isClone) Safe_Release(m_pModel_Loader);

	CComponent::Free();
}
