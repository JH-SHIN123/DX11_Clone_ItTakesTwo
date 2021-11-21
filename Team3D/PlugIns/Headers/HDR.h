#pragma once

#include "Base.h"

BEGIN(Engine)

class CHDR final : public CBase
{
	DECLARE_SINGLETON(CHDR)
public:
	explicit CHDR() = default;
	virtual ~CHDR() = default;

public:
	HRESULT Ready_HDR(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float fBufferWidth, _float fBufferHeight);
	HRESULT Render_HDR();

private:
	HRESULT Build_FirstPassResources(_float iWidth, _float iHeight); /* 휘도의 중간값을 저장하기 위한 리소스들 */
	HRESULT Build_SecondPassResources(); /* 부동소수점 형태로 평균 휘도 값을 저장 */
	HRESULT Build_PrevLumAvgResources(); /* 부동소수점 형태로 평균 휘도 값을 저장 */
	HRESULT Build_ComputeShaders(const _tchar* pShaderFilePath, const char* pTechniqueName);

	HRESULT	Set_Variable(const char* pConstantName, void* pData, _uint iByteSize);
	HRESULT	Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pResourceView);
	HRESULT	Set_UnorderedAccessView(const char* pConstantName, ID3D11UnorderedAccessView* pResourceView);

	HRESULT	Calculate_LuminanceAvg();

	HRESULT Unbind_ShaderResources();

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
	
private:
	_float	m_fMiddleGrey = 0.0008f;
	_float	m_fLumWhiteSqr = 5.9f;
	
	_float m_fAdaptationDeltaT = 0.f;
	_float m_fAdaptation = 0.f;

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

private: /* For.CS - Shader */
	ID3DX11Effect* m_pEffect_CS = nullptr;
	vector<INPUT_LAYOUT_DESC>	m_InputLayouts_CS;

private: /* For. PS - ToneMapping */
	class CVIBuffer_RectRHW* m_pVIBuffer_ToneMapping = nullptr;

public:
	void			Clear_Buffer();
	virtual void	Free() override;
};
END