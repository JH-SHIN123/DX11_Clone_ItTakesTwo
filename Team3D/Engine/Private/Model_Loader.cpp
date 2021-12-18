#include "..\public\Model_Loader.h"
#include "Model.h"
#include "Model_Instance.h"
#include "Path.h"
#include "Mesh.h"
#include "Textures.h"
#include "HierarchyNode.h"
#include "Anim.h"
#include "AnimChannel.h"
#include <io.h>

HRESULT CModel_Loader::Load_ModelFromFile(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, MODEL_TYPE eModelType, void * pModel, const _tchar * pModelFilePath, const _tchar * pModelFileName, _uint iMaterialSetCount)
{
	_tchar szBasePath[MAX_PATH] = TEXT("");
	_tchar szModelPath[MAX_PATH] = TEXT("");

	lstrcpy(szBasePath, pModelFilePath);
	lstrcat(szBasePath, pModelFileName);
	lstrcat(szBasePath, TEXT("/"));

	lstrcpy(szModelPath, szBasePath);
	lstrcat(szModelPath, pModelFileName);
	lstrcat(szModelPath, TEXT(".Model"));

	/* Create_Container & Reserve */
	MESHES				Meshes;
	MATERIALS			Materials;
	NODES				Nodes;
	TRANSFORMATIONS		Transformations;
	ANIMS				Anims;

	_uint	iVertexCount = 0;
	_uint	iFaceCount = 0;
	_uint	iCount = 0;

	HANDLE hFile = CreateFile(szModelPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile) return E_FAIL;

	_ulong dwByte = 0;

	/* Load_Vetices */
	ReadFile(hFile, &iVertexCount, sizeof(_uint), &dwByte, nullptr);
	VTXMESH* pVertices = new VTXMESH[iVertexCount];
	ReadFile(hFile, pVertices, sizeof(VTXMESH) * iVertexCount, &dwByte, nullptr);

	/* Load_Faces */
	ReadFile(hFile, &iFaceCount, sizeof(_uint), &dwByte, nullptr);
	POLYGON_INDICES32* pFaces = new POLYGON_INDICES32[iFaceCount];
	ReadFile(hFile, pFaces, sizeof(POLYGON_INDICES32) * iFaceCount, &dwByte, nullptr);

	/* Load_Mashes&Bones */
	ReadFile(hFile, &iCount, sizeof(_uint), &dwByte, nullptr);
	Meshes.reserve(iCount);
	for (_uint iIndex = 0; iIndex < iCount; ++iIndex)
	{
		CMesh::MESH_DESC MeshDesc;
		ReadFile(hFile, &MeshDesc, sizeof(CMesh::MESH_DESC), &dwByte, nullptr);
		CMesh* pMesh = CMesh::Create(MeshDesc);
		Meshes.emplace_back(pMesh);

		if (MeshDesc.iBoneCount > 0)
		{
			BONES Bones;
			Bones.resize(MeshDesc.iBoneCount);

			ReadFile(hFile, &Bones[0], sizeof(BONE_DESC) * MeshDesc.iBoneCount, &dwByte, nullptr);
			pMesh->Bring_BoneContainer(Bones);
		}
	}

	/* Load_Material */
	ReadFile(hFile, &iCount, sizeof(_uint), &dwByte, nullptr);
	Materials.reserve(iCount);
	for (_uint iMaterialCount = 0; iMaterialCount < iCount; ++iMaterialCount)
	{
		MATERIAL* pMaterial = new MATERIAL;
		ZeroMemory(pMaterial, sizeof(MATERIAL));

		for (_uint iTextureType = 0; iTextureType < AI_TEXTURE_TYPE_MAX; ++iTextureType)
		{
			_bool bCheck;
			ReadFile(hFile, &bCheck, sizeof(_bool), &dwByte, nullptr);

			if (true == bCheck)
			{
				_tchar szMaterialFullPath[MAX_PATH] = TEXT("");
				_tchar szMaterialFileName[MAX_PATH] = TEXT("");
				_tchar szMaterialName[MAX_PATH] = TEXT("");
				_tchar szExt[MAX_PATH] = TEXT("");

				ReadFile(hFile, szMaterialFileName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
				_wsplitpath_s(szMaterialFileName, nullptr, 0, nullptr, 0, szMaterialName, MAX_PATH, szExt, MAX_PATH);

				lstrcpy(szMaterialFullPath, szBasePath);
				lstrcat(szMaterialFullPath, TEXT("Material/"));
				//lstrcat(szMaterialFullPath, szMaterialFileName);
				lstrcat(szMaterialFullPath, szMaterialName);
				lstrcat(szMaterialFullPath, TEXT(".tga"));

				pMaterial->pMaterialTexture[iTextureType] = CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_TGA, szMaterialFullPath, iMaterialSetCount, iTextureType);

				//if (!lstrcmp(szExt, TEXT(".dds")))
				//	pMaterial->pMaterialTexture[iTextureType] = CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_DDS, szMaterialFullPath, iMaterialSetCount, iTextureType);
				//else if (!lstrcmp(szExt, TEXT(".tga")))
				//	pMaterial->pMaterialTexture[iTextureType] = CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_TGA, szMaterialFullPath, iMaterialSetCount, iTextureType);
				//else
				//	pMaterial->pMaterialTexture[iTextureType] = CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, szMaterialFullPath, iMaterialSetCount, iTextureType);
			}
		}
		Materials.emplace_back(pMaterial);
	}

	////////////////////////
	if (TYPE_INSTANCE == eModelType)
	{
		CloseHandle(hFile);
		static_cast<CModel_Instance*>(pModel)->Bring_Containers(pVertices, iVertexCount, pFaces, iFaceCount, Meshes, Materials);
		return S_OK;
	}
	////////////////////////

	/* Load_Nodes&Transformations */
	ReadFile(hFile, &iCount, sizeof(_uint), &dwByte, nullptr);
	Nodes.reserve(iCount);
	Transformations.resize(iCount);
	for (_uint iIndex = 0; iIndex < iCount; ++iIndex)
	{
		CHierarchyNode::NODE_DESC NodeDesc;
		ReadFile(hFile, &NodeDesc, sizeof(CHierarchyNode::NODE_DESC), &dwByte, nullptr);
		CHierarchyNode* pNode = CHierarchyNode::Create(NodeDesc);
		Nodes.emplace_back(pNode);
	}
	if (iCount > 0)
		ReadFile(hFile, &Transformations[0], sizeof(_float4x4) * iCount, &dwByte, nullptr);

	CloseHandle(hFile);

	/* For.Anim */
	_tchar szAnimFullPath[MAX_PATH] = TEXT("");
	_tchar szAnimPath[MAX_PATH] = TEXT("");

	lstrcat(szBasePath, TEXT("Animation/"));

	lstrcpy(szAnimPath, szBasePath);
	lstrcat(szAnimPath, TEXT("*.Anim"));

	//if ((_waccess(szFullPath, 0)) == -1)
	//	return S_OK;

	_wfinddata_t anim_file;
	intptr_t hAnimFile;
	_uint iAnimCount = 0;

	if ((hAnimFile = _wfindfirst(szAnimPath, &anim_file)) >= 0)
	{
		do {
			lstrcpy(szAnimFullPath, szBasePath);
			lstrcat(szAnimFullPath, anim_file.name);

			++iAnimCount;

			hFile = CreateFile(szAnimFullPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if (INVALID_HANDLE_VALUE == hFile) return E_FAIL;

			CAnim::ANIM_DESC AnimDesc;
			ReadFile(hFile, &AnimDesc, sizeof(CAnim::ANIM_DESC), &dwByte, nullptr);
			CAnim* pAnim = CAnim::Create(AnimDesc);

			CHANNELS Channels;
			ReadFile(hFile, &iCount, sizeof(_uint), &dwByte, nullptr);
			Channels.reserve(iCount);

			for (_uint iChannelIndex = 0; iChannelIndex < iCount; ++iChannelIndex)
			{
				CAnimChannel::CHANNEL_DESC ChannelDesc;
				ReadFile(hFile, &ChannelDesc, sizeof(CAnimChannel::CHANNEL_DESC), &dwByte, nullptr);
				CAnimChannel* pChannel = CAnimChannel::Create(ChannelDesc);

				_uint iKeyFrameCount;
				ReadFile(hFile, &iKeyFrameCount, sizeof(_uint), &dwByte, nullptr);

				if (iKeyFrameCount > 0)
				{
					KEYFRAMES KeyFrames;
					KeyFrames.resize(iKeyFrameCount);
					ReadFile(hFile, &KeyFrames[0], sizeof(KEY_FRAME) * iKeyFrameCount, &dwByte, nullptr);
					pChannel->Bring_KeyFrameContainer(KeyFrames);
				}
				Channels.emplace_back(pChannel);
			}

			pAnim->Bring_ChannelContainer(Channels);
			Anims.emplace_back(pAnim);


			CloseHandle(hFile);
		} while (_wfindnext(hAnimFile, &anim_file) == 0);
		_findclose(hAnimFile);
	}




	if (TYPE_NORMAL == eModelType)
		static_cast<CModel*>(pModel)->Bring_Containers(pVertices, iVertexCount, pFaces, iFaceCount, Meshes, Materials, Nodes, Transformations, Anims);
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CModel_Loader::Load_PathFromFile(void** ppOutput, const _tchar* pPathFilePath)
{
	HANDLE hFile = CreateFile(pPathFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile) return E_FAIL;

	_ulong dwByte = 0;
	_uint	iCount = 0;

	/* For.Anim */
	// 애니메이션 이름 상관없음.
	// 패스태그를 이름으로 사용 (애니메이션 이름 중복 상관X)
	CAnim::ANIM_DESC AnimDesc;
	ReadFile(hFile, &AnimDesc, sizeof(CAnim::ANIM_DESC), &dwByte, nullptr);
	CAnim* pAnim = CAnim::Create(AnimDesc);

	CHANNELS Channels;
	ReadFile(hFile, &iCount, sizeof(_uint), &dwByte, nullptr);
	Channels.reserve(iCount);

	for (_uint iChannelIndex = 0; iChannelIndex < iCount; ++iChannelIndex)
	{
		CAnimChannel::CHANNEL_DESC ChannelDesc;
		ReadFile(hFile, &ChannelDesc, sizeof(CAnimChannel::CHANNEL_DESC), &dwByte, nullptr);
		CAnimChannel* pChannel = CAnimChannel::Create(ChannelDesc);

		_uint iKeyFrameCount;
		ReadFile(hFile, &iKeyFrameCount, sizeof(_uint), &dwByte, nullptr);

		if (iKeyFrameCount > 0)
		{
			KEYFRAMES KeyFrames;
			KeyFrames.resize(iKeyFrameCount);
			ReadFile(hFile, &KeyFrames[0], sizeof(KEY_FRAME) * iKeyFrameCount, &dwByte, nullptr);
			pChannel->Bring_KeyFrameContainer(KeyFrames);
		}
		Channels.emplace_back(pChannel);
	}
	pAnim->Bring_ChannelContainer(Channels);

	CloseHandle(hFile);

	/* Copy to Anim Pointer */
	(*ppOutput) = pAnim;

	return S_OK;
}