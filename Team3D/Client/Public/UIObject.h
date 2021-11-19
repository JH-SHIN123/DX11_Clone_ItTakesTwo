#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTextures;
class CTransform;
class CVIBuffer_Terrain;
class CVIBuffer;
END

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
	virtual HRESULT	Render() PURE;

public:
	virtual _float Get_DistanceFromCamera() override;

public:
	void Set_Dead();
	void Set_PlayerID(Player::ID ePlayerID);
	void Set_PosX(_float fPosX);
	void Set_TargetPos(_vector vPos);

protected:
	UI_DESC						m_UIDesc;
	_bool						m_IsDead = false;
	_float						m_fSortOrder = 0.f;
	Player::ID					m_ePlayerID = Player::PLAYER_END;
	_uint						m_iOption = 0;
	_float4						m_vTargetPos;

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTextures*			m_pTextureCom = nullptr;
	CTextures*			m_pSubTexturCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;

protected:
	HRESULT Ready_Component();
	HRESULT Set_UIVariables_Perspective(CVIBuffer* pVIBuffer);
	HRESULT Set_InterActiveVariables_Perspective(CVIBuffer* pVIBuffer);


public:
	virtual CGameObject* Clone_GameObject(void* pArg) PURE;
	virtual void Free() PURE;
};

END