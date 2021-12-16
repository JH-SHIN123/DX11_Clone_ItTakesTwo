#include "..\Public\FontDraw.h"
#include "GameInstance.h"

CFontDraw::CFontDraw(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CFontDraw::CFontDraw(const CFontDraw & rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_pFont(rhs.m_pFont)
	, m_pFontBT(rhs.m_pFontBT)
	, m_pInputLayout(rhs.m_pInputLayout)
	, m_fProjMatrix(rhs.m_fProjMatrix)
{
	Safe_AddRef(m_pInputLayout);
}

_float CFontDraw::Get_TextSizeX(_tchar * pText, _float fScale)
{
	wstring strTemp = pText;

	_uint iSize = (_uint)strTemp.size();
	_uint iCountSpecialChar = 0;
	_uint iCountOtherChar = 0;
	_tchar Space = ' ';
	_tchar Point = '.';
	_tchar Exclamation = '!';
	_tchar Question = '?';

	for (_uint i = 0; i < iSize; ++i)
	{
		if (strTemp[i] == Space || strTemp[i] == Point || strTemp[i] == Exclamation || strTemp[i] == Question)
			++iCountSpecialChar;
		else
			++iCountOtherChar;
	}

	_float fButterSizeX = (iCountOtherChar * (50.f * fScale)) + (iCountSpecialChar * (25.f * fScale));
	return fButterSizeX;
}

HRESULT CFontDraw::NativeConstruct_Prototype(_tchar* pFilePath, _uint iWinCX, _uint iWinCY)
{
	CComponent::NativeConstruct_Prototype();

	XMStoreFloat4x4(&m_fProjMatrix,XMMatrixOrthographicLH((_float)iWinCX, (_float)iWinCY, 0.f, 1.f));

	m_pEffect = new BasicEffect(m_pDevice);
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = nullptr;
	size_t		ShaderByteCodeLength = 0;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &ShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(DirectX::VertexPositionColor::InputElements, DirectX::VertexPositionColor::InputElementCount, pShaderByteCode, ShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;

	m_pFontBT = new SpriteBatch(m_pDeviceContext);
	if (nullptr == m_pFontBT)
		return E_FAIL;

	m_pFont = new SpriteFont(m_pDevice, pFilePath);
	if (nullptr == m_pFont)
		return E_FAIL;
	return S_OK;
}

HRESULT CFontDraw::NativeConstruct(void * pArg)
{
	CComponent::NativeConstruct(pArg);

	return S_OK;
}

HRESULT CFontDraw::Render_Font(_tchar * pText, _float2 vPosition, _fvector vColor, _float fScale)
{
	if (nullptr == m_pEffect || nullptr == m_pFontBT)
		return E_FAIL;

	/* 쉐이더 셋팅에 대한 작업. */
	m_pEffect->SetView(XMMatrixIdentity());
	m_pEffect->SetProjection(XMLoadFloat4x4(&m_fProjMatrix));
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	m_pEffect->Apply(m_pDeviceContext);

	m_pFontBT->Begin();
	m_pFont->DrawString(m_pFontBT, pText, Carculate_TextPosition(pText, vPosition, fScale * 2.f), vColor, 0.f, _float2(0.f, 0.f), fScale);
	m_pFontBT->End();

	return S_OK;
}

HRESULT CFontDraw::Render_Font(_tchar * pText, _float2 vPosition, _float fAlpha, _float fScale)
{
	if (nullptr == m_pEffect || nullptr == m_pFontBT)
		return E_FAIL;

	/* 쉐이더 셋팅에 대한 작업. */
	m_pEffect->SetView(XMMatrixIdentity());
	m_pEffect->SetProjection(XMLoadFloat4x4(&m_fProjMatrix));
	m_pDeviceContext->IASetInputLayout(m_pInputLayout); 
	m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	m_pEffect->Apply(m_pDeviceContext);

	m_pFontBT->Begin();
	m_pFont->DrawString(m_pFontBT, pText, Carculate_TextPosition(pText, vPosition, fScale), XMVectorSet(0.25f, 0.f, 0.f, fAlpha), 0.f, _float2(0.f, 0.f), fScale);
	m_pFontBT->End();

	return S_OK;
}

_float2 CFontDraw::Carculate_TextPosition(_tchar* pText, _float2 fPosition, _float fScale)
{
	wstring strTemp = pText;

	_uint iSize = (_uint)strTemp.size();
	_uint iCountSpecialChar = 0;
	_uint iCountOtherChar = 0;

	_tchar Space = ' ';
	_tchar Point = '.';
	_tchar Exclamation = '!';
	_tchar Question = '?';

	for (_uint i = 0; i < iSize; ++i)
	{
		if (strTemp[i] == Point)
			++iCountSpecialChar;
		else
			++iCountOtherChar;
	}

	_float fButterSizeX;
	if(1 < iCountSpecialChar)
		fButterSizeX = (iCountOtherChar * (50.f * fScale)) + (iCountSpecialChar * (25.f * fScale));
	else
		fButterSizeX = (iCountOtherChar * (50.f * fScale)) + (iCountSpecialChar * (50.f * fScale));

	_float fButterSizeY = 50.f * fScale;

	_float2 fTextPos;
	fTextPos.x = fPosition.x - (fButterSizeX * 0.5f);
	fTextPos.y = fPosition.y - (fButterSizeY * 0.5f);

	return fTextPos;
}

CFontDraw * CFontDraw::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _tchar* pFilePath, _uint iWinCX, _uint iWinCY)
{
	CFontDraw* pInstance = new CFontDraw(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pFilePath, iWinCX, iWinCY)))
	{
		MSG_BOX("Failed to Create Instance - CFontDraw");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CFontDraw::Clone_Component(void * _pArg)
{
	CFontDraw* pInstance = new CFontDraw(*this);

	if (FAILED(pInstance->NativeConstruct(_pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CFontDraw");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFontDraw::Free()
{
	if (false == m_isClone)
	{
		Safe_Delete(m_pEffect);
		Safe_Delete(m_pFontBT);
		Safe_Delete(m_pFont);
	}
	Safe_Release(m_pInputLayout);

	CComponent::Free();
}
