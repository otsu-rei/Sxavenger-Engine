#include "Model.h"

/* 右手座標から左手座標系への変換
 position = x, y, -z;
 normal   = x, y, -z;
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// MyEngine
#include <MyEngine.h>

// logger
#include <Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Model class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Model::Init(const std::string& directoryPath, const std::string& filename) {

	// modelDataの取得
	modelData_ = ModelMethods::LoadModelFile(directoryPath, filename);

	// modelIndexSizeの保存
	modelIndexSize_ = static_cast<uint32_t>(modelData_.meshes.size());

	// modelで使うtextureの呼び出し
	for (auto& material : modelData_.materials) {
		//!< textureの読み込み
		for (int i = 0; i < kCountOfTextureType; ++i) {
			//!< textureを使わないので
			if (material.textureFilePaths[i].empty()) { continue; }

			MyEngine::LoadTexture(material.textureFilePaths[i]);
		}
	}

}

void Model::Term() {

	// modelで使ったtextureの解放
	for (auto& material : modelData_.materials) {
		//!< texture解放
		for (int i = 0; i < kCountOfTextureType; ++i) {
			//!< textureを使わないので
			if (material.textureFilePaths[i].empty()) { continue; }

			MyEngine::ReleaseTexture(material.textureFilePaths[i]);
		}
	}

	// modelの解放
	modelData_.Clear();
	modelIndexSize_ = 0;

}

void Model::SetBuffers(ID3D12GraphicsCommandList* commandList, uint32_t modelIndex) {

	if (modelIndex >= modelIndexSize_) {
		assert(false); //!< 配列以上のmodelDataの呼び出し
	}

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = modelData_.meshes[modelIndex].vertexResource->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW indexBufferView = modelData_.meshes[modelIndex].indexResource->GetIndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);
}

void Model::SetGraphicsTextureHandle(ID3D12GraphicsCommandList* commandList, uint32_t modelIndex, UINT parameterNum, TextureType type) {

	if (modelData_.materials[modelIndex].textureFilePaths[type].empty()) { assert(false); } //!< textureを使ってないので

	commandList->SetGraphicsRootDescriptorTable(parameterNum, MyEngine::GetTextureHandleGPU(modelData_.materials[modelIndex].textureFilePaths[type]));
}

void Model::DrawCall(ID3D12GraphicsCommandList* commandList, uint32_t modelIndex, uint32_t instanceCount) {
	commandList->DrawIndexedInstanced(modelData_.meshes[modelIndex].indexResource->GetIndexSize(), instanceCount, 0, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////
// ModelMethods namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

ModelData ModelMethods::LoadModelFile(const std::string& directoryPath, const std::string& filename) {

	ModelData result;

	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;

	uint32_t option = 0;
	option |= aiProcess_FlipWindingOrder;
	option |= aiProcess_FlipUVs;
	option |= aiProcess_JoinIdenticalVertices;
	option |= aiProcess_Triangulate;

	const aiScene* scene
		= importer.ReadFile(filePath.c_str(), option);

	assert(scene->HasMeshes()); //!< メッシュナシは未対応

	// ModelDataの要素数の指定
	result.meshes.resize(scene->mNumMeshes);
	result.materials.resize(scene->mNumMaterials);

	// meshesの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {

		// meshの取得
		aiMesh* mesh = scene->mMeshes[meshIndex];

		//assert(mesh->HasNormals());        //!< 法線がない場合は未対応
		//assert(mesh->HasTextureCoords(0)); //!< Texcoordがない場合は未対応 todo...

		// 保存の確保
		result.meshes[meshIndex].Create(MyEngine::GetDevicesObj(), mesh->mNumVertices, mesh->mNumFaces * 3);
		auto& meshData = result.meshes[meshIndex];

		// verticesの解析
		for (uint32_t element = 0; element < mesh->mNumVertices; ++element) {

			// 自前の構造体に保存
			VertexData vtx;
			vtx.Init();

			aiVector3D& position = mesh->mVertices[element];
			vtx.position = { position.x, position.y, -position.z }; //!< 左手座標系に変換

			if (mesh->HasNormals()) {
				aiVector3D& normal = mesh->mNormals[element];
				vtx.normal = { normal.x, normal.y, -normal.z }; //!< 左手座標系に変換
			}

			if (mesh->HasTextureCoords(0)) {
				aiVector3D& texcoord = mesh->mTextureCoords[0][element];
				vtx.texcoord = { texcoord.x, texcoord.y };
			}

			// データの保存
			meshData.vertexResource->operator[](element) = vtx;

		}

		// faceの解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {

			// faceの取得
			aiFace& face = mesh->mFaces[faceIndex];

			assert(face.mNumIndices == 3); //!< 三角形のみの対応

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				// データの保存
				meshData.indexResource->operator[](faceIndex * 3 + element) = face.mIndices[element];
			}
		}
	}

	// materialsの解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {

		// materialの取得
		aiMaterial* material = scene->mMaterials[materialIndex];

		// materialデータの保存場所の指定
		auto& materialData = result.materials[materialIndex];

		// diffuse textureの取得
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			
			// データの保存
			materialData.textureFilePaths[TEXTURE_DIFFUSE] = directoryPath + "/" + textureFilePath.C_Str();
		}

		// normal textureの取得
		if (material->GetTextureCount(aiTextureType_NORMALS) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_NORMALS, 0, &textureFilePath);

			// データの保存
			materialData.textureFilePaths[TEXTURE_NORMAL] = directoryPath + "/" + textureFilePath.C_Str();
		}
	}

	// nodeの取得
	result.rootNode = ReadNode(scene->mRootNode);
	
	return result;
}

Node ModelMethods::ReadNode(aiNode* node) {

	Node result;

	// transformの取得
	aiVector3D scale, translate;
	aiQuaternion rotate;

	node->mTransformation.Decompose(scale, rotate, translate);

	// resultに代入
	result.transform.scale     = { scale.x, scale.y, scale.z };
	result.transform.rotate    = { rotate.x, -rotate.y, -rotate.z, rotate.w }; //!< 右手 -> 左手座標系に変換
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

Animation ModelMethods::LoadAnimationFile(const std::string& directoryPath, const std::string& filename) {
	// todo: LoadOBJFileを使う場合, modelを二回読み込んでることになるので統合する
	Animation result;

	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	//* 同じimporter, sceneを使うからクラスにしてもいい

	// filePathからsceneの読み込み
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);

	assert(scene->mAnimations != 0); //!< animationがない

	aiAnimation* animationAi = scene->mAnimations[0];
	// todo: 複数に対応する

	result.duration = static_cast<float>(animationAi->mDuration / animationAi->mTicksPerSecond); //!< 時間の単位を秒に変更

	// channelを回してNodeAnimationを取得
	for (uint32_t channelIndex = 0; channelIndex < animationAi->mNumChannels; ++channelIndex) {

		// assimpではchannelがnodeAnimationを所持しているので取得
		aiNodeAnim* nodeAnimationAi = animationAi->mChannels[channelIndex];

		// NodeAnimationの参照を取得
		NodeAnimation& nodeAnimation = result.nodeAnimations[nodeAnimationAi->mNodeName.C_Str()];

		// positionKeyの取得
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAi->mNumPositionKeys; ++keyIndex) {

			// keyの取得
			aiVectorKey& keyAi = nodeAnimationAi->mPositionKeys[keyIndex];

			// 結果代入先
			KeyframeVector3 keyframe;
			keyframe.time  = static_cast<float>(keyAi.mTime / animationAi->mTicksPerSecond); //!< 秒に変更
			keyframe.value = { keyAi.mValue.x, keyAi.mValue.y, -keyAi.mValue.z };            //!< 左手座標系に変換

			// keyframeをnodeAnimationに代入
			nodeAnimation.translate.push_back(keyframe);
		}

		// rotate
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAi->mNumRotationKeys; ++keyIndex) {

			// keyの取得
			aiQuatKey& keyAi = nodeAnimationAi->mRotationKeys[keyIndex];

			// 結果代入先
			KeyframeQuaternion keyframe;
			keyframe.time  = static_cast<float>(keyAi.mTime / animationAi->mTicksPerSecond);       //!< 秒に変更
			keyframe.value = { keyAi.mValue.x, -keyAi.mValue.y, -keyAi.mValue.z, keyAi.mValue.w }; //!< 左手座標系に変換

			// keyframeをnodeAnimationに代入
			nodeAnimation.rotate.push_back(keyframe);
		}

		// scale
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAi->mNumScalingKeys; ++keyIndex) {

			// keyの取得
			aiVectorKey& keyAi = nodeAnimationAi->mScalingKeys[keyIndex];

			// 結果代入先
			KeyframeVector3 keyframe;
			keyframe.time  = static_cast<float>(keyAi.mTime / animationAi->mTicksPerSecond); //!< 秒に変更
			keyframe.value = { keyAi.mValue.x, keyAi.mValue.y, keyAi.mValue.z };             //!< valueを代入

			// keyframeをnodeAnimationに代入
			nodeAnimation.scale.push_back(keyframe);
		}
	}

	return result;
}

int32_t ModelMethods::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {

	// jointの構築
	Joint joint;
	joint.name                = node.name;
	joint.localMatrix         = node.localMatrix;
	joint.skeletonSpaceMatrix = Matrix4x4::Identity();
	joint.transform           = node.transform;
	joint.index               = static_cast<int32_t>(joints.size()); //!< 現在登録されてるindexに
	joint.parent              = parent;

	joints.push_back(joint); //!< skeletonのjoin列に追加

	for (auto& child : node.children) {
		// 子のJointを作成し, そのindexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}

	// 自身のindexを返す
	return joint.index;
}

Skeleton ModelMethods::CreateSkeleton(const Node& rootNode) {

	Skeleton skelton;
	skelton.root = CreateJoint(rootNode, {}, skelton.joints);

	// 名前とindexのmapingを行いアクセスしやすいように
	for (auto& joint : skelton.joints) {
		skelton.jointMap.emplace(joint.name, joint.index);
	}

	return skelton;
}
