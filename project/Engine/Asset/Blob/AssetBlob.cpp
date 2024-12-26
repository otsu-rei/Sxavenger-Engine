#include "AssetBlob.h"

void AssetBlob::Load(_MAYBE_UNUSED const DirectXThreadContext* context) {
	if (!BaseAsset::CheckAndBeginLoad()) {
		return;
	}

	blob_.Create(filepath_, profile_);

	BaseAsset::EndLoad();
	BaseAsset::Complete();
}
