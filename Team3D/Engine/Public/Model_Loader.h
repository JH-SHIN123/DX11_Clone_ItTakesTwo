#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CModel_Loader
{
private:
	explicit CModel_Loader() = default;
	virtual ~CModel_Loader() = default;

public: /* Enum */
	enum MODEL_TYPE { TYPE_NORMAL, TYPE_INSTANCE };

public:
	static HRESULT	Load_ModelFromFile(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, MODEL_TYPE eModelType, void* pModel, const _tchar* pModelFilePath, const _tchar* pModelFileName, _uint iMaterialSetCount);
	static HRESULT	Load_PathFromFile(void** ppOutput, const _tchar* pPathFilePath);

private: /* Typedef */
	typedef vector<class CMesh*>			MESHES;
	typedef vector<MATERIAL*>				MATERIALS;
	typedef	vector<class CHierarchyNode*>	NODES;
	typedef vector<BONE_DESC>				BONES;
	typedef vector<class CAnim*>			ANIMS;
	typedef vector<class CAnimChannel*>		CHANNELS;
	typedef vector<KEY_FRAME>				KEYFRAMES;
	typedef vector<_float4x4>				TRANSFORMATIONS;
};

END