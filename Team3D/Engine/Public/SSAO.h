#pragma once


#include "Base.h"

BEGIN(Engine)
class CSSAO final : public CBase
{
	DECLARE_SINGLETON(CSSAO)

public:
	explicit CSSAO() = default;
	virtual ~CSSAO() = default;

public:
	ID3D11ShaderResourceView* Get_ShaderResourceView() const { return m_pSRV_SSAO; }

public:
	HRESULT Ready_SSAO(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float fBufferWidth, _float fBufferHeight);
	HRESULT Compute_SSAO();

private:
	HRESULT DownScaleDepth();
	HRESULT SSAO();

private:
	HRESULT Build_DownScaledDepthBuffer();
	HRESULT Build_SSAOResources();
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
	_uint	m_iDownScaledWidth = 0;
	_uint	m_iDownScaledHeight = 0;

private: /* For.CS - Shader */
	ID3DX11Effect*				m_pEffect_CS = nullptr;
	vector<INPUT_LAYOUT_DESC>	m_InputLayouts_CS;

private: /* For. DownScaled_Depth - 1/4 Size */
	ID3D11Buffer* m_pDownScaledDepthBuffer = nullptr; 
	ID3D11UnorderedAccessView* m_pUAV_DownScaledDepth = nullptr;
	ID3D11ShaderResourceView* m_pSRV_DownScaledDepth = nullptr;

private: /* For. SSAO values for usage with the directional light */
	ID3D11Texture2D* m_pSSAOTex = nullptr;
	ID3D11UnorderedAccessView* m_pUAV_SSAO = nullptr;
	ID3D11ShaderResourceView* m_pSRV_SSAO = nullptr;

public:
	virtual void	Free() override;

#ifdef _DEBUG
public:
	HRESULT Ready_DebugBuffer(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_DebugBuffer();
	void	Clear_Buffer();
private:
	class CVIBuffer_RectRHW* m_pVIBuffer = nullptr;
#endif
};
END
