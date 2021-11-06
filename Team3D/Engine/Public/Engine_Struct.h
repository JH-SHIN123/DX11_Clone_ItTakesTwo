#pragma once

namespace Engine
{
	typedef struct tagLightDesc
	{
		enum LIGHTTYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		LIGHTTYPE	eType;
		XMFLOAT3	vDirection;
		XMFLOAT3	vPosition;
		float		fRadius;
		float		fAtt;
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

	typedef struct tagBoneDesc 
	{
		class CHierarchyNode*	pHierarchyNode;
		XMFLOAT4X4				OffsetMatrix;
	}BONE_DESC;

	typedef struct tagKeyFrame
	{
		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vPosition;
		double		dTime;
	}KEY_FRAME;
}