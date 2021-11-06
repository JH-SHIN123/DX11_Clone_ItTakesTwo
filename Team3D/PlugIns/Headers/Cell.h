#ifndef Cell_h__
#define Cell_h__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCell final : public CBase
{
public:
	explicit CCell();
	virtual ~CCell() = default;

public:
	enum POINT	{ POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE	{ LINE_AB, LINE_BC, LINE_CA, LINE_END };

public: /* Getter */
	 const _uint	Get_CellType() const;

public:
	HRESULT	NativeConstruct		(_uint iType, _uint iIndex, _float3* vPoints, _int* iNeighbors);
	_int	Move_InCell			(vector<CCell*>& Cells, _fvector vOldPos, _fvector vNewPos, _float& fOutDist, _vector& vOutSlide, _uint iFlag);

private:
	_uint	m_iType		= 0;
	_uint	m_iIndex	= 0;
	_int	m_iNeighbors[LINE_END];
	_float3	m_vPoints[POINT_END];
	_float3	m_vLines[LINE_END];

public:
	static CCell*	Create	(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iType, _uint iIndex, _float3* vPoints, _int* iNeighbors);
	virtual void	Free	() override;

#ifdef _DEBUG
public:
	explicit CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
public:
	HRESULT	Render_Cell(_uint iCurrentCell);
private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;
	ID3D11Buffer*				m_pVB = nullptr;
	D3D11_BUFFER_DESC			m_VBDesc;
	D3D11_SUBRESOURCE_DATA		m_VBSubresourceData;
	_uint						m_iVertexCount = 0;
	_uint						m_iVertexStride = 0;
	_uint						m_iVertexBufferCount = 0;
	void*						m_pVertices = nullptr;
	ID3DX11Effect*				m_pEffect = nullptr;
	vector<INPUT_LAYOUT_DESC>	m_InputLayouts;
private:
	HRESULT Ready_VertexBuffer();
#endif
};

END

#endif // Cell_h__