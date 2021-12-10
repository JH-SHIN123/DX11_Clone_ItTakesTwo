#pragma once

#include "Base.h"

BEGIN(Engine)

class CShaderCompiler final : public CBase
{
	DECLARE_SINGLETON(CShaderCompiler)
private:
	explicit CShaderCompiler() = default;
	virtual ~CShaderCompiler() = default;

public:
	ID3DBlob* Get_CompiledCode(const _tchar* pShaderFilePath);

private:
	unordered_map<const _tchar*, ID3DBlob*> m_CompiledCodes;

public:
	virtual void Free() override;
};

END