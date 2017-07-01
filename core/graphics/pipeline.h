#pragma once

#include <glm/glm.hpp>
#include "container.h"

#include "batch.h"
#include "framebuffer.h"

namespace oak::graphics {

	enum class PipelineStageType {
		CLEAR,
		SWAP,
		DRAW,
		BIND_FRAMEBUFFER
	};

	struct PipelineStage {
		PipelineStageType type;
	};

	struct PipelineStageClear : public PipelineStage {

		PipelineStageClear() : PipelineStage{ PipelineStageType::CLEAR } {}

		constexpr static uint32_t COLOR_BIT = 0x01;
		constexpr static uint32_t DEPTH_BIT = 0x02;
		
		glm::vec4 color;
		float depth;
		uint32_t flags;
	};

	struct PipelineStageSwap : public PipelineStage {
		PipelineStageSwap() : PipelineStage{ PipelineStageType::SWAP } {}
	};

	struct PipelineStageDraw : public PipelineStage {
		PipelineStageDraw() : PipelineStage{ PipelineStageType::DRAW } {}
		oak::vector<std::pair<uint32_t, Batch>> batches; //list of batches with the layer they are apart of
	};

	struct PipelineStageBindFramebuffer : public PipelineStage {
		PipelineStageBindFramebuffer() : PipelineStage{ PipelineStageType::BIND_FRAMEBUFFER } {}
		Framebuffer *framebuffer;
	};

	class Pipeline {
	public:
		void setup(); //setup pipeline

		oak::vector<PipelineStage*> stages;
	};

}