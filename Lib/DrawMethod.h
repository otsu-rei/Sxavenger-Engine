#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cmath>
#include <numbers>
#include <memory>

// DxObject
#include <DxBufferResource.h>

// structure
#include <ObjectStructure.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DrawData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct DrawData {
	std::unique_ptr<DxObject::BufferResource<VertexData>> vertex;
	std::unique_ptr<DxObject::IndexBufferResource>        index;

	void Reset() {
		vertex.reset();
		index.reset();
	}

	void SetBuffer(ID3D12GraphicsCommandList* commandList) const {
		D3D12_VERTEX_BUFFER_VIEW vertexBuffer = vertex->GetVertexBufferView();
		D3D12_INDEX_BUFFER_VIEW indexBuffer   = index->GetIndexBufferView();

		commandList->IASetVertexBuffers(0, 1, &vertexBuffer);
		commandList->IASetIndexBuffer(&indexBuffer);
	}

	void DrawCall(ID3D12GraphicsCommandList* commandList) const {
		commandList->DrawIndexedInstanced(index->GetIndexSize(), 1, 0, 0, 0);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// DrawMethod namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DrawMethods {

	DrawData Sphere(float radius, uint32_t kSubdivision);
	DrawData Plane(const Vector2f& size);

	DrawData Plane2D();
};