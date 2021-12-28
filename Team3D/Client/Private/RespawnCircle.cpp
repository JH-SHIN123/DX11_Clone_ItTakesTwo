#include "stdafx.h"
#include "..\Public\RespawnCircle.h"

#include "GameInstance.h"
#include "UI_Generator.h"
#include "Cody.h"
#include "May.h"

CRespawnCircle::CRespawnCircle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)	
	: CUIObject(pDevice, pDeviceContext)
{
}

CRespawnCircle::CRespawnCircle(const CUIObject & rhs)
	: CUIObject(rhs)
{
}

HRESULT CRespawnCircle::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CRespawnCircle::NativeConstruct_Prototype(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UI_DESC));

	return S_OK;
}

HRESULT CRespawnCircle::NativeConstruct(void * pArg)
{
	CUIObject::NativeConstruct(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (pArg != nullptr)
		memcpy(&m_iOption, pArg, sizeof(_uint));

	m_vStartScale = m_UIDesc.vScale;
	m_UIDesc.vScale.x = m_UIDesc.vScale.x * 3.f;
	m_UIDesc.vScale.y = m_UIDesc.vScale.y * 3.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

	m_pSubTexturesCom = (CTextures*)m_pGameInstance->Add_Component_Clone(Level::LEVEL_STATIC, TEXT("CoolDown"));
	m_pNoiseTextureCom = (CTextures*)m_pGameInstance->Add_Component_Clone(Level::LEVEL_STATIC, TEXT("Noise"));

	if (1 == m_iOption)
		m_iPassNum = 6;
	else
		m_iPassNum = 5;

	m_vUV = { 0.f, 0.f };

	return S_OK;
}

_int CRespawnCircle::Tick(_double TimeDelta)
{
	if (true == m_IsDead)
		return EVENT_DEAD;

	CUIObject::Tick(TimeDelta);

	Spawn_Effect(TimeDelta);
	Alpha_Effect(TimeDelta);

	if (m_IsFullGuage && false == m_IsRespawnCharacterOnce)
	{
		if (Player::ID::Cody == m_ePlayerID)
		{
			CCody* pCody = (CCody*)(DATABASE->GetCody());
			if (pCody) pCody->Respawn_InBossroom();
			m_IsRespawnCharacterOnce = true;
		}
		else if (Player::ID::May == m_ePlayerID)
		{
			CMay* pMay = (CMay*)(DATABASE->GetMay());
			if (pMay) pMay->Respawn_InBossroom();
			m_IsRespawnCharacterOnce = true;
		}
	}

	return _int();
}

_int CRespawnCircle::Late_Tick(_double TimeDelta)
{
	_int iActive = CUIObject::Late_Tick(TimeDelta);

	if (2 == iActive)
		return NO_EVENT;

	m_fSubTime += (_float)TimeDelta * 3.f;
	m_fHeartTime += (_float)TimeDelta * 5.f;

	if (360.f <= m_fSubTime)
		m_fSubTime = 0.f;

	if (360.f <= m_fHeartTime)
		m_fHeartTime = 0.f;

	Set_Gauge(TimeDelta);

	m_vUV.x += (_float)TimeDelta * 0.5f;
	m_vUV.y += (_float)TimeDelta * 0.5f;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_UI, this);
}

HRESULT CRespawnCircle::Render(RENDER_GROUP::Enum eGroup)
{
	CUIObject::Render(eGroup);

	if (FAILED(CUIObject::Set_UIVariables_Perspective(m_pVIBuffer_RectCom)))
		return E_FAIL;

	m_pVIBuffer_RectCom->Set_Variable("g_fAlpha", &m_fAlpha, sizeof(_float));

	if (0 == m_iOption)
	{
		m_pVIBuffer_RectCom->Set_Variable("g_Angle", &m_fSubTime, sizeof(_float));
		m_pVIBuffer_RectCom->Set_Variable("g_fHeartTime", &m_fHeartTime, sizeof(_float));

		m_pVIBuffer_RectCom->Set_Variable("g_Time", &m_fTime, sizeof(_float));
		m_pVIBuffer_RectCom->Set_Variable("g_UV", &m_vUV, sizeof(_float2));
		m_pVIBuffer_RectCom->Set_ShaderResourceView("g_DiffuseSubTexture", m_pSubTexturesCom->Get_ShaderResourceView(0));
		m_pVIBuffer_RectCom->Set_ShaderResourceView("g_DiffuseNoiseTexture", m_pNoiseTextureCom->Get_ShaderResourceView(0));

	}

	m_pVIBuffer_RectCom->Render(m_iPassNum);

	return S_OK;
}

HRESULT CRespawnCircle::Ready_Component()
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_VIBuffer_Rect_UI"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer_RectCom), E_FAIL);

	return S_OK;
}

void CRespawnCircle::Set_Gauge(_double TimeDelta)
{
	if (true == m_IsFullGuage)
		return;

#ifdef __CONTROL_MAY_KEYBOARD
	if ((m_ePlayerID == Player::ID::Cody && m_pGameInstance->Key_Down(DIK_E)) || (m_ePlayerID == Player::ID::May && m_pGameInstance->Key_Down(DIK_O)))
#else
	if ((m_ePlayerID == Player::ID::Cody && m_pGameInstance->Key_Down(DIK_E)) || (m_ePlayerID == Player::ID::May && m_pGameInstance->Pad_Key_Down(DIP_Y)))
#endif
	{
		m_fTime += (_float)TimeDelta * 2.f;

		m_iRespawnOption = 1;
		//UI_Generator->Set_Scale(Player::Cody, UI::RespawnCircle, _float2(30.f, 30.f));;

		if (1.f <= m_fTime)
			m_IsFullGuage = true;
	}
	else
	{
		m_fTime -= (_float)TimeDelta * 0.05f;

		if (0.f >= m_fTime)
			m_fTime = 0.f;

		m_fColorChangeTime += (_float)TimeDelta;

		if (0.1f <= m_fColorChangeTime)
		{
			m_iRespawnOption = 0;
			m_fColorChangeTime = 0.f;
		}
	}

	m_pVIBuffer_RectCom->Set_Variable("g_iRespawnOption", &m_iRespawnOption, sizeof(_int));

}

void CRespawnCircle::Spawn_Effect(_double TimeDelta)
{
	if (true == m_IsSpawnEnd)
		return;

	if (m_vStartScale.x < m_UIDesc.vScale.x)
	{
		m_UIDesc.vScale.x -= (_float)TimeDelta * 2000.f;
		m_UIDesc.vScale.y -= (_float)TimeDelta * 2000.f;
		m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));
	}
	else
	{
		m_UIDesc.vScale.x = m_vStartScale.x;
		m_UIDesc.vScale.y = m_vStartScale.y;
		m_pTransformCom->Set_Scale(XMVectorSet(m_UIDesc.vScale.x, m_UIDesc.vScale.y, 0.f, 0.f));

		m_IsSpawnEnd = true;
	}
}

void CRespawnCircle::Alpha_Effect(_double TimeDelta)
{
	if (false == m_IsFullGuage)
		return;

	m_fAlpha += (_float)TimeDelta * 1.5f;

	if (1.f <= m_fAlpha)
	{
		if (m_ePlayerID == Player::Cody)
		{
			UI_Delete(Cody, RespawnCircle);
		}
		else
		{
			UI_Delete(May, RespawnCircle);
		}
	}
}

CRespawnCircle * CRespawnCircle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CRespawnCircle* pInstance = new CRespawnCircle(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create RespawnCircle Prototype, Error to CRespawnCircle::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRespawnCircle::Clone_GameObject(void * pArg)
{
	CRespawnCircle* pClone = new CRespawnCircle(*this);

	if (FAILED(pClone->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CRespawnCircle, Error to CRespawnCircle::Clone_GameObject");
		Safe_Release(pClone);
	}

	return pClone;
}

void CRespawnCircle::Free()
{
	Safe_Release(m_pVIBuffer_RectCom);
	Safe_Release(m_pSubTexturesCom);
	Safe_Release(m_pNoiseTextureCom);

	CUIObject::Free();
}
