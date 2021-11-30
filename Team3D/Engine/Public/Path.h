#pragma once

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CPath final : public CComponent
{
private:
	explicit CPath(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPath(const CPath& rhs);
	virtual ~CPath() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(const _tchar * pFilePath, const _tchar * pPathTag);
	virtual HRESULT	NativeConstruct(void* pArg) override;

public:
	HRESULT	Update_Animation(_double dTimeDelta, _matrix& WorldMatrix);

private:
	_float3 Get_RotationAngles(const _float4x4& fRotateMatrix);

private:
	class CModel_Loader*	m_pModel_Loader = nullptr;
	class CAnim*			m_pPathAnim = nullptr;
	_tchar					m_szPathTag[MAX_PATH] = L"";

private: /* Typedef */
	typedef vector<_float4x4>				TRANSFORMATIONS;

private:
	_double					m_dCurrentTime = 0.0;		// 현재 애니메이션 진행시간
	_uint					m_iCurAnimFrame = 0;		// 현재 애니메이션 프레임
	_float					m_fProgressAnim = 0.f;		// 애니메이션 진행도
	TRANSFORMATIONS			m_AnimTransformations;		// 채널의 Transformation이 채워진 TransformationMatrix

public:
	static CPath* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pFilePath, const _tchar* pPathTag);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};
END