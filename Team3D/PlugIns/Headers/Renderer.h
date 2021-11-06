#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
private:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRenderer(const CRenderer& rhs);
	virtual ~CRenderer() = default;

public: /* Enum */
	enum RENDER_GROUP { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END };

public:
	virtual HRESULT	NativeConstruct_Prototype();
	virtual HRESULT	NativeConstruct(void* pArg);
	HRESULT			Add_GameObject_ToRenderGroup(RENDER_GROUP eGroup, class CGameObject* pGameObject);
	HRESULT			Draw_Renderer();

private: /* Typedef */
	typedef list<class CGameObject*> RENDER_OBJECTS;
private:
	RENDER_OBJECTS					m_RenderObjects[RENDER_END];
	class CRenderTarget_Manager*	m_pRenderTarget_Manager = nullptr;
	class CVIBuffer_RectRHW*		m_pVIBuffer = nullptr;
	class CVIBuffer_RectRHW*		m_pDOFBuffer = nullptr;


private:
	HRESULT	Render_Priority();
	HRESULT	Render_NonAlpha();
	HRESULT	Render_Alpha();
	HRESULT	Render_UI();
	HRESULT Render_LightAcc();
	HRESULT Render_Blend();

	//Post Processing
	HRESULT Render_DOF();

	void	Sort_GameObjects(RENDER_OBJECTS& GameObjects);

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent*	Clone_Component(void* pArg) override;
	virtual void Free() override;
};

END