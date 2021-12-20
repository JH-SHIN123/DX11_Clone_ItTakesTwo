#pragma once

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CPath final : public CComponent
{
public:
	enum CHANNELID	{ CHANNEL_TRANSLATION, CHANNEL_ROTATION, CHANNEL_SCALING };
	enum STATE		{ STATE_FORWARD, STATE_BACKWARD, STATE_END };

	typedef struct tagPathDesc 
	{
		_float4x4	WorldMatrix = MH_XMFloat4x4Identity();
		_float		fPivotScale = 0.001f;
	}PATH_DESC;

private:
	explicit CPath(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPath(const CPath& rhs);
	virtual ~CPath() = default;

public:
	_float	Get_ProgressAnim() const { return m_fProgressAnim; }
	void	Get_FramesWorldMatrices(vector<_uint>& OutFrameIndices, vector<_float4x4>& OutMatrices, _uint iPerNodeInteract);

public:
	virtual HRESULT	NativeConstruct_Prototype(const _tchar * pFilePath, const _tchar * pPathTag, _fmatrix PivotMatrix);
	virtual HRESULT	NativeConstruct(void* pArg) override;

public:
	HRESULT Start_Path(STATE eState, _uint iAnimFrame, _bool bStop);
	_bool	Update_Animation(_double dTimeDelta, _matrix& WorldMatrix);

private:
	_bool		Update_AnimTransformations();
	_fmatrix	Update_CombinedTransformations();

private: /* Typedef */
	typedef vector<_float4x4>				TRANSFORMATIONS;

private: /* Prototype Info */
	class CAnim*			m_pPathAnim = nullptr;
	_tchar					m_szPathTag[MAX_PATH] = L"";
	_double					m_dDurationTime = 0.0;
	_float4x4				m_PivotMatrix;

private: /* Clone Info */
	_bool					m_bPlayAnimation = false;
	STATE					m_eState = STATE_END;
	PATH_DESC				m_tDesc;

private:
	_float					m_fLerpRatio = 0.f;
	_double					m_dCurrentTime = 0.0;		// 현재 애니메이션 진행시간
	_uint					m_iCurAnimFrame = 0;		// 현재 애니메이션 프레임
	_float					m_fProgressAnim = 0.f;		// 애니메이션 진행도
	TRANSFORMATIONS			m_CurAnimTransformations;		// Channel : Scaling / Rotation / Position
	TRANSFORMATIONS			m_NextAnimTransformations;		// Channel : Scaling / Rotation / Position

public:
	static CPath* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pFilePath, const _tchar* pPathTag, _fmatrix PivotMatrix);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};
END