#ifndef Navigation_h__
#define Navigation_h__

#include "Cell.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CBase
{
private:
	explicit CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CNavigation() = default;

public: /* Setter */
	void	Set_CurrentIndex	(_uint iIndex);
	void	Set_GateIndex		(_uint iDirection, _uint iIndex);
	void	Set_Index_EnterGate	(_byte byDirection);
	_uint	Get_CurrentIndex	() const;

public:
	HRESULT NativeConstruct		(const _tchar* pNavigationFilePath);
	_bool	Is_Move				(_fvector vOldPos, _fvector vNewPos, _uint& iOutCellIndex, _float& fOutDist, _vector& vOutSlide, _uint iFlag);

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr; 
	vector<CCell*>				m_Cells;
	_uint						m_iCurrentIndex		= 0;
	_int						m_iGateIndex_Top	= -1;
	_int						m_iGateIndex_Bottom = -1;
	_int						m_iGateIndex_Left	= -1;
	_int						m_iGateIndex_Right	= -1;

public:
	static CNavigation* Create	(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pNavigationFilePath);
	virtual void		Free	() override;

#ifdef _DEBUG
public:
	HRESULT Render();
#endif
};

END

#endif // Navigation_h__