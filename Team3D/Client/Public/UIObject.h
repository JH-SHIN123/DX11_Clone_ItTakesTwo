#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CUIObject abstract : public CGameObject
{
public:
	typedef struct tagPassingData_UI
	{
		tagPassingData_UI() { ZeroMemory(this, sizeof(tagPassingData_UI)); }

		unsigned int		iLevelIndex;
		unsigned int		iRenderGroup;
		unsigned int		iTextureLevelIndex;
		unsigned int		iTextureRenderIndex;
		unsigned int		iSubTextureNum;
		wchar_t				szSubTextureTag[MAX_PATH];
		wchar_t				szTextureTag[MAX_PATH];
		wchar_t				szUITag[MAX_PATH];
		XMFLOAT2			vPos;
		XMFLOAT2			vScale;

	}UI_DESC;

protected:
	explicit CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUIObject(const CUIObject& rhs);
	virtual ~CUIObject() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() PURE;
	virtual HRESULT	NativeConstruct(void* pArg) PURE;
	virtual _int	Tick(_double TimeDelta) PURE;
	virtual _int	Late_Tick(_double TimeDelta) PURE;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) PURE;

public:
	virtual _float Get_DistanceFromCamera() override;

public:
	_uint Get_TextureRenderIndex();

public:
	void Set_Active(_bool bActive) { m_bActive = bActive; }
	void Set_Dead();
	void Set_PlayerID(Player::ID ePlayerID);
	void Set_PosX(_float fPosX);
	void Set_FadeInSpeed(_float fSpeed);
	void Set_FadeOutSpeed(_float fSpeed);
	void Set_FadeOut();
	void Set_Ready();
	void Set_UIAllActivation(_bool IsCheck);

public:
	virtual void Set_TargetPos(_vector vPos);
	virtual void Set_ScaleEffect();

protected:
	_bool m_bActive = true;

protected:
	UI_DESC						m_UIDesc;
	_bool						m_IsDead = false;
	_float						m_fSortOrder = 0.f;
	Player::ID					m_ePlayerID = Player::PLAYER_END;
	_uint						m_iOption = 0;
	_float4						m_vTargetPos;
	_float						m_fFadeInSpeed = 5.f;
	_float						m_fFadeOutSpeed = 5.f;
	_bool						m_IsFadeOut = false;
	_bool						m_IsActive = true;
	_int						m_iColorOption = 0;

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTextures*			m_pTextureCom = nullptr;
	CTextures*			m_pSubTexturCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;

protected:
	HRESULT Ready_Component();
	HRESULT Set_UIDefaultVariables_Perspective(CVIBuffer* pVIBuffer);
	HRESULT Set_UIVariables_Perspective(CVIBuffer* pVIBuffer);
	HRESULT Set_InterActiveVariables_Perspective(CVIBuffer* pVIBuffer);

public:
	virtual CGameObject* Clone_GameObject(void* pArg) PURE;
	virtual void Free() PURE;
};

END