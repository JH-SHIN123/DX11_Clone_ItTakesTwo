#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
private:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRenderer(const CRenderer& rhs);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype();
	virtual HRESULT	NativeConstruct(void* pArg);

	HRESULT			Add_GameObject_ToRenderGroup(RENDER_GROUP::Enum eGroup, class CGameObject* pGameObject);
	HRESULT			Draw_Renderer(_double TimeDelta);

private: /* Typedef */
	typedef list<class CGameObject*> RENDER_OBJECTS;
private:
	RENDER_OBJECTS					m_RenderObjects[RENDER_GROUP::RENDER_END];
	class CRenderTarget_Manager*	m_pRenderTarget_Manager = nullptr;
	class CVIBuffer_RectRHW*		m_pVIBuffer = nullptr;

private:
	HRESULT	Render_Priority();
	HRESULT	Render_NonAlpha();
	HRESULT	Render_Alpha();
	HRESULT	Render_Effect();
	HRESULT	Render_Effect_No_Blur();
	HRESULT	Render_UI();

private:
	HRESULT Render_ShadowsForAllCascades();
	HRESULT Render_Volume();
	HRESULT Render_LightAcc();
	HRESULT Render_Blend();

private:
	HRESULT PostProcessing(_double TimeDelta);
	HRESULT Compute_SSAO();

private:
	void	Sort_GameObjects(RENDER_OBJECTS& GameObjects);

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent*	Clone_Component(void* pArg) override;
	virtual void Free() override;

#ifdef _DEBUG
private:
	_bool m_bShowDebugBuffer = false;
#endif
};

END