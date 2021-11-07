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

class COrtho_UIObject : public CGameObject
{
public:
	typedef struct tagPassingData_UI
	{
		tagPassingData_UI() { ZeroMemory(this, sizeof(tagPassingData_UI)); }

		unsigned int	iLevelIndex;
		unsigned int	iRenderGroup;
		unsigned int	iTextureLevelIndex;
		unsigned int	iSubTextureNum;
		wchar_t			szSubTextureTag[MAX_PATH];
		wchar_t			szTextureTag[MAX_PATH];
		wchar_t			szUITag[MAX_PATH];
		_float2			vPos;
		_float2			vScale;

	}UI_DESC;

protected:
	explicit COrtho_UIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit COrtho_UIObject(const COrtho_UIObject& rhs);
	virtual ~COrtho_UIObject() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(_uint iDiffTextureIndex = 0, _uint iSubTextureIndex = 0);

protected:
	UI_DESC				m_UIDesc;

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTextures*			m_pTextureCom = nullptr;
	CTextures*			m_pSubTexturCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_Terrain*	m_pVIBufferCom = nullptr;

private:
	HRESULT Ready_Component();


public:
	static COrtho_UIObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END