#include "Path.h"
#include "Model_Loader.h"
#include "Anim.h"

CPath::CPath(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CPath::CPath(const CPath& rhs)
	: CComponent(rhs),
	m_pPathAnim(rhs.m_pPathAnim),
	m_CurAnimTransformations(rhs.m_CurAnimTransformations),
	m_NextAnimTransformations(rhs.m_NextAnimTransformations),
	m_dDurationTime(rhs.m_dDurationTime),
	m_PivotMatrix(rhs.m_PivotMatrix)
{
	lstrcpy(m_szPathTag, rhs.m_szPathTag);
	Safe_AddRef(m_pPathAnim);
}

void CPath::Get_FramesWorldMatrices(vector<_uint>& OutFrameIndices, vector<_float4x4>& OutMatrices, _uint iPerNodeInteract)
{
	vector<_uint> FrameIndices;
	vector<_float4x4> FrameWorldMatrices;
	FrameIndices.reserve((size_t)(m_dDurationTime / iPerNodeInteract));;
	FrameWorldMatrices.reserve((size_t)(m_dDurationTime / iPerNodeInteract));

	_float4x4 NodeWorldMatrix = MH_XMFloat4x4Identity();
	_uint iCurAnimFrame = 0;
	_double dCurrentTime = 0;
	
	TRANSFORMATIONS tTransformation;
	tTransformation.resize(m_pPathAnim->Get_ChannelCount());

	for (; dCurrentTime < m_dDurationTime; dCurrentTime += iPerNodeInteract)
	{
		/* Update_AnimTransformations */
		m_pPathAnim->Update_PathTransformation(dCurrentTime, iCurAnimFrame, tTransformation, iPerNodeInteract);

		/* Update_CombinedTransformations */
		XMStoreFloat4x4(&NodeWorldMatrix, XMLoadFloat4x4(&tTransformation[CHANNEL_TRANSLATION]) * XMLoadFloat4x4(&m_PivotMatrix) * XMLoadFloat4x4(&m_tDesc.WorldMatrix));
		
		FrameIndices.emplace_back(iCurAnimFrame);
		FrameWorldMatrices.emplace_back(NodeWorldMatrix);
	}

	OutFrameIndices.swap(FrameIndices);
	OutMatrices.swap(FrameWorldMatrices);
}

HRESULT CPath::NativeConstruct_Prototype(const _tchar* pFilePath, const _tchar* pPathTag, _fmatrix PivotMatrix)
{
	NULL_CHECK_RETURN(pFilePath, E_FAIL);
	NULL_CHECK_RETURN(pPathTag, E_FAIL);

	CComponent::NativeConstruct_Prototype();

	/* Set Pivot */
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);;

	/* Set Path Tag */
	lstrcpy(m_szPathTag, pPathTag);

	/* Load Path Anim */
	FAILED_CHECK_RETURN(CModel_Loader::Load_PathFromFile((void**)(&m_pPathAnim), pFilePath), E_FAIL);
	
	/* Set Duration */
	m_dDurationTime = m_pPathAnim->Get_Duration();

	/* Init Channel's Transformations Identity */
	m_CurAnimTransformations.resize(m_pPathAnim->Get_ChannelCount());
	for (auto& pAnimTransfomation : m_CurAnimTransformations)
		pAnimTransfomation = MH_XMFloat4x4Identity();

	m_NextAnimTransformations.resize(m_pPathAnim->Get_ChannelCount());
	for (auto& pAnimTransfomation : m_NextAnimTransformations)
		pAnimTransfomation = MH_XMFloat4x4Identity();

	return S_OK;
}

HRESULT CPath::NativeConstruct(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tDesc, pArg, sizeof(m_tDesc));

	CComponent::NativeConstruct(nullptr);

	return S_OK;
}

HRESULT CPath::Start_Path(STATE eState, _uint iAnimFrame, _bool bStop)
{
	if (bStop) m_bPlayAnimation = false;

	if (m_bPlayAnimation) return S_OK;
	NULL_CHECK_RETURN(m_pPathAnim, E_FAIL);

	m_eState = eState;
	m_iCurAnimFrame = iAnimFrame;
	m_dCurrentTime = m_pPathAnim->Get_ChannelKeyFrameTimeAvg(iAnimFrame); // 해당 프레임의 시간값으로 적용
	m_bPlayAnimation = true;

	return S_OK;
}

_bool CPath::Update_Animation(_double dTimeDelta, _matrix& WorldMatrix)
{
	if (nullptr == m_pPathAnim) return false;
	if (false == m_bPlayAnimation) return false;

	_double dTicksPerSecond = m_pPathAnim->Get_TicksPerSecond();

	if(STATE_FORWARD == m_eState)
		m_dCurrentTime += fmod(dTicksPerSecond * dTimeDelta, m_dDurationTime);
	else if (STATE_BACKWARD == m_eState)
		m_dCurrentTime -= fmod(dTicksPerSecond * dTimeDelta, m_dDurationTime);

	/* 애니메이션이 끝나면, 업데이트 종료 (멈추기) */
	_float fOffsetTime = 1.1f;
	_bool bFinish = false;
	if (m_eState == STATE_FORWARD && m_dCurrentTime >= m_dDurationTime - fOffsetTime)
		bFinish = true;
	else if (m_eState == STATE_BACKWARD && m_dCurrentTime < fOffsetTime)
		bFinish = true;

	if (bFinish)
	{
		m_eState = STATE_END;
		m_bPlayAnimation = false;
		return false;
	}

	/* Update_AnimTransformations */
	_bool isFrameChange = Update_AnimTransformations();

	/* 보간 비율 조정 */
	m_fLerpRatio += (_float)(fmod(dTicksPerSecond * dTimeDelta, m_dDurationTime));
	if (m_fLerpRatio >= 1.f) m_fLerpRatio = 1.f;

	/* Update_CombinedTransformations */
	WorldMatrix = Update_CombinedTransformations();
	if (isFrameChange) m_fLerpRatio = 0.f;

	/* Update Progress */
	if (STATE_FORWARD == m_eState)
		m_fProgressAnim = _float(m_dCurrentTime / m_dDurationTime);
	else if (STATE_BACKWARD == m_eState)
		m_fProgressAnim = _float((m_dDurationTime - m_dCurrentTime) / m_dDurationTime);

	return true;
}

_bool CPath::Update_AnimTransformations()
{
	NULL_CHECK_RETURN(m_pPathAnim, false);

	_uint iNextAnimFrame = m_iCurAnimFrame;
	_bool isFrameChange = false;

	switch (m_eState)
	{
	case Engine::CPath::STATE_FORWARD:
		iNextAnimFrame += 1;
		isFrameChange = m_pPathAnim->Update_PathTransformation(m_dCurrentTime, m_iCurAnimFrame, m_CurAnimTransformations);
		m_pPathAnim->Update_PathTransformation(m_dCurrentTime, iNextAnimFrame, m_NextAnimTransformations);
		break;
	case Engine::CPath::STATE_BACKWARD:
		iNextAnimFrame -= 1;
		isFrameChange = m_pPathAnim->Update_RewindPathTransformation(m_dCurrentTime, m_iCurAnimFrame, m_CurAnimTransformations);
		m_pPathAnim->Update_RewindPathTransformation(m_dCurrentTime, iNextAnimFrame, m_NextAnimTransformations);
		break;
	}

	return isFrameChange;
}

_fmatrix CPath::Update_CombinedTransformations()
{
	_matrix CurFinalTransformMatrix, NextFinalTransformMatrix, BlendFinalTransformMatrix;

	/* Rotation */
	_float3 fRotateAngle = MH_GetRoatationAnglesToMatrix(m_CurAnimTransformations[CHANNEL_ROTATION]);

	/* Pitch */
	_matrix RotateMatrix = XMMatrixRotationX((fRotateAngle.x));
	/* Yaw */
	if (STATE_FORWARD == m_eState) RotateMatrix *= XMMatrixRotationY(XMConvertToRadians(90.f) - fRotateAngle.z);
	else RotateMatrix *= XMMatrixRotationY(-fRotateAngle.z + XMConvertToRadians(90.f) + XMConvertToRadians(180.f));
	/* Roll */
	RotateMatrix *= XMMatrixRotationZ((fRotateAngle.y));

	CurFinalTransformMatrix = MH_RemoveScale(RotateMatrix * XMLoadFloat4x4(&m_CurAnimTransformations[CHANNEL_TRANSLATION]) * XMLoadFloat4x4(&m_PivotMatrix) * XMLoadFloat4x4(&m_tDesc.WorldMatrix));

	/* Rotation */
	fRotateAngle = MH_GetRoatationAnglesToMatrix(m_NextAnimTransformations[CHANNEL_ROTATION]);

	/* Pitch */
	RotateMatrix = XMMatrixRotationX((fRotateAngle.x));
	/* Yaw */
	if (STATE_FORWARD == m_eState) RotateMatrix *= XMMatrixRotationY(XMConvertToRadians(90.f) - fRotateAngle.z);
	else RotateMatrix *= XMMatrixRotationY(-fRotateAngle.z + XMConvertToRadians(90.f) + XMConvertToRadians(180.f));
	/* Roll */
	RotateMatrix *= XMMatrixRotationZ((fRotateAngle.y));

	NextFinalTransformMatrix = MH_RemoveScale(RotateMatrix * XMLoadFloat4x4(&m_NextAnimTransformations[CHANNEL_TRANSLATION]) * XMLoadFloat4x4(&m_PivotMatrix) * XMLoadFloat4x4(&m_tDesc.WorldMatrix));

	return CurFinalTransformMatrix * (1.f - m_fLerpRatio) + NextFinalTransformMatrix * m_fLerpRatio;
}

CPath* CPath::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pFilePath, const _tchar* pPathTag, _fmatrix PivotMatrix)
{
	CPath* pInstance = new CPath(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pFilePath, pPathTag, PivotMatrix)))
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

	CComponent::Free();
}
