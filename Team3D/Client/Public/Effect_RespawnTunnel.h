#pragma once

#ifndef __EFFECT_RESPAWNTUNNEL_H__

#include "InGameEffect_Model.h"
#include "WarpGate.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)
class CEffect_RespawnTunnel final : public CInGameEffect_Model
{
private:
	explicit CEffect_RespawnTunnel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_RespawnTunnel(const CEffect_RespawnTunnel& rhs);
	virtual ~CEffect_RespawnTunnel() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual void SetUp_WorldMatrix(_fmatrix WorldMatrix) override;

public:
	_fmatrix Calcul_WorldMatrix(_float4 vLocalPos);
	void	 Set_Stage_Viewer(CWarpGate::STAGE_VALUE eValue);

private:
	CTextures*	m_pTexturesCom_Distortion		= nullptr;
	CTextures*	m_pTexturesCom_ColorRamp		= nullptr;
	_float		m_fTime_UV = 0.f;
	_int		m_iStageViewer = 0;

private:
	_float4x4 m_MatrixMask;

private: //비쳐보이는 스테이지
	CVIBuffer_Rect*		m_pBufferRectCom_Preview	= nullptr;
	CTextures*			m_pTexturesCom_Preview		= nullptr;
	CTextures*			m_pTexturesCom_Masking		= nullptr;
	CTextures*			m_pTexturesCom_Distortion_2 = nullptr;

	_float4x4			m_LocalMatrix_Preview		= MH_XMFloat4x4Identity();
	_float m_fRadianAngle = 0.f;
public:
	static CEffect_RespawnTunnel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __EFFECT_RESPAWNTUNNEL_H__
#endif // !__EFFECT_RESPAWNTUNNEL_H__
