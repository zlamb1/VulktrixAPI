#pragma once

#include <stdint.h>

#include "Magma/Buffer/buffer.h"

// TODO: abstract this
#include "Magma/Vulkan/Memory/descriptor.h"

namespace Magma {

	enum class VertexInputRate {
		VERTEX, INSTANCE
	};

	enum class DataFormat {
		R_SFLOAT32, RG_SFLOAT32, RGB_SFLOAT32, RGBA_SFLOAT32
	};

	struct VertexBinding {
		uint32_t binding, stride;
		VertexInputRate inputRate;
	};

	struct VertexAttribute {
		uint32_t binding, location, offset;
		DataFormat format; 
	};

	class ShaderAttributes {

	public:
		virtual VertexBinding createVertexBinding(uint32_t binding = 0,
			uint32_t stride = 0,
			VertexInputRate inputRate = VertexInputRate::VERTEX) = 0;
		virtual VertexAttribute createVertexAttribute(uint32_t binding = 0,
			uint32_t location = 0,
			uint32_t offset = 0,
			DataFormat format = DataFormat::RGB_SFLOAT32) = 0;

		virtual Descriptor createDescriptor(uint32_t pBinding = 0, uint32_t pCount = 1,
			VulkanShaderType pStageFlags = VulkanShaderType::VERTEX) = 0;
		virtual std::shared_ptr<DescriptorSetLayout> createDescriptorSetLayout() = 0;
		virtual std::shared_ptr<DescriptorSet> createDescriptorSet(Buffer& pBuffer,
			uint32_t pMaxSets = 1, VkDeviceSize pSize = 0) = 0;

	};

}