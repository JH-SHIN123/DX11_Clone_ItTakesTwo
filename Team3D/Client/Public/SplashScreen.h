#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CTextures;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CSplashScreen : public CGameObject
{
protected:
	explicit CSplashScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSplashScreen(const CSplashScreen& rhs);
	virtual ~CSplashScreen() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual _float Get_DistanceFromCamera() override;
	void Render_Font();

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CTextures*			m_pTextureCom = nullptr;
	CTextures*			m_pSubTextureCom = nullptr;
	CVIBuffer_Rect*		m_pVIBuffer_RectCom = nullptr;

	CFontDraw*			m_pFont = nullptr;

private:
	_uint				m_iOption = 0;
	_uint				m_iPassNum = 0;
	_float				m_fSortOrder = 0.f;
	_float				m_fScreenAlpha = 0.f;
	_float2				m_vMaskUV;
	_bool				m_IsDisappear = false;
	_bool				m_IsDead = false;
	_bool				m_IsBackScreenAlpha = false;

	_float				m_fFontAlpha = 0.f;
	_bool				m_bAlphaCheck = false;
	_double				m_dCoolTime = 0.0;

private:
	HRESULT Ready_Component();
	HRESULT Set_UIVariables_Perspective();
	void Logo_DisAppearing(_double TimeDelta);


public:
	static CSplashScreen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END