#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CChapterImage : public CGameObject
{
protected:
	explicit CChapterImage(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CChapterImage(const CChapterImage& rhs);
	virtual ~CChapterImage() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;

	CTextures*			m_pTextureCom = nullptr;
	CVIBuffer_Rect*		m_pVIBuffer_RectCom = nullptr;

private:
	HRESULT Ready_Component();
	HRESULT Set_UIVariables_Perspective();

public:
	static CChapterImage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END