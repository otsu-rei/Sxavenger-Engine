#pragma once
//*****************************************************************************************
// BaseBufferResource:                don't use
// BufferResource, BufferPtrResource: ConstantBuffer, IASetVertexBuffer
// IndexBufferResouruce:              IASetIndexBuffer
// DynamicBufferResource:             test using
// 
// buffers is use DxObject::StructuredBuffer
//*****************************************************************************************

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>

// c++
#include <cstdint>
#include <cassert>
#include <vector>
#include <memory>
#include <span>

// DxObject
#include <DxObjectMethod.h>
#include <DxDevices.h>

// ComPtr
#include <ComPtr.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxObject {

	////////////////////////////////////////////////////////////////////////////////////////////
	// BaseBufferResource class
	////////////////////////////////////////////////////////////////////////////////////////////
	class BaseBufferResource { //!< Baseクラスなのでこのクラス単体では使わない
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @breif コンストラクタ
		//!
		//! @param[in] indexSize     配列のサイズ
		//! @param[in] structureSize 構造体のサイズ
		BaseBufferResource(uint32_t indexSize, size_t structureSize) {
			Init(indexSize, structureSize);
		}

		//! @brief デストラクタ
		virtual ~BaseBufferResource() {}

		//! @breif 初期化処理
		//! 
		//! @param[in] indexSize     配列のサイズ
		//! @param[in] structureSize 構造体のサイズ
		void Init(uint32_t indexSize, size_t structureSize);

		//! @breif 終了処理
		void Term();

		//* Getters *//

		//! @breif 配列のサイズを取得
		virtual const uint32_t GetIndexSize() const { return indexSize_; }

		//! @breif 構造体のサイズを取得
		const UINT GetStructureSize() const { return structureSize_; }

		//! @brief GPUAddressを取得
		//! 
		//! @return GPUAddressを返却
		virtual const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() {
			return resource_->GetGPUVirtualAddress();
		}

		//! @brief Resourceの取得
		virtual ID3D12Resource* GetResource() { return resource_.Get(); }

	protected:

		//=========================================================================================
		// protected variables
		//=========================================================================================

		ComPtr<ID3D12Resource> resource_;

		uint32_t indexSize_;     //!< 配列のサイズ
		UINT     structureSize_; //!< 構造体のサイズ

		//=========================================================================================
		// protected methods
		//=========================================================================================

		//! @brief 要素数が配列外参照でないかの確認
		//! 
		//! @retval true  ok
		//! @retval false 配列外参照する要素数
		virtual bool CheckElementCount(uint32_t elementCount) const;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// BufferResource class
	////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	class BufferResource
		: public BaseBufferResource {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		BufferResource() = delete;

		//! @breif コンストラクタ
		//! 
		//! @param[in] devices   DxObject::Devices
		//! @param[in] indexSize 配列サイズ
		BufferResource(Devices* devices, uint32_t indexSize)
			: BaseBufferResource(indexSize, sizeof(T)) {
			Init(devices);
		}

		//! @brief デストラクタ
		~BufferResource() override { Term(); }

		//! @brief 初期化処理
		//! 
		//! @param[in] devices DxObject::Devices
		void Init(Devices* devices);

		//! @brief 終了処理
		void Term();

		//! @brief VertexBufferを取得
		//! 
		//! @return VertexBufferを返却
		const D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() {
			D3D12_VERTEX_BUFFER_VIEW result = {};
			result.BufferLocation = GetGPUVirtualAddress();
			result.SizeInBytes    = structureSize_ * indexSize_;
			result.StrideInBytes  = structureSize_;

			return result;
		}

		void Memcpy(const T* value) {
			std::memcpy(mappedDatas_.data(), value, structureSize_ * indexSize_);
		}

		//=========================================================================================
		// operator
		//=========================================================================================

		T& operator[](uint32_t element) {
			CheckElementCount(element);

			return mappedDatas_[element];
		}

		//
		// test functions
		//

		T* GetData() const {
			return mappedDatas_.data();
		}

		const std::span<T>& GetMappedDatas() const {
			return mappedDatas_;
		}

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		std::span<T> mappedDatas_;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// BufferPtrResource class
	////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	class BufferPtrResource
		: public BaseBufferResource {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		BufferPtrResource() = delete;

		//! @breif コンストラクタ
		//! 
		//! @param[in] devices   DxObject::Devices
		//! @param[in] indexSize 配列サイズ
		BufferPtrResource(Devices* devices, uint32_t indexSize)
			: BaseBufferResource(indexSize, sizeof(T)) {
			Init(devices);
		}

		//! @brief デストラクタ
		~BufferPtrResource() override { Term(); }

		//! @brief 初期化処理
		//! 
		//! @param[in] devices DxObject::Devices
		void Init(Devices* devices);

		//! @brief 終了処理
		void Term();

		//! @brief dataPtrArrayにvalueを設定
		//! 
		//! @param[in] element 要素数
		//! @param[in] value データ
		void SetPtr(uint32_t element, T* value) {
			if (!CheckElementCount(element)) {
				assert(false); //!< 範囲外参照
				return;
			}

			dataPtrArray_.at(element) = value;
		}

		const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() override {
			LoadPtrData();
			return resource_->GetGPUVirtualAddress();
		}

		//! @brief VertexBufferを取得
		//! 
		//! @return VertexBufferを返却
		const D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() {
			D3D12_VERTEX_BUFFER_VIEW result = {};
			result.BufferLocation = GetGPUVirtualAddress();
			result.SizeInBytes    = structureSize_ * indexSize_;
			result.StrideInBytes  = structureSize_;

			return result;
		}

		//! @brief Resourceの取得
		virtual ID3D12Resource* GetResource() override {
			LoadPtrData();
			return resource_.Get();
		}

		//
		// Test functions
		//

		const T* GetData() {
			LoadPtrData();
			return mappedDatas_.data();
		}

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		std::span<T>    mappedDatas_;
		std::vector<T*> dataPtrArray_;

		//=========================================================================================
		// private methods
		//=========================================================================================

		void LoadPtrData();

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// IndexBufferResource class
	////////////////////////////////////////////////////////////////////////////////////////////
	class IndexBufferResource
		: public BaseBufferResource {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		IndexBufferResource() = delete;

		//! @breif コンストラクタ
		//! 
		//! @param[in] devices   DxObject::Devices
		//! @param[in] indexSize 配列サイズ
		IndexBufferResource(Devices* devices, uint32_t indexSize)
			: BaseBufferResource(indexSize, sizeof(uint32_t)) {
			Init(devices);
		}

		//! @brief デストラクタ
		~IndexBufferResource() override { Term(); }

		//! @brief 初期化処理
		//! 
		//! @param[in] devices DxObject::Devices
		void Init(Devices* devices);

		//! @brief 終了処理
		void Term();

		//! @brief IndexBufferを取得
		//! 
		//! @return IndexBufferを返却
		const D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const {
			D3D12_INDEX_BUFFER_VIEW result = {};
			result.BufferLocation = resource_->GetGPUVirtualAddress();
			result.SizeInBytes    = structureSize_ * indexSize_;
			result.Format         = DXGI_FORMAT_R32_UINT;

			return result;
		}

		void Memcpy(const uint32_t* value) {
			std::memcpy(mappedDatas_.data(), value, sizeof(uint32_t) * indexSize_);
		}

		//=========================================================================================
		// operator
		//=========================================================================================

		uint32_t& operator[](uint32_t element) {
			CheckElementCount(element);

			return mappedDatas_[element];
		}

		//
		// Test funcitons
		//

		uint32_t* GetData() const {
			return mappedDatas_.data();
		}

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		std::span<uint32_t> mappedDatas_;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// DynamicBufferResource class
	////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	class DynamicBufferResource
		: public BaseBufferResource { //!< テスト段階なのでstructuredBufferなどとして使わないように
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		DynamicBufferResource() = delete;

		//! @breif コンストラクタ
		//! 
		//! @param[in] devices DxObject::Devices
		DynamicBufferResource(Devices* devices)
			: BaseBufferResource(0, sizeof(T)) {
			Init(devices);
		}

		//! @brief デストラクタ
		~DynamicBufferResource() override { Term(); }

		//! @brief 初期化処理
		//! 
		//! @param[in] devices DxObject::Devices
		void Init(Devices* devices);

		//! @brief 終了処理
		void Term();

		//! @brief 要素の全削除
		void Clear();

		//! @brief 配列の最大数を返却
		const uint32_t GetIndexSize() const override { return kMaxIndexSize_; }

		const uint32_t GetCurrentIndexSize() const { return indexSize_; }

		void Memcpy(const T* value, uint32_t indexSize) {
			assert(CheckElementCount(indexSize));
			std::memcpy(dataArray_, value, structureSize_ * indexSize);
		}

		//=========================================================================================
		// operator
		//=========================================================================================

		T& operator[](uint32_t element) {
			assert(CheckElementCount(element));
			return dataArray_[element];
		}

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		static const uint32_t kMaxIndexSize_ = 200; //!< 不足した場合, 増やす

		T* dataArray_;

		/* 用途の変更
		 indexSize -> 現在のindexの最大値
		*/

		ID3D12Device* device_; //!< 完全な動的bufferにする場合, 必須

		//=========================================================================================
		// private methods
		//=========================================================================================

		//! @brief 要素数がindexSize以上でないかの確認
		//! 
		//! @retval true  配列サイズ以下
		//! @retval false 配列サイズ以上
		bool CheckElementCount(uint32_t elementCount) ;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// OffsetBufferResource class
	////////////////////////////////////////////////////////////////////////////////////////////
	class OffsetBufferResource
		: public BaseBufferResource {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		OffsetBufferResource() = delete;

		//! @breif コンストラクタ
		//! 
		//! @param[in] devices DxObject::Devices
		OffsetBufferResource(Devices* devices, UINT structureSize)
			: BaseBufferResource(0, structureSize) {
			Init(devices);
		}

		~OffsetBufferResource() override { Term(); }

		//! @brief 初期化処理
		//! 
		//! @param[in] devices DxObject::Devices
		void Init(Devices* devices);

		//! @brief 終了処理
		void Term();

		//* setter *//

		template <typename T>
		void SetValue(UINT offset, const T& value);

		template <typename T>
		const T& GetValue(UINT offset);

		/*template <typename T>
		T& AccessValue(UINT offset);*/

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		BYTE* mappedDatas_ = nullptr; //!< byte数で考えるのでBYTEを使用

	};

	

}

////////////////////////////////////////////////////////////////////////////////////////////
// BufferResource class methods
////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline void DxObject::BufferResource<T>::Init(Devices* devices) {

	// deviceを取り出す
	ID3D12Device* device = devices->GetDevice();

	// 配列分のBufferResourceを生成
	resource_ = DxObjectMethod::CreateBufferResource(
		device,
		structureSize_ * indexSize_
	);
	
	T* mappingTarget = nullptr;

	// resourceをマッピング
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappingTarget));

	mappedDatas_ = { mappingTarget, indexSize_ };
}

template<typename T>
inline void DxObject::BufferResource<T>::Term() {
}

////////////////////////////////////////////////////////////////////////////////////////////
// BufferPtrResource class methods
////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline void DxObject::BufferPtrResource<T>::Init(Devices* devices) {

	// deviceを取り出す
	ID3D12Device* device = devices->GetDevice();

	dataPtrArray_.resize(indexSize_);

	// 配列分のBufferResourceを生成
	resource_ = DxObjectMethod::CreateBufferResource(
		device,
		structureSize_ * indexSize_
	);

	T* mappingTarget = nullptr;

	// resourceをマッピング
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappingTarget));

	mappedDatas_ = { mappingTarget, indexSize_ };
}

template<typename T>
inline void DxObject::BufferPtrResource<T>::Term() {
	dataPtrArray_.clear();
}

template<typename T>
inline void DxObject::BufferPtrResource<T>::LoadPtrData() {
	memcpy(mappedDatas_.data(), *dataPtrArray_.data(), structureSize_ * indexSize_);
}

////////////////////////////////////////////////////////////////////////////////////////////
// DynamicBufferResource class methods
////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline void DxObject::DynamicBufferResource<T>::Init(Devices* devices) {

	// deviceを取り出す
	device_ = devices->GetDevice();

	// 配列分のBufferResourceを生成
	resource_ = DxObjectMethod::CreateBufferResource(
		device_,
		structureSize_ * kMaxIndexSize_
	);

	// resourceをマッピング
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&dataArray_));

	Clear();
}

template<typename T>
inline void DxObject::DynamicBufferResource<T>::Term() {
}

template<typename T>
inline void DxObject::DynamicBufferResource<T>::Clear() {
	T t;
	std::fill(dataArray_, dataArray_ + kMaxIndexSize_, t);
	indexSize_ = 0;
}

template<typename T>
inline bool DxObject::DynamicBufferResource<T>::CheckElementCount(uint32_t elementCount) {
	if (elementCount >= kMaxIndexSize_) {
		return false;
	}

	// 最大数の更新
	if (elementCount >= indexSize_) {
		indexSize_ = elementCount + 1;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
// OffsetBufferResource class
////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void DxObject::OffsetBufferResource::SetValue(UINT offset, const T& value) {
	assert(offset + static_cast<UINT>(sizeof(T)) <= structureSize_); //!< 範囲外への書き込み

	*reinterpret_cast<T*>(mappedDatas_ + offset) = value;
}

template<typename T>
inline const T& DxObject::OffsetBufferResource::GetValue(UINT offset) {
	assert(offset + static_cast<UINT>(sizeof(T)) <= structureSize_); //!< 範囲外への書き込み

	return *reinterpret_cast<T*>(mappedDatas_ + offset);
}