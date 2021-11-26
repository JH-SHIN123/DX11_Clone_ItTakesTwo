#pragma once

#include "Base.h"

BEGIN(Engine)

class CBlur final : public CBase
{
	DECLARE_SINGLETON(CBlur)

public:
	explicit CBlur() = default;
	virtual ~CBlur() = default;

public:
	HRESULT Ready_Blur(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float fBufferWidth, _float fBufferHeight);
	HRESULT BlurInPlace(ID3D11ShaderResourceView* inputSRV, ID3D11UnorderedAccessView* inputUAV);

private: /* For.CS - Shader */
	ID3DX11Effect*				m_pEffect_CS = nullptr;
	vector<INPUT_LAYOUT_DESC>	m_InputLayouts_CS;

private: /* For. Blur */
	ID3D11Texture2D*			m_pDownScaledHDRTex = nullptr; // g_HDRDownScale
	ID3D11UnorderedAccessView*	m_pUnorderedAccessView_DownScaledHDR = nullptr;
	ID3D11ShaderResourceView*	m_pShaderResourceView_DownScaledHDR = nullptr;

	ID3D11Texture2D* m_pBloomTex_Temp = nullptr;
	ID3D11UnorderedAccessView* m_pUnorderedAccessView_Bloom_Temp = nullptr;
	ID3D11ShaderResourceView* m_pShaderResourceView_Bloom_Temp = nullptr;

	ID3D11Texture2D* m_pBloomTex_Temp2 = nullptr;
	ID3D11UnorderedAccessView* m_pUnorderedAccessView_Bloom_Temp2 = nullptr;
	ID3D11ShaderResourceView* m_pShaderResourceView_Bloom_Temp2 = nullptr;
	
public:
	void			Clear_Buffer();
	virtual void	Free() override;
};
END