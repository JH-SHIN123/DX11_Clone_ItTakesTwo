#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
class CPerformer : public CGameObject
{
public:
	typedef struct tagPerformerDesc
	{
		_float3 vPosition = { 0.f,0.f,0.f };
		_float3 vScale = { 1.f,1.f,1.f };
		_float3 vRot = { 0.f,0.f,0.f };
		wstring strModelTag = L"";
	}PERFORMERDESC;
protected:
	explicit CPerformer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPerformer(const CPerformer& rhs);
	virtual ~CPerformer() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	
public:
	void Set_PerformerDesc(PERFORMERDESC tDesc) { m_tDesc = tDesc; }
	void Start_Perform();
	void Finish_Perform();
public:
	CTransform* Get_Transform() { return m_pTransformCom; }
	CModel* Get_Model() { return m_pModelCom; }
	PERFORMERDESC& Get_PerformerDesc() { return m_tDesc; }

	PERFORMERDESC		m_tDesc;
	_bool				m_bStartAnim = false;

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;

public:
	static CPerformer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END