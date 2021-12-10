#include "..\Public\ShaderCompiler.h"

IMPLEMENT_SINGLETON(CShaderCompiler)

ID3DBlob * CShaderCompiler::Get_CompiledCode(const _tchar * pShaderFilePath)
{
	if (0 < m_CompiledCodes.size())
	{
		auto iter = find_if(m_CompiledCodes.begin(), m_CompiledCodes.end(), CTagFinder(pShaderFilePath));

		if (iter != m_CompiledCodes.end())
			return iter->second;
	}

	_uint		iFlag = 0;
	ID3DBlob*	pCompiledShaderCode = nullptr;
	ID3DBlob*	pCompileErrorMsg = nullptr;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	FAILED_CHECK_RETURN(D3DCompileFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, nullptr, "fx_5_0", iFlag, 0, &pCompiledShaderCode, &pCompileErrorMsg), nullptr);

	m_CompiledCodes.emplace(pShaderFilePath, pCompiledShaderCode);

	Safe_Release(pCompileErrorMsg);

	return pCompiledShaderCode;
}

void CShaderCompiler::Free()
{
	for (auto& Pair : m_CompiledCodes)
		Safe_Release(Pair.second);
}