#include "Model.h"
//* 右手座標から左手座標系への変換 -zを採用
//* Vector3
//*  position = x, y, -z;
//*  normal   = x, y, -z;
//*
//* Vector4
//*  Quarternion = -x, -y, z, w;

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Content/SxavengerContent.h>

//=========================================================================================
// static variables
//=========================================================================================

const uint32_t Model::kDefaultAssimpOption_ = aiProcess_FlipWindingOrder | aiProcess_FlipUVs;

////////////////////////////////////////////////////////////////////////////////////////////
// Model class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Model::Load(const std::string& directory, const std::string& filename, const DirectXThreadContext* context, uint32_t assimpOption) {
	TaskThreadExecution::SetState(ExecutionState::kRunning);

	directory_ = directory;
	filename_  = filename;

	std::string filepath = directory_ + "/" + filename_;

	// sceneの取得
	Assimp::Importer importer; //!< scene保存するため保管
	const aiScene* aiScene = importer.ReadFile(filepath.c_str(), assimpOption);

	Assert(aiScene != nullptr, "model load failed. filepath: " + filepath, importer.GetErrorString());
	Assert(aiScene->HasMeshes()); //!< メッシュナシは未対応

	LoadMesh(aiScene);
	LoadMaterial(aiScene, directory_, context);

	root_ = ReadNode(aiScene->mRootNode);

	TaskThreadExecution::SetState(ExecutionState::kCompleted);
}

void Model::Term() {
}

void Model::AsyncLoad(const std::string& directory, const std::string& filename, uint32_t assimpOption) {

	directory_ = directory;
	filename_  = filename;

	TaskThreadExecution::SetTask([&](_MAYBE_UNUSED const Thread* const thread) {
		Load(directory_, filename_, thread, assimpOption);
	});

	SxavengerSystem::PushTask(this);
}

void Model::LoadMesh(const aiScene* aiScene) {

	// mesh数のメモリ確保
	meshes_.resize(aiScene->mNumMeshes);

	// meshesの解析
	for (uint32_t meshIndex = 0; meshIndex < aiScene->mNumMeshes; ++meshIndex) {

		// meshの取得
		const aiMesh* aiMesh = aiScene->mMeshes[meshIndex];

		//* 
		//* InputAssembler
		//* 

		// InputAssemblerの初期化
		auto& ia = meshes_.at(meshIndex).mesh;
		ia.Create(aiMesh->mNumVertices, aiMesh->mNumFaces * 3 /*_triangle*/);

		auto vertex = ia.GetVertex();
		auto index  = ia.GetIndex();

		// verticesの解析
		for (uint32_t element = 0; element < aiMesh->mNumVertices; ++element) {
			//!< position
			const aiVector3D& position = aiMesh->mVertices[element];
			(*vertex)[element].position = { position.x, position.y, -position.z }; //!< 左手座標系に変換

			//!< normal
			if (aiMesh->HasNormals()) {
				aiVector3D& normal = aiMesh->mNormals[element];
				(*vertex)[element].normal = { normal.x, normal.y, -normal.z }; //!< 左手座標系に変換
			}

			//!< texcoord
			if (aiMesh->HasTextureCoords(0)) {
				aiVector3D& texcoord = aiMesh->mTextureCoords[0][element];
				(*vertex)[element].texcoord = { texcoord.x, texcoord.y };
			}
		}

		// faceの解析
		for (uint32_t faceIndex = 0; faceIndex < aiMesh->mNumFaces; ++faceIndex) {
			// faceの取得
			const aiFace& aiFace = aiMesh->mFaces[faceIndex];

			Assert(aiFace.mNumIndices == 3); //!< 三角形のみの対応

			// indexの解析
			for (uint32_t element = 0; element < aiFace.mNumIndices; ++element) {
				//!< index
				(*index)[faceIndex * 3 + element] = aiFace.mIndices[element];
			}
		}

		//* 
		//* SkinCluster
		//*

		auto& skinCluster = meshes_.at(meshIndex).skinCluster;

		// skinClusterの解析
		for (uint32_t boneIndex = 0; boneIndex < aiMesh->mNumBones; ++boneIndex) {

			// jointごとの格納領域を作る
			const aiBone* aiBone = aiMesh->mBones[boneIndex];

			// clusterの登録
			std::string name = aiBone->mName.C_Str();
			JointWeightData& jointWeightData = skinCluster[name];

			// inverseBindPoseMatrixの抽出
			aiMatrix4x4 aiBindPoseMatrix = aiBone->mOffsetMatrix;
			aiBindPoseMatrix.Inverse();

			aiVector3D scale, translate;
			aiQuaternion rotate;
			aiBindPoseMatrix.Decompose(scale, rotate, translate); //!< 成分を抽出

			// 左手系のBindPoseMatrixを作る
			Matrix4x4 bindPoseMatrix = Matrix::MakeAffine(
				{ scale.x, scale.y, scale.z }, { -rotate.x, -rotate.y, rotate.z, rotate.w }, { translate.x, translate.y, -translate.z }
			);

			// inverseBindOiseMatrixにする
			jointWeightData.inverseBindPoseMatrix = bindPoseMatrix.Inverse();

			// weight情報を取り出し
			for (uint32_t weightIndex = 0; weightIndex < aiBone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.emplace_back(aiBone->mWeights[weightIndex].mWeight, aiBone->mWeights[weightIndex].mVertexId);
			}
		}

		//* 
		//* Material
		//*

		auto& materialIndex = meshes_.at(meshIndex).materialIndex;
		materialIndex = aiMesh->mMaterialIndex;
	}
}

void Model::LoadMaterial(const aiScene* aiScene, const std::string& directory, const DirectXThreadContext* context) {

	// materail数のメモリ確保
	materials_.resize(aiScene->mNumMaterials);

	// materialsの解析
	for (uint32_t materialIndex = 0; materialIndex < aiScene->mNumMaterials; ++materialIndex) {

		// materialの取得
		const aiMaterial* aiMaterial = aiScene->mMaterials[materialIndex];

		auto& material = materials_.at(materialIndex);

		// diffuse textureの取得
		if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);

			// データの保存
			material.textures_[static_cast<uint8_t>(MaterialTextureType::kDiffuse)] = SxavengerContent::TryLoadTexture(directory + "/" + textureFilePath.C_Str(), context);
		}

		// normal textureの取得
		if (aiMaterial->GetTextureCount(aiTextureType_NORMALS) != 0) {
			aiString textureFilePath;
			aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &textureFilePath);

			// データの保存
			material.textures_[static_cast<uint8_t>(MaterialTextureType::kNormal)] = SxavengerContent::TryLoadTexture(directory + "/" + textureFilePath.C_Str(), context);
		}
	}
}

Node Model::ReadNode(aiNode* node) {
	
	Node result = {};

	// transformの取得
	aiVector3D scale, translate;
	aiQuaternion rotate;

	node->mTransformation.Decompose(scale, rotate, translate);

	// resultに代入
	result.transform.scale     = { scale.x, scale.y, scale.z };
	result.transform.rotate    = { -rotate.x, -rotate.y, rotate.z, rotate.w }; //!< 右手 -> 左手座標系に変換
	result.transform.translate = { translate.x, translate.y, -translate.z };   //!< 右手 -> 左手座標系に変換

	// nodeのlocalMatの取得
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;
	aiLocalMatrix.Transpose();

	// resultにcopy
	std::memcpy(&result.localMatrix, &aiLocalMatrix, sizeof(Matrix4x4));

	// ノード名を格納
	result.name = node->mName.C_Str();

	// 子の数だけの要素の格納
	result.children.resize(node->mNumChildren);

	// 子のノード情報の取得
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}