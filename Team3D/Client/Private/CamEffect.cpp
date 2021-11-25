#include "stdafx.h"
#include "..\public\CamEffect.h"


CCamEffect::CCamEffect()
{
}

HRESULT CCamEffect::NativeConstruct(const _tchar * pEffectName)
{
	lstrcpy(m_szName, pEffectName);

	for (_uint i = 0; i < 2; i++)
		ReSet_Effect((CFilm::ScreenType)i);
	return S_OK;
}

HRESULT CCamEffect::Add_CamShakeCycleDesc(CamShakeCycleDesc * pDesc)
{
	if (nullptr == pDesc)
		return E_FAIL;
	m_ShakeDescs.emplace_back(pDesc);
	return S_OK;
}

void CCamEffect::ReSet_Effect(CFilm::ScreenType eScreen)
{
	m_dTime[eScreen] = 0.0;
	m_iCurrentIdx[eScreen] = 0;
	XMStoreFloat4x4(&m_matApplyEffectMatrix[eScreen], XMMatrixIdentity());
	m_bFinish[eScreen] = false;
	m_dCurBlendInTime[eScreen] = 0.0;
	m_dCurBlendOutTime[eScreen] = 0.0;

}


_fmatrix CCamEffect::MakeShakeMatrix_Location(_fmatrix matIn, _double dCurrentTime, CamShakeCycleDesc & tDesc)
{
	if (dCurrentTime >= tDesc.dFinishTime || dCurrentTime <= tDesc.dStartTime) return matIn; //
	if (!tDesc.bOnCamShakeOption[(_uint)CamShakeOption::CamShake_Loc_Right] &&
		!tDesc.bOnCamShakeOption[(_uint)CamShakeOption::CamShake_Loc_Up] &&
		!tDesc.bOnCamShakeOption[(_uint)CamShakeOption::CamShake_Loc_Look])
		return matIn;

	_matrix matResult = matIn;
	_bool bElapseMiddleTime = dCurrentTime > tDesc.dMiddleTime; //중앙을 넘어갓나?
	_float fProgressPercentage = bElapseMiddleTime ?
		(_float)((dCurrentTime - tDesc.dMiddleTime) / (tDesc.dFinishTime - tDesc.dMiddleTime)) :
		(_float)((dCurrentTime - tDesc.dStartTime) / (tDesc.dMiddleTime - tDesc.dStartTime)); //진행 퍼센트(중앙시간 넘어가면 다시)

	_float fZ =
		tDesc.bOnCamShakeOption[(_uint)CamShakeOption::CamShake_Loc_Look] ?
		(sinf(fProgressPercentage * (_float)MATH_PI)* (bElapseMiddleTime ?
			(_float)tDesc.tCamShakeDesc[(_uint)CamShakeOption::CamShake_Loc_Look].dMinForce :
			(_float)tDesc.tCamShakeDesc[(_uint)CamShakeOption::CamShake_Loc_Look].dMaxForce)) : 0.f,

		fX =
		tDesc.bOnCamShakeOption[(_uint)CamShakeOption::CamShake_Loc_Right] ?
		(_float)(sinf(fProgressPercentage * (_float)MATH_PI)* (_float)(bElapseMiddleTime ?
			(_float)tDesc.tCamShakeDesc[(_uint)CamShakeOption::CamShake_Loc_Right].dMinForce :
			(_float)tDesc.tCamShakeDesc[(_uint)CamShakeOption::CamShake_Loc_Right].dMaxForce)) : 0.f,

		fY =
		tDesc.bOnCamShakeOption[(_uint)CamShakeOption::CamShake_Loc_Up] ?
		(_float)(sinf(fProgressPercentage * (_float)MATH_PI)* (_float)(bElapseMiddleTime ?
			(_float)tDesc.tCamShakeDesc[(_uint)CamShakeOption::CamShake_Loc_Up].dMinForce :
			(_float)tDesc.tCamShakeDesc[(_uint)CamShakeOption::CamShake_Loc_Up].dMaxForce)) : 0.f;
	
	matResult.r[3] += XMVectorSet(fX, fY, fZ, 0.f);
	return matResult;
}

_fmatrix CCamEffect::MakeShakeMatrix_QuarternionRot(_fmatrix matIn, _double dCurrentTime, CamShakeCycleDesc& tDesc)
{
	if (dCurrentTime >= tDesc.dFinishTime || dCurrentTime <= tDesc.dStartTime) return matIn; //
	if (!tDesc.bOnCamShakeOption[(_uint)CamShakeOption::CamShake_Rot_Right] &&
		!tDesc.bOnCamShakeOption[(_uint)CamShakeOption::CamShake_Rot_Up] &&
		!tDesc.bOnCamShakeOption[(_uint)CamShakeOption::CamShake_Rot_Look])
		return matIn;

	_matrix matResult = matIn;
	_bool bElapseMiddleTime = dCurrentTime > tDesc.dMiddleTime; //중앙을 넘어갓나?
	_float fProgressPercentage = bElapseMiddleTime? 
	(_float)(	(dCurrentTime - tDesc.dMiddleTime) / (tDesc.dFinishTime - tDesc.dMiddleTime)) :
	(_float)(	(dCurrentTime - tDesc.dStartTime) / (tDesc.dMiddleTime - tDesc.dStartTime)); //진행 퍼센트(중앙시간 넘어가면 다시)

	_float fRoll =
		tDesc.bOnCamShakeOption[(_uint)CamShakeOption::CamShake_Rot_Look] ? 
		(sinf(fProgressPercentage * (_float)MATH_PI)* (bElapseMiddleTime ?
			(_float)tDesc.tCamShakeDesc[(_uint)CamShakeOption::CamShake_Rot_Look].dMinForce :
			(_float)tDesc.tCamShakeDesc[(_uint)CamShakeOption::CamShake_Rot_Look].dMaxForce)) : 0.f,
		
		fPitch =
		tDesc.bOnCamShakeOption[(_uint)CamShakeOption::CamShake_Rot_Right] ? 
		(sinf(fProgressPercentage * (_float)MATH_PI)* (bElapseMiddleTime ?
			(_float)tDesc.tCamShakeDesc[(_uint)CamShakeOption::CamShake_Rot_Right].dMinForce :
			(_float)tDesc.tCamShakeDesc[(_uint)CamShakeOption::CamShake_Rot_Right].dMaxForce)) : 0.f,

		fYaw= 
		tDesc.bOnCamShakeOption[(_uint)CamShakeOption::CamShake_Rot_Up] ? 
		(sinf(fProgressPercentage * (_float)MATH_PI)* (bElapseMiddleTime ?
			(_float)tDesc.tCamShakeDesc[(_uint)CamShakeOption::CamShake_Rot_Up].dMinForce :
			(_float)tDesc.tCamShakeDesc[(_uint)CamShakeOption::CamShake_Rot_Up].dMaxForce)) : 0.f;
	_vector vPos = matResult.r[3];
	matResult.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	matResult = matResult*
		XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(fRoll,fPitch,fYaw));
	matResult.r[3] = vPos;
	return matResult;
}



HRESULT CCamEffect::Tick_CamEffect(CFilm::ScreenType eScreen, _double dTimeDelta, _fmatrix matIn)
{
	

	BlendState eBlendState = Blend_None;
	if (m_bIsBlendIn)
	{
		if (m_dCurBlendInTime[eScreen] < m_fBlendInTime)	//시작하기블렌딩중
		{
			m_dCurBlendInTime[eScreen] += dTimeDelta;
			eBlendState = Blend_In;
		}
		else
			m_dCurBlendInTime[eScreen] = (_double)m_fBlendInTime;
	}
	if (m_bIsBlendOut)
	{
		if (m_dTime[eScreen] > m_dDuration + m_dCurBlendInTime[eScreen]) //끝나기블렌딩중
		{
			m_dCurBlendOutTime[eScreen] += dTimeDelta;
			eBlendState = Blend_Out;
		}
		else if(m_dCurBlendOutTime[eScreen] >= (_double)m_fBlendOutTime)
			m_dCurBlendOutTime[eScreen] = (_double)m_fBlendOutTime;
	}
	if(eBlendState == Blend_None) m_dTime[eScreen] += dTimeDelta;

	//현재길이
	if (m_dTime[eScreen] + (m_bIsBlendIn ? m_dCurBlendInTime[eScreen] : 0.0) + (m_bIsBlendOut ? m_dCurBlendOutTime[eScreen] : 0.0)
		>= m_dDuration  +(m_bIsBlendIn ? m_fBlendInTime : 0.0) + (m_bIsBlendOut ? m_fBlendOutTime : 0.0))//전체길이
		m_bFinish[eScreen] = true;

	_uint iLastIdx =(_uint)m_ShakeDescs.size();
	if (m_dDuration <= m_dTime[eScreen] && !eBlendState) //블렌딩중?
	{
		XMStoreFloat4x4(&m_matApplyEffectMatrix[eScreen], matIn);
		return S_OK;
	}
	else if (m_iCurrentIdx[eScreen] + 1 >= iLastIdx && !eBlendState) //블렌딩중?
	{
		XMStoreFloat4x4(&m_matApplyEffectMatrix[eScreen], matIn);
		return S_OK;
	}
	else
	{

		if (eBlendState == Blend_None && m_dTime[eScreen] > m_ShakeDescs[m_iCurrentIdx[eScreen] + 1]->dStartTime)
			m_iCurrentIdx[eScreen]++;

		CamShakeCycleDesc tDesc = *m_ShakeDescs[m_iCurrentIdx[eScreen]];
		_matrix matResult = matIn;
		_double dCurTime = m_dTime[eScreen];
		switch (eBlendState)
		{
		case CCamEffect::Blend_In:
			dCurTime = m_dCurBlendInTime[eScreen];
			tDesc.dStartTime = 0.0;
			tDesc.dMiddleTime = (_double)m_fBlendInTime*0.5;
			tDesc.dFinishTime = (_double)m_fBlendInTime;
			for (_uint i = 0; i < 6; i++)
			{
				tDesc.tCamShakeDesc[i].dMaxForce *= tDesc.bOnCamShakeOption[i] ? m_dCurBlendInTime[eScreen] : 0.0;
				tDesc.tCamShakeDesc[i].dMinForce *= tDesc.bOnCamShakeOption[i] ? m_dCurBlendInTime[eScreen] : 0.0;
			}
			break;
		case CCamEffect::Blend_Out:
			dCurTime = m_dCurBlendOutTime[eScreen];
			tDesc.dStartTime = 0.0;
			tDesc.dMiddleTime = (_double)m_fBlendOutTime*0.5;
			tDesc.dFinishTime = (_double)m_fBlendOutTime;
			for (_uint i = 0; i < 6; i++)
			{
				tDesc.tCamShakeDesc[i].dMaxForce *= tDesc.bOnCamShakeOption[i] ? 1.0 - m_dCurBlendOutTime[eScreen] : 0.0;
				tDesc.tCamShakeDesc[i].dMinForce *= tDesc.bOnCamShakeOption[i] ? 1.0 - m_dCurBlendOutTime[eScreen] : 0.0;
			}
			break;
		}
		
		matResult = MakeShakeMatrix_QuarternionRot(matIn, dCurTime, tDesc);
		
		matResult = MakeShakeMatrix_Location(matResult, dCurTime, tDesc);


		XMStoreFloat4x4(&m_matApplyEffectMatrix[eScreen], matResult);
	}

	return S_OK;
}

_fmatrix CCamEffect::Get_ApplyEffectMatrix(CFilm::ScreenType eScreen)
{
	return XMLoadFloat4x4(&m_matApplyEffectMatrix[eScreen]);
}

CCamEffect * CCamEffect::Create(const _tchar* pEffectName)
{
	CCamEffect* pInstance = new CCamEffect;
	if (FAILED(pInstance->NativeConstruct(pEffectName)))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamEffect::Free()
{
	for (auto& pDesc : m_ShakeDescs)
		Safe_Delete(pDesc);
	m_ShakeDescs.clear();
}


