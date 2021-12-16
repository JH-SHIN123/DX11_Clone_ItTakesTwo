#pragma once
#include "Component.h"
#include "DebugDraw.h"

BEGIN(Engine)
class ENGINE_DLL CFontDraw final : public CComponent
{
public:
	explicit CFontDraw(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CFontDraw(const CFontDraw& rhs);
	virtual ~CFontDraw() = default;

public:
	_float Get_TextSizeX(_tchar* pText, _float fScale);

public:
	virtual HRESULT	NativeConstruct_Prototype(_tchar* pFilePath, _uint iWinCX, _uint iWinCY);
	virtual HRESULT	NativeConstruct(void* pArg) override;

	HRESULT Render_Font(_tchar* pText, _float2 vPosition, _fvector vColor = XMVectorSet(0.25, 0.f, 0.f, 1.f), _float fScale = 1.f);
	HRESULT Render_Font(_tchar* pText, _float2 vPosition, _float fAlpha = 1.f, _float fScale = 1.f);

private:
	ID3D11InputLayout*		m_pInputLayout = nullptr;
	BasicEffect*			m_pEffect = nullptr;

	SpriteBatch*			m_pFontBT;
	SpriteFont*				m_pFont;

	_float4x4				m_fProjMatrix;

	_uint					m_iFontSize = 50;
	_uint					m_iCount = 0;

private:
	_float2 Carculate_TextPosition(_tchar* pText, _float2 fPosition, _float fScale);

public:
	static CFontDraw* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _tchar* pFilePath, _uint iWinCX, _uint iWinCY);
	virtual CComponent* Clone_Component(void* _pArg);
	virtual void Free() override;
};
END