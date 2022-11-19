#include "vk_pipeline_wrapper.h"

// Public

VkPipelineWrapper::VkPipelineWrapper(_VkDevice& _vkDevice,
	_VkSwapchain& _vkSwapchain) :
	_vkLogger{ _VkLogger::Instance() }, 
	_vkDevice( _vkDevice ),
	_vkSwapchain{ _vkSwapchain }
{
	initGraphicsPipeline();
	initFixedFunctionState();
	initRenderPass();
}

VkPipelineWrapper::~VkPipelineWrapper() {
	vkDestroyPipeline(_vkDevice.vkDevice, vkPipeline, nullptr);
}

void VkPipelineWrapper::newFrame(_VkCmdBuffer& vkCmdBuffer, 
	uint32_t imageIndex) {
	vkCmdBuffer.resetCmdBuffer();
	vkCmdBuffer.recordCmdBuffer();

	auto vkSwapchainExtent = _vkSwapchain.vkSwapchainExtent;

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = vkRenderPassWrapper->getRenderPass();
	renderPassInfo.framebuffer = 
		vkFramebuffer->vkFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = vkSwapchainExtent;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(vkCmdBuffer.vkCmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(vkCmdBuffer.vkCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)vkSwapchainExtent.width;
	viewport.height = (float)vkSwapchainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(vkCmdBuffer.vkCmdBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = vkSwapchainExtent;
	vkCmdSetScissor(vkCmdBuffer.vkCmdBuffer, 0, 1, &scissor);

	vkCmdDraw(vkCmdBuffer.vkCmdBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(vkCmdBuffer.vkCmdBuffer);

	auto vkEndCommandBufferResult = vkEndCommandBuffer(vkCmdBuffer.vkCmdBuffer);
	_vkLogger.LogResult("vkEndCommandBuffer =>", vkEndCommandBufferResult);
}

void VkPipelineWrapper::init() {

	VkGraphicsPipelineCreateInfo vkPipelineInfo{};
	vkPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	vkPipelineInfo.stageCount = 2;

	auto shaderStages = vkGraphicsPipeline->getShaderStages();
	vkPipelineInfo.pStages = shaderStages.data();

	vkPipelineInfo.pVertexInputState = &vkFixedFunctionWrapper->getVertexInputState();
	vkPipelineInfo.pInputAssemblyState = &vkFixedFunctionWrapper->getInputAssemblyState();
	vkPipelineInfo.pViewportState = &vkFixedFunctionWrapper->getViewportState();
	vkPipelineInfo.pRasterizationState = &vkFixedFunctionWrapper->getRasterizerState();
	vkPipelineInfo.pMultisampleState = &vkFixedFunctionWrapper->getMultisamplingState();
	vkPipelineInfo.pDepthStencilState = &vkFixedFunctionWrapper->getDepthStencilState(); // Optional
	vkPipelineInfo.pColorBlendState = &vkFixedFunctionWrapper->getColorBlendState();
	vkPipelineInfo.pDynamicState = &vkFixedFunctionWrapper->getDynamicState();

	vkPipelineInfo.layout = vkFixedFunctionWrapper->getPipelineLayout();

	vkPipelineInfo.renderPass = vkRenderPassWrapper->getRenderPass();
	vkPipelineInfo.subpass = 0;

	vkPipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // optional
	vkPipelineInfo.basePipelineIndex = -1; // optional

	auto vkCreateGraphicsPipelineResult = vkCreateGraphicsPipelines(_vkDevice.vkDevice,
		VK_NULL_HANDLE, 1, &vkPipelineInfo, nullptr,
		&vkPipeline);
	_vkLogger.LogResult("vkCreateGraphicsPipeline =>", vkCreateGraphicsPipelineResult);

	// initialize frame buffers after init
	initFramebuffers();
}

// Private

void VkPipelineWrapper::initGraphicsPipeline() {
	vkGraphicsPipeline = std::make_unique<VkGraphicsPipeline>(_vkDevice);
	vkGraphicsPipeline->init();
}

void VkPipelineWrapper::initFixedFunctionState() {
	vkFixedFunctionWrapper = std::make_unique<VkFixedFunctionWrapper>(
		_vkDevice, _vkSwapchain);
	vkFixedFunctionWrapper->init();
}

void VkPipelineWrapper::initRenderPass() {
	vkRenderPassWrapper = std::make_unique<VkRenderPassWrapper>(
		_vkDevice, _vkSwapchain);
	vkRenderPassWrapper->init();
}

void VkPipelineWrapper::initFramebuffers() {
	vkFramebuffer = std::make_unique<_VkFramebuffer>();

	vkFramebuffer->_pSwapchain = &_vkSwapchain;
	vkFramebuffer->pDevice = &_vkDevice.vkDevice;
	vkFramebuffer->pRenderPass = &vkRenderPassWrapper->getRenderPass();

	vkFramebuffer->create();
}