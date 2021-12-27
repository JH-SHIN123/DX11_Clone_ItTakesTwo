#pragma once
#include "InGameEffect.h"

BEGIN(Client)
class CEffect_CS_CodySize final : public CInGameEffect
{
public:
	enum SIZE_TYPE
	{
		TYPE_MIDDLE_LARGE,
		TYPE_LARGE_SMALL,
		TYPE_SMALL_LARGE,
		TYPE_SMALL_MIDDLE,
		TYPE_END
	};

private:
	explicit CEffect_CS_CodySize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_CS_CodySize(const CEffect_CS_CodySize& rhs);
	virtual ~CEffect_CS_CodySize() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	void Check_Size();
	void Set_Model();				// Initialize
	void Update_Matrix();

protected:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;

private:
	HRESULT Ready_Instance();
	void Check_Size(_double TimeDelta);
	void Resizing_LS(_double TimeDelta); // 
	void Resizing_SL(_double TimeDelta); 
	void Resizing_SM(_double TimeDelta); //
	void Resizing_ML(_double TimeDelta); //
	void Resizing_LS_End(_double TimeDelta);
	void Resizing_SL_End(_double TimeDelta);
	void Resizing_SM_End(_double TimeDelta);
	void Resizing_ML_End(_double TimeDelta);

private:
	_int Get_InstanceCount();
	void Reset_Instance();


private:
	SIZE_TYPE m_eChangeSize_Now = TYPE_END;

private:
	const _int	m_iInstance_Small = 550;
	const _int	m_iInstance_Middle = 1200;
	const _int	m_iInstance_Large = 3000;

	_double		m_dReSizeTime;
	_double		m_dParabolaTime;
	_bool		m_IsReSizing = false;
	_bool		m_IsNextSizing = false;
	_bool		m_IsSetDir = false;

private:
	CModel*		m_pTargetModel = nullptr;
	_float4*	m_pInstance_LocalPos = nullptr;
	_float4*	m_pInstance_LocalPos_Origin = nullptr;
	_float*		m_pParabola_WorldPos_Y = nullptr;
	_float*		m_pInstance_SizeType = nullptr;

private:
	//CTextures* m_pTexturesCom_Circle = nullptr;
	//CVIBuffer_Rect*

public:
	static CEffect_CS_CodySize* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END