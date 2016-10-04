#pragma once

#include <glad/glad.h>

namespace oak::graphics {

	using MaterialId = uint32_t;

	template<typename TPipeline, typename TTexture>
	class Material {
	public:
		Material(const TPipeline &pipeline, const TTexture &texture, MaterialId id) : pipeline_{ pipeline }, texture_{ texture }, id_{ id } {}

		inline void bind() { pipeline_.bind(); texture_.bind(GL_TEXTURE0); }
		inline void unbind() { pipeline_.unbind(); texture_.unbind(); }

		const MaterialId& getId() const { return id_; }
	private:
		TPipeline pipeline_;
		TTexture texture_;
		MaterialId id_;
	};

}