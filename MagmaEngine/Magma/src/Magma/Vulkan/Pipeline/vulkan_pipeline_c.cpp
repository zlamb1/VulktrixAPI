#include "vulkan_pipeline_c.h"

namespace Magma {

	std::vector<VkPipelineShaderStageCreateInfo> VulkanShaderPipeline::getShaderStages() {
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
		for (auto pVulkanShader : pVulkanShaders) {
			if (pVulkanShader != nullptr) {
				shaderStages.push_back(pVulkanShader->getShaderStageCreateInfo());
			}
		}

		return shaderStages;
	}

	void VulkanShaderPipeline::addShader(std::shared_ptr<MagmaShader> pVulkanShader) {
		pVulkanShaders.push_back(pVulkanShader);
	}

	VkResult VulkanShaderPipeline::init() {
		return VK_SUCCESS;
	}

	VulkanFixedFunctionState::VulkanFixedFunctionState(
		ShaderAttributes& pShaderAttributes) :
		pShaderAttributes{ pShaderAttributes } {}

	VulkanFixedFunctionState::~VulkanFixedFunctionState() {
		if (pVulkanDevice != nullptr) {
			vkDestroyPipelineLayout(pVulkanDevice->getDevice(), vkPipelineLayout, nullptr);
		}
	}

	VkResult VulkanFixedFunctionState::init() {
		if (pVulkanDevice == nullptr) {
			Z_LOG_TXT("VulkanFixedFunctionState::init", "pVulkanDevice is nullptr");
			return VK_ERROR_INITIALIZATION_FAILED;
		}

		if (pVulkanSwapchain == nullptr) {
			Z_LOG_TXT("VulkanFixedFunctionState::init", "pVulkanSwapchain is nullptr");
			return VK_ERROR_INITIALIZATION_FAILED;
		}

		vkDynamicCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		vkDynamicCreateInfo.dynamicStateCount = static_cast<uint32_t>(vkDynamicStates.size());
		vkDynamicCreateInfo.pDynamicStates = vkDynamicStates.data();

		vkVertexInputCreateInfo.sType =
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		vkVertexInputCreateInfo.vertexBindingDescriptionCount =
			static_cast<uint32_t>(pShaderAttributes.getVertexBindings().size());
		vkVertexInputCreateInfo.pVertexBindingDescriptions =
			pShaderAttributes.getVertexBindings().data();

		vkVertexInputCreateInfo.vertexAttributeDescriptionCount =
			static_cast<uint32_t>(pShaderAttributes.getVertexAttributes().size());
		vkVertexInputCreateInfo.pVertexAttributeDescriptions =
			pShaderAttributes.getVertexAttributes().data();

		vkInputAssemblyCreateInfo.sType =
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		vkInputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		vkInputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

		auto vkSwapchainExtent = pVulkanSwapchain->getSwapchainExtent();
		vkViewport.x = 0.0f;
		vkViewport.y = 0.0f;
		vkViewport.width = (float)vkSwapchainExtent.width;
		vkViewport.height = (float)vkSwapchainExtent.height;
		vkViewport.minDepth = 0.0f;
		vkViewport.maxDepth = 1.0f;

		vkScissor.offset = { 0, 0 };
		vkScissor.extent = vkSwapchainExtent;

		vkViewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		vkViewportCreateInfo.viewportCount = 1;
		vkViewportCreateInfo.pViewports = &vkViewport;
		vkViewportCreateInfo.scissorCount = 1;
		vkViewportCreateInfo.pScissors = &vkScissor;

		vkRasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		vkRasterizationCreateInfo.depthClampEnable = VK_FALSE;
		vkRasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		vkRasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		vkRasterizationCreateInfo.lineWidth = 1.0f;

		// face culling
		vkRasterizationCreateInfo.cullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
		vkRasterizationCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

		vkRasterizationCreateInfo.depthBiasEnable = VK_FALSE;
		vkRasterizationCreateInfo.depthBiasConstantFactor = 0.0f; // optional
		vkRasterizationCreateInfo.depthBiasClamp = 0.0f; // optional
		vkRasterizationCreateInfo.depthBiasSlopeFactor = 0.0f; // optional


		vkMultisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		vkMultisampleCreateInfo.sampleShadingEnable = VK_FALSE;
		vkMultisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		vkMultisampleCreateInfo.minSampleShading = 1.0f; // optional
		vkMultisampleCreateInfo.pSampleMask = nullptr; // optional
		vkMultisampleCreateInfo.alphaToCoverageEnable = VK_FALSE; // optional
		vkMultisampleCreateInfo.alphaToOneEnable = VK_FALSE; // optional

		vkColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
			| VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		vkColorBlendAttachmentState.blendEnable = VK_FALSE;
		vkColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // optional
		vkColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // optional
		vkColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD; // optional
		vkColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // optional
		vkColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // optional
		vkColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD; // optional

		vkColorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		vkColorBlendCreateInfo.logicOpEnable = VK_FALSE;
		vkColorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY; // optional
		vkColorBlendCreateInfo.attachmentCount = 1;
		vkColorBlendCreateInfo.pAttachments = &vkColorBlendAttachmentState;
		vkColorBlendCreateInfo.blendConstants[0] = 0.0f; // optional
		vkColorBlendCreateInfo.blendConstants[1] = 0.0f; // optional
		vkColorBlendCreateInfo.blendConstants[2] = 0.0f; // optional
		vkColorBlendCreateInfo.blendConstants[3] = 0.0f; // optional

		vkPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		auto& descriptorSetLayouts = pShaderAttributes.getDescriptorSetLayouts();
		if (descriptorSetLayouts.size() == 0) {
			vkPipelineLayoutCreateInfo.setLayoutCount = 0;
			vkPipelineLayoutCreateInfo.pSetLayouts = nullptr;
		}
		else {
			vkPipelineLayoutCreateInfo.setLayoutCount = descriptorSetLayouts.size();
			vkPipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
		}

		vkPipelineLayoutCreateInfo.pushConstantRangeCount = 0; // optional
		vkPipelineLayoutCreateInfo.pPushConstantRanges = nullptr; // optional

		auto createPipelineLayoutResult = vkCreatePipelineLayout(pVulkanDevice->getDevice(),
			&vkPipelineLayoutCreateInfo, nullptr, &vkPipelineLayout);
		Z_LOG_OBJ("VulkanFixedFunctionState::init::vkCreatePipelineLayoutResult", createPipelineLayoutResult);

		return createPipelineLayoutResult;
	}

	VkPipelineDynamicStateCreateInfo& VulkanFixedFunctionState::getDynamicCreateInfo() {
		return vkDynamicCreateInfo;
	}

	VkPipelineVertexInputStateCreateInfo& VulkanFixedFunctionState::getVertexInputCreateInfo() {
		return vkVertexInputCreateInfo;
	}

	VkPipelineInputAssemblyStateCreateInfo& VulkanFixedFunctionState::getInputAssemblyCreateInfo() {
		return vkInputAssemblyCreateInfo;
	}

	VkViewport& VulkanFixedFunctionState::getViewport() {
		return vkViewport;
	}

	VkRect2D& VulkanFixedFunctionState::getScissor() {
		return vkScissor;
	}

	VkPipelineViewportStateCreateInfo& VulkanFixedFunctionState::getViewportCreateInfo() {
		return vkViewportCreateInfo;
	}

	VkPipelineRasterizationStateCreateInfo& VulkanFixedFunctionState::getRasterizationCreateInfo() {
		return vkRasterizationCreateInfo;
	}

	VkPipelineMultisampleStateCreateInfo& VulkanFixedFunctionState::getMultisampleCreateInfo() {
		return vkMultisampleCreateInfo;
	}

	VkPipelineDepthStencilStateCreateInfo& VulkanFixedFunctionState::getDepthStencilCreateInfo() {
		return vkDepthStencilCreateInfo;
	}

	VkPipelineColorBlendAttachmentState& VulkanFixedFunctionState::getColorBlendAttachmentState() {
		return vkColorBlendAttachmentState;
	}

	VkPipelineColorBlendStateCreateInfo& VulkanFixedFunctionState::getColorBlendCreateInfo() {
		return vkColorBlendCreateInfo;
	}

	VkPipelineLayoutCreateInfo& VulkanFixedFunctionState::getPipelineLayoutCreateInfo() {
		return vkPipelineLayoutCreateInfo;
	}

	VkPipelineLayout& VulkanFixedFunctionState::getPipelineLayout() {
		return vkPipelineLayout;
	}

	VulkanRenderPass::VulkanRenderPass(std::shared_ptr<VulkanDevice> pVulkanDevice,
		std::shared_ptr<VulkanSwapchain> pVulkanSwapchain) : pVulkanDevice{ pVulkanDevice },
		pVulkanSwapchain{ pVulkanSwapchain } {}

	VulkanRenderPass::~VulkanRenderPass() {
		if (pVulkanDevice != nullptr) {
			vkDestroyRenderPass(pVulkanDevice->getDevice(), vkRenderPass, nullptr);
		}
	}

	VkResult VulkanRenderPass::init() {
		if (pVulkanDevice == nullptr) {
			Z_LOG_TXT("VulkanRenderPass::init", "pDevice is nullptr");
			return VK_ERROR_INITIALIZATION_FAILED;
		}

		if (pVulkanSwapchain == nullptr) {
			Z_LOG_TXT("VulkanRenderPass::init", "pSwapchain is nullptr");
			return VK_ERROR_INITIALIZATION_FAILED;
		}

		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = pVulkanSwapchain->getSwapchainImageFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		auto createRenderPassResult = vkCreateRenderPass(pVulkanDevice->getDevice(), &renderPassInfo,
			nullptr, &vkRenderPass);
		Z_LOG_OBJ("VulkanRenderPass::init::vkCreateRenderPass", createRenderPassResult);

		return createRenderPassResult;
	}

	VkRenderPass& VulkanRenderPass::getRenderPass() {
		return vkRenderPass;
	}

}