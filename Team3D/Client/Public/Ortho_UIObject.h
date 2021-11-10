#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTextures;
class CTransform;
class CVIBuffer_Terrain;
END

BEGIN(Client)

class COrtho_UIObject abstract : public CGameObject
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
	explicit COrtho_UIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit COrtho_UIObject(const COrtho_UIObject& rhs);
	virtual ~COrtho_UIObject() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() PURE;
	virtual HRESULT	NativeConstruct(void* pArg) PURE;
	virtual _int	Tick(_double TimeDelta) PURE;
	virtual _int	Late_Tick(_double TimeDelta) PURE;
	virtual HRESULT	Render() PURE;

public:
	void Set_Dead();

protected:
	UI_DESC						m_UIDesc;
	_bool						m_IsDead = false;

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTextures*			m_pTextureCom = nullptr;
	CTextures*			m_pSubTexturCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;

protected:
	HRESULT Ready_Component();


public:
	virtual CGameObject* Clone_GameObject(void* pArg) PURE;
	virtual void Free() PURE;
};

END