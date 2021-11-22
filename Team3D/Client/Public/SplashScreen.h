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

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CTextures*			m_pTextureCom = nullptr;
	CTextures*			m_pSubTextureCom = nullptr;
	CVIBuffer_Rect*		m_pVIBuffer_RectCom = nullptr;

private:
	_uint				m_iOption = 0;
	_uint				m_iPassNum = 0;
	_float				m_fSortOrder = 0.f;
	_float				m_fScreenAlpha = 0.f;
	_float2				m_vMaskUV;

private:
	HRESULT Ready_Component();
	HRESULT Set_UIVariables_Perspective();

public:
	static CSplashScreen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END