#pragma once

#include "Base.h"

BEGIN(Engine)

class CPostFX final : public CBase
{
	DECLARE_SINGLETON(CPostFX)
public:
	explicit CPostFX() = default;
	virtual ~CPostFX() = default;

public:
	HRESULT Ready_PostFX(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float fBufferWidth, _float fBufferHeight);
	HRESULT PostProcessing(_double TimeDelta);

private:
	HRESULT DownScale(_double TimeDelta);
	HRESULT Bloom();
	HRESULT Blur();
	HRESULT FinalPass();

	HRESULT Tick_Adaptation(_double TimeDelta);

private:
	HRESULT Build_LuminanceBuffer(_float iWidth, _float iHeight); /* 휘도의 중간값을 저장하기 위한 리소스들 & 부동소수점 형태로 평균 휘도 값을 저장 */
	HRESULT Build_BloomResources(_float iWidth, _float iHeight);
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

	_float	m_fMiddleGrey = 0.0025f; 
	_float	m_fLumWhiteSqr = 1.5f;
	
	_float	m_fAdaptTime = 1.f;
	_float	m_fAdaptationDeltaT = 0.f;
	_float	m_fAdaptation = 0.f;

private: /* For. CS - First Pass */
	ID3D11Buffer*				m_pHDRBuffer_Lum = nullptr;
	ID3D11UnorderedAccessView*	m_pUnorderedAccessView_Lum = nullptr; // Ouput
	ID3D11ShaderResourceView*	m_pShaderResourceView_Lum = nullptr; //Input

private: /* For. CS - Second Pass */
	ID3D11Buffer*				m_pHDRBuffer_LumAve = nullptr;
	ID3D11UnorderedAccessView*	m_pUnorderedAccessView_LumAve = nullptr;
	ID3D11ShaderResourceView*	m_pShaderResourceView_LumAve = nullptr;

private: /* For. Prev LumAve */
	ID3D11Buffer*				m_pHDRBuffer_PrevLumAve = nullptr;
	ID3D11UnorderedAccessView*	m_pUnorderedAccessView_PrevLumAve = nullptr;
	ID3D11ShaderResourceView*	m_pShaderResourceView_PrevLumAve = nullptr;

private: /* For. Bloom */
	ID3D11Texture2D*			m_pDownScaledHDRTex = nullptr; // g_HDRDownScale
	ID3D11UnorderedAccessView*	m_pUnorderedAccessView_DownScaledHDR = nullptr;
	ID3D11ShaderResourceView*	m_pShaderResourceView_DownScaledHDR = nullptr;

	ID3D11Texture2D*			m_pBloomTex_Temp = nullptr;
	ID3D11UnorderedAccessView*	m_pUnorderedAccessView_Bloom_Temp = nullptr;
	ID3D11ShaderResourceView*	m_pShaderResourceView_Bloom_Temp = nullptr;
	
	ID3D11Texture2D*			m_pBloomTex_Temp2 = nullptr;
	ID3D11UnorderedAccessView*	m_pUnorderedAccessView_Bloom_Temp2 = nullptr;
	ID3D11ShaderResourceView*	m_pShaderResourceView_Bloom_Temp2 = nullptr;

	ID3D11Texture2D*			m_pBloomTex = nullptr; // g_Bloom
	ID3D11UnorderedAccessView*	m_pUnorderedAccessView_Bloom = nullptr;
	ID3D11ShaderResourceView*	m_pShaderResourceView_Bloom = nullptr;

private: /* For.CS - Shader */
	ID3DX11Effect* m_pEffect_CS = nullptr;
	vector<INPUT_LAYOUT_DESC>	m_InputLayouts_CS;

private:
	ID3DX11Effect* m_pEffect_CS_Bloom = nullptr;
	vector<INPUT_LAYOUT_DESC>	m_InputLayouts_CS_Bloom;

private: /* For. PS - ToneMapping */
	class CVIBuffer_RectRHW* m_pVIBuffer_ToneMapping = nullptr;

public:
	void			Clear_Buffer();
	virtual void	Free() override;
};
END