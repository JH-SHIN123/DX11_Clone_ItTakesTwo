#include "..\public\Model_Loader.h"
#include "Model.h"
#include "Model_Instance.h"
#include "Mesh.h"
#include "Textures.h"
#include "HierarchyNode.h"
#include "Anim.h"
#include "AnimChannel.h"

IMPLEMENT_SINGLETON(CModel_Loader)

HRESULT CModel_Loader::Load_ModelFromFile(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, CModel * pModel, const char * pModelFilePath, const char * pModelFileName)
{
	NULL_CHECK_RETURN(pModel, E_FAIL);

	/* Import_Scene */
	char szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	Assimp::Importer Importer;
	//Importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_ANIMATIONS);
	const aiScene* pScene = Importer.ReadFile(szFullPath, aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	NULL_CHECK_RETURN(pScene, E_FAIL);
	NULL_CHECK_RETURN(pScene->mRootNode, E_FAIL);

	/* Create_Container & Reserve */
	MESHES			Meshes;
	MATERIALS		Materials;
	TEMPNODES		TempNodes;
	NODES			Nodes;
	TRANSFORMATIONS	Transformations;
	ANIMS			Anims;

	_uint	iVertexCount	= 0;
	_uint	iFaceCount		= 0;
	_uint	iNodeCount		= 0;
	_uint	iAnimCount		= pScene->mNumAnimations;

	for (_uint iIndex = 0; iIndex < pScene->mNumMeshes; ++iIndex)
	{
		iVertexCount	+= pScene->mMeshes[iIndex]->mNumVertices;
		iFaceCount		+= pScene->mMeshes[iIndex]->mNumFaces;
	}

	NULL_CHECK_RETURN(iVertexCount, E_FAIL);
	NULL_CHECK_RETURN(iFaceCount, E_FAIL);

	VTXMESH* pVertices = new VTXMESH[iVertexCount];
	ZeroMemory(pVertices, sizeof(VTXMESH) * iVertexCount);

	POLYGON_INDICES32* pFaces = new POLYGON_INDICES32[iFaceCount];
	ZeroMemory(pFaces, sizeof(POLYGON_INDICES32) * iFaceCount);

	Find_NodeCount(pScene->mRootNode, iNodeCount);

	Meshes.reserve(pScene->mNumMeshes);
	Materials.reserve(pScene->mNumMaterials);
	Nodes.reserve(iNodeCount);
	TempNodes.reserve(iNodeCount);
	Transformations.reserve(iNodeCount);
	Anims.reserve(iAnimCount);


	/* Load_Meshes */
	FAILED_CHECK_RETURN(Add_MeshToContainer(pScene, pVertices, pFaces, Meshes), E_FAIL);
	/* Load_Materials */
	FAILED_CHECK_RETURN(Add_MaterialToContainer(pDevice, pDeviceContext, pScene, pModelFilePath, Materials), E_FAIL);
	/* Load_Hierarchy */
	FAILED_CHECK_RETURN(Add_NodeToContainer(pScene->mRootNode, Meshes, TempNodes), E_FAIL);
	/* Sort_Nodes */
	sort(TempNodes.begin(), TempNodes.end(), [](TEMPNODE pDst, TEMPNODE pSrc) { return pDst.first->Get_Depth() < pSrc.first->Get_Depth(); });
	/* Set_NodeIndex */
	Set_NodeIndex(TempNodes, Nodes, Transformations);
	/* Load_BoneInfo */
	FAILED_CHECK_RETURN(Add_BoneInfoToContainer(pScene, pVertices, Meshes, Nodes), E_FAIL);
	/* Load_Animations */
	FAILED_CHECK_RETURN(Add_AnimToContainer(pScene, Nodes, Anims), E_FAIL);
	/* Send_Containers */
	pModel->Bring_Containers(pVertices, iVertexCount, pFaces, iFaceCount, Meshes, Materials, Nodes, Transformations, Anims);

	return S_OK;
}

HRESULT CModel_Loader::Load_ModelFromFile(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, CModel_Instance * pModel, const char * pModelFilePath, const char * pModelFileName)
{
	NULL_CHECK_RETURN(pModel, E_FAIL);

	/* Import_Scene */
	char szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	Assimp::Importer Importer;
	//Importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_ANIMATIONS);
	const aiScene* pScene = Importer.ReadFile(szFullPath, aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	NULL_CHECK_RETURN(pScene, E_FAIL);
	NULL_CHECK_RETURN(pScene->mRootNode, E_FAIL);

	/* Create_Container & Reserve */
	MESHES			Meshes;
	MATERIALS		Materials;

	_uint	iVertexCount = 0;
	_uint	iFaceCount = 0;

	for (_uint iIndex = 0; iIndex < pScene->mNumMeshes; ++iIndex)
	{
		iVertexCount += pScene->mMeshes[iIndex]->mNumVertices;
		iFaceCount += pScene->mMeshes[iIndex]->mNumFaces;
	}

	NULL_CHECK_RETURN(iVertexCount, E_FAIL);
	NULL_CHECK_RETURN(iFaceCount, E_FAIL);

	VTXMESH* pVertices = new VTXMESH[iVertexCount];
	ZeroMemory(pVertices, sizeof(VTXMESH) * iVertexCount);

	POLYGON_INDICES32* pFaces = new POLYGON_INDICES32[iFaceCount];
	ZeroMemory(pFaces, sizeof(POLYGON_INDICES32) * iFaceCount);

	Meshes.reserve(pScene->mNumMeshes);
	Materials.reserve(pScene->mNumMaterials);

	/* Load_Meshes */
	FAILED_CHECK_RETURN(Add_MeshToContainer(pScene, pVertices, pFaces, Meshes), E_FAIL);
	/* Load_Materials */
	FAILED_CHECK_RETURN(Add_MaterialToContainer(pDevice, pDeviceContext, pScene, pModelFilePath, Materials), E_FAIL);
	/* Send_Containers */
	pModel->Bring_Containers(pVertices, iVertexCount, pFaces, iFaceCount, Meshes, Materials);

	return S_OK;
}

HRESULT CModel_Loader::Add_MeshToContainer(const aiScene * pScene, VTXMESH * pVertices, POLYGON_INDICES32 * pFaces, MESHES & Meshes)
{
	_uint iCurrentVertexIndex	= 0;
	_uint iCurrentFaceIndex		= 0;

	for (_uint iMeshIndex = 0; iMeshIndex < pScene->mNumMeshes; ++iMeshIndex)
	{
		aiMesh* pAiMesh = pScene->mMeshes[iMeshIndex];

		NULL_CHECK_RETURN(pAiMesh->mNumVertices, E_FAIL);

		CMesh* pMesh = CMesh::Create(pAiMesh->mName.data, iCurrentVertexIndex, iCurrentFaceIndex, pAiMesh->mNumFaces, pAiMesh->mMaterialIndex);
		Meshes.emplace_back(pMesh);

		/* Add_VertexToContainer */
		for (_uint iVertexIndex = 0; iVertexIndex < pAiMesh->mNumVertices; ++iVertexIndex)
		{
			memcpy(&pVertices[iCurrentVertexIndex].vPosition,	&pAiMesh->mVertices[iVertexIndex],			sizeof(_float3));
			memcpy(&pVertices[iCurrentVertexIndex].vNormal,		&pAiMesh->mNormals[iVertexIndex],			sizeof(_float3));
			memcpy(&pVertices[iCurrentVertexIndex].vTangent,	&pAiMesh->mTangents[iVertexIndex],			sizeof(_float3));
			memcpy(&pVertices[iCurrentVertexIndex++].vTexUV,	&pAiMesh->mTextureCoords[0][iVertexIndex],	sizeof(_float2));
		}
		/* Add_IndexToContainer */
		for (_uint iFaceIndex = 0; iFaceIndex < pAiMesh->mNumFaces; ++iFaceIndex)
		{
			pFaces[iCurrentFaceIndex]._0 = pAiMesh->mFaces[iFaceIndex].mIndices[0];
			pFaces[iCurrentFaceIndex]._1 = pAiMesh->mFaces[iFaceIndex].mIndices[1];
			pFaces[iCurrentFaceIndex]._2 = pAiMesh->mFaces[iFaceIndex].mIndices[2];
			++iCurrentFaceIndex;
		}
	}

	return S_OK;
}

HRESULT CModel_Loader::Add_MaterialToContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const aiScene * pScene, const char * pModelFilePath, MATERIALS & Materials)
{
	for (_uint iMaterialIndex = 0; iMaterialIndex < pScene->mNumMaterials; ++ iMaterialIndex)
	{
		MATERIAL* pMaterial = new MATERIAL;
		ZeroMemory(pMaterial, sizeof(MATERIAL));

		for (_uint iTextureType = 0; iTextureType < AI_TEXTURE_TYPE_MAX; ++iTextureType)
		{
			aiString strTexturePath;

			if (FAILED(pScene->mMaterials[iMaterialIndex]->GetTexture(aiTextureType(iTextureType), 0, &strTexturePath)))
				continue;

			char	szTextureFileName[MAX_PATH] = "";
			char	szExt[MAX_PATH] = "";
			char	szFullPath[MAX_PATH] = "";
			_tchar	szFullPathW[MAX_PATH] = TEXT("");

			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szTextureFileName, MAX_PATH, szExt, MAX_PATH);
			strcat_s(szTextureFileName, szExt);
			strcpy_s(szFullPath, pModelFilePath);
			strcat_s(szFullPath, szTextureFileName);
			MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szFullPathW, MAX_PATH);

			if (!strcmp(szExt, ".dds"))
				pMaterial->pMaterialTexture[iTextureType] = CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_DDS, szFullPathW);
			else if (!strcmp(szExt, ".tga"))
				pMaterial->pMaterialTexture[iTextureType] = CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_TGA, szFullPathW);
			else
				pMaterial->pMaterialTexture[iTextureType] = CTextures::Create(pDevice, pDeviceContext, CTextures::TYPE_WIC, szFullPathW);
		}
		Materials.emplace_back(pMaterial);
	}

	return S_OK;
}

HRESULT CModel_Loader::Add_NodeToContainer(aiNode * pAiNode, MESHES & Meshes, TEMPNODES & TempNodes, CHierarchyNode * pParent, _uint iDepth)
{
	_float4x4 TransformationMatrix;

	XMStoreFloat4x4(&TransformationMatrix, XMMatrixTranspose(XMMATRIX(pAiNode->mTransformation[0])));

	CHierarchyNode*	pNode = CHierarchyNode::Create(pAiNode->mName.data, pParent, iDepth);
	NULL_CHECK_RETURN(pNode, E_FAIL);

	TEMPNODE TempNode(pNode, TransformationMatrix);

	TempNodes.emplace_back(TempNode);

	for (_uint iIndex = 0; iIndex < pAiNode->mNumChildren; ++iIndex)
		Add_NodeToContainer(pAiNode->mChildren[iIndex], Meshes, TempNodes, pNode, iDepth + 1);

	return S_OK;
}

HRESULT CModel_Loader::Add_BoneInfoToContainer(const aiScene * pScene, VTXMESH * pVertices, MESHES & Meshes, NODES & Nodes)
{
	for (_uint iMeshIndex = 0; iMeshIndex < pScene->mNumMeshes; ++iMeshIndex)
	{
		aiMesh* pMesh = pScene->mMeshes[iMeshIndex];

		BONES	Bones;
		Bones.reserve(pMesh->mNumBones);

		for (_uint iBoneIndex = 0; iBoneIndex < pMesh->mNumBones; ++iBoneIndex)
		{
			aiBone* pBone = pMesh->mBones[iBoneIndex];
			NULL_CHECK_RETURN(pBone, E_FAIL);

			BONE_DESC* pBoneDesc = new BONE_DESC;
			ZeroMemory(pBoneDesc, sizeof(BONE_DESC));

			pBoneDesc->pHierarchyNode = Find_Node_ByName(Nodes, pBone->mName.data);
			NULL_CHECK_RETURN(pBoneDesc->pHierarchyNode, E_FAIL);
			Safe_AddRef(pBoneDesc->pHierarchyNode);

			memcpy(&pBoneDesc->OffsetMatrix, &XMMatrixTranspose(XMMATRIX(pBone->mOffsetMatrix[0])), sizeof(_matrix));

			for (_uint iWeightIndex = 0; iWeightIndex < pBone->mNumWeights; ++iWeightIndex)
			{
				_uint	iVertexIndex = pBone->mWeights[iWeightIndex].mVertexId + Meshes[iMeshIndex]->Get_StartVertexIndex();

				if (0.f == pVertices[iVertexIndex].vBlendWeight.x)
				{
					pVertices[iVertexIndex].vBlendIndex.x	= iBoneIndex;
					pVertices[iVertexIndex].vBlendWeight.x	= pBone->mWeights[iWeightIndex].mWeight;
				}
				else if (0.f == pVertices[iVertexIndex].vBlendWeight.y)
				{
					pVertices[iVertexIndex].vBlendIndex.y	= iBoneIndex;
					pVertices[iVertexIndex].vBlendWeight.y	= pBone->mWeights[iWeightIndex].mWeight;
				}
				else if (0.f == pVertices[iVertexIndex].vBlendWeight.z)
				{
					pVertices[iVertexIndex].vBlendIndex.z	= iBoneIndex;
					pVertices[iVertexIndex].vBlendWeight.z	= pBone->mWeights[iWeightIndex].mWeight;
				}
				else
				{
					pVertices[iVertexIndex].vBlendIndex.w	= iBoneIndex;
					pVertices[iVertexIndex].vBlendWeight.w	= pBone->mWeights[iWeightIndex].mWeight;
				}
			}
			Bones.emplace_back(pBoneDesc);
		}
		FAILED_CHECK_RETURN(Meshes[iMeshIndex]->Bring_BoneContainer(Bones), E_FAIL);
	}

	return S_OK;
}

HRESULT CModel_Loader::Add_AnimToContainer(const aiScene * pScene, NODES & Nodes, ANIMS & Anims)
{
	_uint iAnimCount = pScene->mNumAnimations;

	if(0 == iAnimCount)
		return S_OK;

	for (_uint iAnimIndex = 0; iAnimIndex < iAnimCount; ++iAnimIndex)
	{
		aiAnimation* pAiAnim = pScene->mAnimations[iAnimIndex];
		NULL_CHECK_RETURN(pAiAnim, E_FAIL);
		CAnim* pAnim = CAnim::Create(pAiAnim->mName.data, pAiAnim->mDuration, pAiAnim->mTicksPerSecond);
		NULL_CHECK_RETURN(pAnim, E_FAIL);

		_uint iChannelCount	= pAiAnim->mNumChannels;

		CHANNELS Channels;
		Channels.reserve(iChannelCount);

		_vector vScale		= XMVectorZero();
		_vector vRotation	= XMVectorZero();
		_vector vPosition	= XMVectorZero();

		for (_uint iChannelIndex = 0; iChannelIndex < iChannelCount; ++iChannelIndex)
		{
			aiNodeAnim* pAnimNode = pAiAnim->mChannels[iChannelIndex];
			NULL_CHECK_RETURN(pAnimNode, E_FAIL);

			CAnimChannel* pChannel = CAnimChannel::Create(pAnimNode->mNodeName.data);
			NULL_CHECK_RETURN(pChannel, E_FAIL);

			_uint iKeyFrameCount = pAnimNode->mNumPositionKeys;
			iKeyFrameCount = max(iKeyFrameCount, pAnimNode->mNumScalingKeys);
			iKeyFrameCount = max(iKeyFrameCount, pAnimNode->mNumRotationKeys);

			KEYFRAMES KeyFrames;
			KeyFrames.reserve(iKeyFrameCount);

			for (_uint iKeyFrameIndex = 0; iKeyFrameIndex < iKeyFrameCount; ++iKeyFrameIndex)
			{
				KEY_FRAME* pKeyFrame = new KEY_FRAME;
				ZeroMemory(pKeyFrame, sizeof(KEY_FRAME));

				if (pAnimNode->mNumScalingKeys	> iKeyFrameIndex)
				{
					memcpy(&pKeyFrame->dTime, &pAnimNode->mScalingKeys[iKeyFrameIndex].mTime, sizeof(_double));
					memcpy(&vScale, &pAnimNode->mScalingKeys[iKeyFrameIndex].mValue, sizeof(_float3));
				}
				if (pAnimNode->mNumRotationKeys > iKeyFrameIndex)
				{
					memcpy(&pKeyFrame->dTime, &pAnimNode->mRotationKeys[iKeyFrameIndex].mTime, sizeof(_double));
					vRotation = XMVectorSetX(vRotation, pAnimNode->mRotationKeys[iKeyFrameIndex].mValue.x);
					vRotation = XMVectorSetY(vRotation, pAnimNode->mRotationKeys[iKeyFrameIndex].mValue.y);
					vRotation = XMVectorSetZ(vRotation, pAnimNode->mRotationKeys[iKeyFrameIndex].mValue.z);
					vRotation = XMVectorSetW(vRotation, pAnimNode->mRotationKeys[iKeyFrameIndex].mValue.w);
				}
				if (pAnimNode->mNumPositionKeys > iKeyFrameIndex)
				{
					memcpy(&pKeyFrame->dTime, &pAnimNode->mPositionKeys[iKeyFrameIndex].mTime, sizeof(_double));
					memcpy(&vPosition, &pAnimNode->mPositionKeys[iKeyFrameIndex].mValue, sizeof(_float3));
				}

				memcpy(&pKeyFrame->vScale, &vScale, sizeof(_float3));
				memcpy(&pKeyFrame->vRotation, &vRotation, sizeof(_float4));
				memcpy(&pKeyFrame->vPosition, &vPosition, sizeof(_float3));

				KeyFrames.emplace_back(pKeyFrame);
			}
			auto iter = find_if(Nodes.begin(), Nodes.end(), [pChannel](CHierarchyNode* pNode) { return !strcmp(pChannel->Get_Name(), pNode->Get_Name()); });
			NULL_CHECK_RETURN(iter != Nodes.end(), E_FAIL);

			pChannel->Set_ConnectedNodeIndex((*iter)->Get_NodeIndex());
			pChannel->Bring_KeyFrameContainer(KeyFrames);
			Channels.emplace_back(pChannel);
		}
		pAnim->Bring_ChannelContainer(Channels);
		Anims.emplace_back(pAnim);
	}

	return S_OK;
}

void CModel_Loader::Find_NodeCount(aiNode * pAiNode, _uint & iCount)
{
	++iCount;

	for (_uint iIndex = 0; iIndex < pAiNode->mNumChildren; ++iIndex)
		Find_NodeCount(pAiNode->mChildren[iIndex], iCount);
}

CHierarchyNode * CModel_Loader::Find_Node_ByName(NODES & Nodes, const char * pNodeName)
{
	auto& iter = find_if(Nodes.begin(), Nodes.end(), [pNodeName](CHierarchyNode* pNode) { return !strcmp(pNodeName, pNode->Get_Name()); });

	if (iter == Nodes.end())
		return nullptr;
	else
		return *iter;
}

void CModel_Loader::Set_NodeIndex(TEMPNODES & TempNodes, NODES & Nodes, TRANSFORMATIONS & Transformations)
{
	_uint iIndex = 0;

	for (auto& pTempNode : TempNodes)
	{
		Nodes.emplace_back(pTempNode.first);
		pTempNode.first->Set_NodeIndex(iIndex++);
		Transformations.emplace_back(pTempNode.second);
	}

	TempNodes.clear();
}

void CModel_Loader::Free()
{
}