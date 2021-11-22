#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CTextures;
END

BEGIN(Client)

class CRespawnCircle final : public CUIObject
{
private:
	explicit CRespawnCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRespawnCircle(const CUIObject& rhs);
	virtual ~CRespawnCircle() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

private:
	_float							m_fTime = 0.5f;
	_uint							m_iPassNum = 0;
	_float2							m_vStartScale;
	_bool							m_IsSpawnEnd = false;
	_int							m_iRespawnOption = 0;
	_float							m_fColorChangeTime = 0.f;
	_float2							m_vUV;
	_float							m_fSubTime = 0.f;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;
	CTextures*						m_pSubTexturesCom = nullptr;
	CTextures*						m_pNoiseTextureCom = nullptr;

private:
	HRESULT Ready_Component();
	void Set_Gauge(_double TimeDelta);
	void Spawn_Effect(_double TimeDelta);

public:
	static CRespawnCircle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END