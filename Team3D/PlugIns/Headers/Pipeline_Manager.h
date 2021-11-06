#ifndef Pipeline_Manager_h__
#define Pipeline_Manager_h__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CPipeline_Manager final : public CBase
{
	DECLARE_SINGLETON(CPipeline_Manager)

public:
	enum TRANSFORM_STATETYPE { TS_VIEW, TS_PROJ, TS_REVERSE_VIEW, TS_END };

private:
	explicit CPipeline_Manager();
	virtual ~CPipeline_Manager() = default;

public: /* Getter */
	_matrix		Get_Transform	(TRANSFORM_STATETYPE eType) const;
	_fvector	Get_CamPosition	() const;
	_float		Get_CamFar		() const;

public: /* Setter */
	void	Set_Transform	(TRANSFORM_STATETYPE eType, _fmatrix matTransformMatrix);
	void	Set_CamFar		(_float fFar);

private:
	_float4x4	m_StateMatrices[TS_END];
	_float		m_fCamFar = 0.f;

public:
	virtual void	Free	() override;
};

END

#endif // Pipeline_Manager_h__