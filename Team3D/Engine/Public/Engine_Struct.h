#pragma once

namespace Engine
{
	typedef struct tagLightDesc
	{
		enum LIGHTTYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOT, TYPE_END };

		LIGHTTYPE	eType;
		XMFLOAT3	vDirection;
		XMFLOAT3	vPosition;
		float		fRange;
		float		fAtt;
		float		fSpotCosOuterCone;
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;
	}LIGHT_DESC;

	typedef struct tagInputLayoutDesc
	{
		ID3D11InputLayout* pLayout;
		ID3DX11EffectPass* pPass;
	}INPUT_LAYOUT_DESC;

	typedef struct tagPolygonIndices16
	{
		unsigned short	_0, _1, _2;
	}POLYGON_INDICES16;

	typedef struct tagPolygonIndices32
	{
		unsigned long	_0, _1, _2;
	}POLYGON_INDICES32;

	typedef struct tagVertexPosition
	{
		XMFLOAT3	vPosition;
	}VTXPOS;

	typedef struct tagVertexTexture
	{
		XMFLOAT3	vPosition;
		XMFLOAT2	vTexUV;
	}VTXTEX;

	typedef struct tagVertexTexture_Triple
	{
		XMFLOAT3	vPosition;
		XMFLOAT2	vTexUV;
		XMFLOAT2	vTexUV_2;
		XMFLOAT2	vTexUV_3;
	}VTXTEX_TRIPLE;

	typedef struct tagVertexColor
	{
		XMFLOAT3	vPosition;
		XMFLOAT4	vColor;
	}VTXCOL;

	typedef struct tagVertexNormalTexture
	{
		XMFLOAT3	vPosition;
		XMFLOAT3	vNormal;
		XMFLOAT2	vTexUV;
	}VTXNORTEX;

	typedef struct tagVertexMatrix_Custom_SizeTexUV
	{
		XMFLOAT4	vRight;
		XMFLOAT4	vUp;
		XMFLOAT4	vLook;
		XMFLOAT4	vPosition;

		XMFLOAT2	vSize;
		XMFLOAT4	vTextureUV;
	}VTXMATRIX_CUSTOM_ST;

	typedef struct tagVertexMatrix_Custom_SizeTexUVTime
	{
		XMFLOAT4	vRight;
		XMFLOAT4	vUp;
		XMFLOAT4	vLook;
		XMFLOAT4	vPosition;

		XMFLOAT2	vSize;
		XMFLOAT4	vTextureUV;
		float		fTime;
	}VTXMATRIX_CUSTOM_STT;

	typedef struct tagVertexFont
	{
		XMFLOAT3	vPosition;
		XMFLOAT2	vScale;
		XMFLOAT4	vTexUV;
	}VTXFONT;

	typedef struct tagVertexMesh
	{
		XMFLOAT3	vPosition;
		XMFLOAT3	vNormal;
		XMFLOAT3	vTangent;
		XMFLOAT2	vTexUV;
		XMUINT4		vBlendIndex;
		XMFLOAT4	vBlendWeight;
	}VTXMESH;

	typedef struct tagVertexTextureRHW
	{
		XMFLOAT4	vPosition;
		XMFLOAT2	vTexUV;
	}VTXTEXRHW;

	typedef struct tagVertexMatrix
	{
		XMFLOAT4	vRight;
		XMFLOAT4	vUp;
		XMFLOAT4	vLook;
		XMFLOAT4	vPosition;
	}VTXMATRIX;

	typedef struct tagVertexPoint
	{
		XMFLOAT3	vPosition;
		XMFLOAT2	vSize;
	}VTXPOINT;

	typedef struct tagMaterial
	{
		class CTextures* pMaterialTexture[AI_TEXTURE_TYPE_MAX];
	}MATERIAL;

	typedef struct tagTextureName
	{
		bool		isNotEmpty;
		wchar_t		szMaterialTextureName[MAX_PATH];
	}MATERIAL_TEX;

	typedef struct tagMaterialNames
	{
		MATERIAL_TEX MaterialTex[AI_TEXTURE_TYPE_MAX];
	}MATERIAL_NAMES;

	typedef struct tagMaterialObject
	{
		/* Material Ratio 전용으로 사용될 예정 */
		XMFLOAT4 vDiffuse = {1.f,1.f,1.f,1.f};
		XMFLOAT4 vAmbient = { 1.f,1.f,1.f,1.f };
		XMFLOAT4 vSpecular = { 1.f,1.f,1.f,1.f }; // w = SpecPower
		XMFLOAT4 vReflect = { 1.f,1.f,1.f,1.f };
	}MATERIAL_OBJECT;

	typedef struct tagBoneDesc 
	{
		XMFLOAT4X4	OffsetMatrix;
		int			iNodeIndex;
	}BONE_DESC;

	typedef struct tagKeyFrame
	{
		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vPosition;
		double		dTime;
		bool		bActive;
	}KEY_FRAME;

	typedef struct tagMeshActorDesc
	{
		void*		pVertices;
		void*		pFaces;
		uint32_t	iVertexCount;
		uint32_t	iFaceCount;

		tagMeshActorDesc() { }
		tagMeshActorDesc(uint32_t _iVertexCount, void* _pVertices, uint32_t _iFaceCount, void* _pFaces) : iVertexCount(_iVertexCount), pVertices(_pVertices), iFaceCount(_iFaceCount), pFaces(_pFaces){}
	}MESHACTOR_DESC;

	typedef struct tagRenderGroup
	{
		enum Enum { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END };
	}RENDER_GROUP;
}