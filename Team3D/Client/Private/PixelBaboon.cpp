#include "stdafx.h"
#include "..\Public\PixelBaboon.h"
#include "GameInstance.h"
#include "Cody.h"
#include "May.h"
#include "PixelHeart.h"
#include "PixelShield.h"
#include "PixelUFO.h"
#include "PixelArrow.h"
#include "RunningMoonBaboon.h"
#include "MoonUFO.h"
#include "PixelCrossHair.h"
#include"CutScenePlayer.h"
#include "Script.h"

CPixelBaboon::CPixelBaboon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CGameObject(pDevice, pDeviceContext)
{
}

CPixelBaboon::CPixelBaboon(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPixelBaboon::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPixelBaboon::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_ArcadePixel"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom), E_FAIL);
	CTransform::TRANSFORM_DESC TransformDesc;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.f);
	TransformDesc.dSpeedPerSec = 1.f;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_PixelBaboon"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom), E_FAIL);

	//vPosition = { 64.0174942f, 601.063843f + 0.56f, 1012.77844f - 0.29f, 1.f };
	if (nullptr != pArg)
	{
		_vector* vPosition = (_vector*)pArg;
		vCenterPos = (*(_vector*)pArg);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, *vPosition);
		m_pTransformCom->Set_Scale(XMVectorSet(0.1f, 0.1f, 0.1f, 0.f));
	}

	CGameObject* pHeart[3] = { nullptr };

	_vector vHeartPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.04f, 0.f, 0.f);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_PixelHeart"), Level::LEVEL_STAGE, TEXT("GameObject_PixelHeart"), &vHeartPos, &pHeart[0]), E_FAIL);

	vHeartPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.04f, 0.02f, 0.f, 0.f);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_PixelHeart"), Level::LEVEL_STAGE, TEXT("GameObject_PixelHeart"), &vHeartPos, &pHeart[1]), E_FAIL);

	vHeartPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(-0.04f, 0.02f, 0.f, 0.f);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_PixelHeart"), Level::LEVEL_STAGE, TEXT("GameObject_PixelHeart"), &vHeartPos, &pHeart[2]), E_FAIL);

	m_pPixelHeart[0] = static_cast<CPixelHeart*>(pHeart[0]);
	m_pPixelHeart[1] = static_cast<CPixelHeart*>(pHeart[1]);
	m_pPixelHeart[2] = static_cast<CPixelHeart*>(pHeart[2]);

	CGameObject* pShield = nullptr;

	_vector vShieldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_PixelShield"), Level::LEVEL_STAGE, TEXT("GameObject_PixelShield"), &vShieldPos, &pShield), E_FAIL);
	m_pPixelShield = static_cast<CPixelShield*>(pShield);

	CGameObject* pArrow = nullptr;

	_vector vArrowPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_PixelArrow"), Level::LEVEL_STAGE, TEXT("GameObject_PixelArrow"), &vArrowPos, &pArrow), E_FAIL);
	m_pPixelArrow = static_cast<CPixelArrow*>(pArrow);

	DATABASE->Set_PixelBaboon(this);

	m_pGameInstance->Set_SoundVolume(CHANNEL_MOONUFO_SEARCH1, m_fSearchVolumeFar);
	m_pGameInstance->Play_Sound(L"UFO_Search1.wav", CHANNEL_MOONUFO_SEARCH1, m_fSearchVolumeFar);
	m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_SEARCH1);

	m_pGameInstance->Set_SoundVolume(CHANNEL_MOONUFO_SEARCH2, m_fSearchVolumeMiddle);
	m_pGameInstance->Play_Sound(L"UFO_Search2.wav", CHANNEL_MOONUFO_SEARCH2, m_fSearchVolumeMiddle);
	m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_SEARCH2);

	m_pGameInstance->Set_SoundVolume(CHANNEL_MOONUFO_SEARCH3, m_fSearchVolumeNear);
	m_pGameInstance->Play_Sound(L"UFO_Search3.wav", CHANNEL_MOONUFO_SEARCH3, m_fSearchVolumeNear);
	m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_SEARCH3);

	return S_OK;
}

_int CPixelBaboon::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (((CRunningMoonBaboon*)DATABASE->Get_RunningMoonBaboon())->Get_IsHitLaser() == true && m_bTriggerOnce == false && m_bStartHeartDelay == false)
	{
		m_bStartHeartDelay = true;
		m_bTriggerOnce = true;
		m_bBlinking = true;
		m_iLifeCount -= 1;

		if (m_iLifeCount == 2)
		{
			m_pPixelHeart[2]->Set_LifeCountRenderOff(true);
			if (SCRIPT->Get_Script_Played(52) == false)
			{
				SCRIPT->VoiceFile_No52();
				SCRIPT->Set_Script_Played(52, true);
			}
		}
		else if (m_iLifeCount == 1)
		{
			m_pPixelHeart[1]->Set_LifeCountRenderOff(true);
			if (SCRIPT->Get_Script_Played(53) == false)
			{
				SCRIPT->VoiceFile_No53();
				SCRIPT->Set_Script_Played(53, true);
			}
		}
		else if (m_iLifeCount == 0)
		{
#ifdef __PLAY_CUTSCENE
			if (CCutScenePlayer::GetInstance()->Get_IsCutScenePlayed(CCutScene::CutSceneOption::CutScene_Outro) == false)
			{
				CCutScenePlayer::GetInstance()->Start_CutScene(TEXT("CutScene_Outro"));
				CCutScenePlayer::GetInstance()->Set_IsCutScenePlayed(CCutScene::CutSceneOption::CutScene_Outro,true);
			}

#endif
			m_pPixelHeart[0]->Set_LifeCountRenderOff(true);    
		}
	}

	if (m_bStartHeartDelay == true)
	{
		m_fHeartDelay += (_float)dTimeDelta;
		if (m_fHeartDelay > 10.f)
		{
			m_bStartHeartDelay = false;
			m_fHeartDelay = 0.f;
		}
	}

	if (m_iLifeCount > 0)
	{
		Check_Degree_And_Distance_From_MoonUFO(dTimeDelta);
		Check_Distance_From_UFO(dTimeDelta);
		Set_Hearts_Pos();
	}
	else if (m_iLifeCount == 0)
	{
		m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_SEARCH2);
		m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_SEARCH1);
		m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_SEARCH3);
	}

	return _int();
}

_int CPixelBaboon::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (m_bBlinking == false)
	{
		if (m_bRender == true)
			return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
	}
	else
	{
		m_fBlinkingTime += (_float)dTimeDelta;
		if (m_fBlinkingTime > 0.1f)
		{
			m_fBlinkingTime = 0.f;
			++m_iBlinkingCount;
			m_bBlink = !m_bBlink;
		}
		else if (m_fBlinkingTime <= 0.1f && m_bBlink == false)
		{
			if (m_bRender == true)
				return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT_NO_BLUR, this);
		}

		if (m_iBlinkingCount == 10)
		{
			m_bTriggerOnce = false;
			m_bBlinking = false;
			m_bBlink = false;
			m_iBlinkingCount = 0;
			m_pPixelShield->Set_Render_State(true);
			m_pPixelHeart[2]->Set_Render_Off(true);
			m_pPixelHeart[1]->Set_Render_Off(true);
			m_pPixelHeart[0]->Set_Render_Off(true);
		}
	}

	return _int();
}

HRESULT CPixelBaboon::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	m_pVIBufferCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_ShaderResourceView(0));

	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);
	m_pVIBufferCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pVIBufferCom->Render(1);

	return S_OK;
}

void CPixelBaboon::Set_Hearts_Pos()
{
	for (_int i = 0; i < 3; ++i)
	{
		_vector vOffSet = XMVectorZero();
		if (i == 0)
			vOffSet = XMVectorSet(0.f, 0.04f, 0.f, 0.f);
		if (i == 1)
			vOffSet = XMVectorSet(0.04f, 0.02f, 0.f, 0.f);
		if (i == 2)
			vOffSet = XMVectorSet(-0.04f, 0.02f, 0.f, 0.f);

		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vOffSet;
		m_pPixelHeart[i]->Set_Position(vPosition);

		m_pPixelShield->Set_Position(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
}

void CPixelBaboon::Check_Degree_And_Distance_From_MoonUFO(_double dTimeDelta)
{
	_vector vMoonBaboonPos = ((CRunningMoonBaboon*)DATABASE->Get_RunningMoonBaboon())->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vUFOPos = ((CMoonUFO*)DATABASE->Get_MoonUFO())->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vUFOUp = ((CMoonUFO*)DATABASE->Get_MoonUFO())->Get_Transform()->Get_State(CTransform::STATE_UP);

	_vector vMoonUFORight = XMVector3Normalize(-((CMoonUFO*)DATABASE->Get_MoonUFO())->Get_Transform()->Get_State(CTransform::STATE_RIGHT));

	_vector vProjPos = vUFOPos - XMVector3Normalize(vUFOUp) * 31.f;
	_vector vProjPosToBaboon = vMoonBaboonPos - vProjPos;
	_vector vUFOToBaboon = vMoonBaboonPos - vUFOPos;


	_float fRadian = XMVectorGetX(XMVector3AngleBetweenNormals(XMVector3Normalize(vProjPosToBaboon), vMoonUFORight));
	_float fLength = XMVectorGetX((XMVector3Length(vProjPosToBaboon)));

	_float fCCW = XMVectorGetX(XMVector3Dot(vUFOUp, XMVector3Cross(XMVector3Normalize(vProjPosToBaboon), vMoonUFORight)));

	_float fX = 0.f;
	_float fY = 0.f;

	fX = vCenterPos.m128_f32[0] - (fLength / 125.f) * cos(fRadian);

	if (fCCW > 0.f)
		fRadian *= -1.f;
	fY = vCenterPos.m128_f32[1] + (fLength / 125.f) * sin(fRadian);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(fX, fY, vCenterPos.m128_f32[2], 1.f));

}

void CPixelBaboon::Check_Distance_From_UFO(_double dTimeDelta)
{
	if (nullptr == DATABASE->Get_PixelUFO())
		return;

	_vector vUFOPosition = ((CPixelUFO*)DATABASE->Get_PixelUFO())->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vMoonBaboonPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float vDist = XMVectorGetX(XMVector3Length(vMoonBaboonPosition - vUFOPosition));

	if (vDist > 0.35f)
	{
		/* SCRIPT && SOUND */
		_bool IsMayUFO = ((CMay*)DATABASE->GetMay())->Get_IsInUFO();
		_bool IsCodyUFO = ((CCody*)DATABASE->GetCody())->Get_IsInArcadeJoyStick();
		if (SCRIPT->Get_Script_Played(50) == false && IsMayUFO == true && IsCodyUFO == true)
		{
			SCRIPT->VoiceFile_No50();
			SCRIPT->Set_Script_Played(50, true);
		}

		m_pPixelHeart[0]->Set_Render_Off(true);
		m_pPixelHeart[1]->Set_Render_Off(true);
		m_pPixelHeart[2]->Set_Render_Off(true);
		m_pPixelShield->Set_Render_State(false);
		m_pPixelArrow->Set_RenderState(true);
		m_bRender = false;
	}
	else
	{
		m_pPixelHeart[0]->Set_Render_Off(false);
		m_pPixelHeart[1]->Set_Render_Off(false);
		m_pPixelHeart[2]->Set_Render_Off(false);
		m_pPixelArrow->Set_RenderState(false);
		m_bRender = true;
	}

	_vector vCrossHairPos = ((CPixelCrossHair*)DATABASE->Get_PixelCrossHair())->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_float vCompensateDist = XMVectorGetX(XMVector3Length(vMoonBaboonPosition - vCrossHairPos));

	/* Sounds */
	if (((CCody*)DATABASE->GetCody())->Get_IsInArcadeJoyStick() == true)
	{
		if (vCompensateDist > 0.25f)
		{
			m_fMiddleSoundDelay = 0.f;
			m_fNearSoundDelay = 0.f;

			m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_SEARCH2);
			m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_SEARCH3);

			m_fFarSoundDelay += (_float)dTimeDelta;

			if (m_fFarSoundDelay > 1.f)
			{
				m_pGameInstance->Play_Sound(L"UFO_Search1.wav", CHANNEL_MOONUFO_SEARCH1, m_fSearchVolumeFar, false);
				m_fFarSoundDelay = 0.f;
			}
		}
		else if (vCompensateDist <= 0.25f && vCompensateDist > 0.1f)
		{
			m_fFarSoundDelay = 0.f;
			m_fNearSoundDelay = 0.f;

			m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_SEARCH3);
			m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_SEARCH1);

			m_fMiddleSoundDelay += (_float)dTimeDelta;
			if (m_fMiddleSoundDelay > 0.5f)
			{
				m_pGameInstance->Play_Sound(L"UFO_Search2.wav", CHANNEL_MOONUFO_SEARCH2, m_fSearchVolumeMiddle, false);
				m_fMiddleSoundDelay = 0.f;
			}
		}
		else if (vCompensateDist <= 0.1f)
		{
			m_fFarSoundDelay = 0.f;
			m_fMiddleSoundDelay = 0.f;

			m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_SEARCH1);
			m_pGameInstance->Stop_Sound(CHANNEL_MOONUFO_SEARCH2);

			m_fNearSoundDelay += (_float)dTimeDelta;
			if (m_fNearSoundDelay > 0.4f)
			{
				m_pGameInstance->Play_Sound(L"UFO_Search3.wav", CHANNEL_MOONUFO_SEARCH3, m_fSearchVolumeNear, false);
				m_fNearSoundDelay = 0.f;
			}
		}	
	}

	if (vCompensateDist < 0.03f && DATABASE->Get_LaserGauge() > 0.9f)
	{
		((CMoonUFO*)DATABASE->Get_MoonUFO())->Compensate_LaserDir(true);
	}
		

}


CPixelBaboon * CPixelBaboon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPixelBaboon* pInstance = new CPixelBaboon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create PixelBaboon Prototype, Error to CPixelBaboon::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPixelBaboon::Clone_GameObject(void * pArg)
{
	CPixelBaboon* pClone = new CPixelBaboon(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CPixelBaboon, Error to CPixelBaboon::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPixelBaboon::Free()
{
	CGameObject::Free();

	Safe_Release(m_pPixelShield);
	Safe_Release(m_pPixelArrow);

	for (_int i = 0; i < 3; ++i)
	{
		Safe_Release(m_pPixelHeart[i]);
	}

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);

}
