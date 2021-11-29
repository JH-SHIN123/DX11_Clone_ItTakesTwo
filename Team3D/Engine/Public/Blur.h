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
	ID3D11ShaderResourceView* Get_ShaderResourceView_BlurEmissive() const { return m_pShaderResourceView_DownScaledEmissive; };
	ID3D11ShaderResourceView* Get_ShaderResourceView_BlurEffect() const { return m_pShaderResourceView_DownScaledEffect; };

public:
	HRESULT Ready_Blur(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float fBufferWidth, _float fBufferHeight);
	HRESULT Blur_Emissive();
	HRESULT Blur_Effect();

private:
	HRESULT DownScale(ID3D11ShaderResourceView* inputSRV, ID3D11UnorderedAccessView* inputUAV);
	HRESULT BlurInPlace(ID3D11ShaderResourceView* inputSRV, ID3D11UnorderedAccessView* inputUAV);

private:
	HRESULT Build_BlurResources(_float fBufferWidth, _float fBufferHeight);
	HRESULT Build_ComputeShaders(const _tchar* pShaderFilePath, const char* pTechniqueName);

	HRESULT	Set_Variable(const char* pConstantName, void* pData, _uint iByteSize);
	HRESULT	Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pResourceView);
	HRESULT	Set_UnorderedAccessView(const char* pConstantName, ID3D11UnorderedAccessView* pResourceView);

	HRESULT Unbind_ShaderResources();

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

private:
	_uint	m_iWinSize[2] = { 0,0 };
	_uint	m_iDownScaleGroups = 0;

private: /* For.CS - Shader */
	ID3DX11Effect*				m_pEffect_CS = nullptr;
	vector<INPUT_LAYOUT_DESC>	m_InputLayouts_CS;

private: /* For. Blur */
	ID3D11Texture2D* m_pBloomTex_Temp = nullptr;
	ID3D11UnorderedAccessView* m_pUnorderedAccessView_Bloom_Temp = nullptr;
	ID3D11ShaderResourceView* m_pShaderResourceView_Bloom_Temp = nullptr;

private: /* For. Emissive */
	ID3D11Texture2D*			m_pDownScaledEmissiveTex = nullptr; // g_HDRDownScale
	ID3D11UnorderedAccessView*	m_pUnorderedAccessView_DownScaledEmissive = nullptr;
	ID3D11ShaderResourceView*	m_pShaderResourceView_DownScaledEmissive = nullptr;

private: /* For. Effect */
	ID3D11Texture2D* m_pDownScaledEffectTex = nullptr; // g_HDRDownScale
	ID3D11UnorderedAccessView* m_pUnorderedAccessView_DownScaledEffect = nullptr;
	ID3D11ShaderResourceView* m_pShaderResourceView_DownScaledEffect = nullptr;

public:
	virtual void	Free() override;
};
END