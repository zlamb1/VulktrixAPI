#pragma once

#include <fstream>
#include <iostream>

#include <shaderc/shaderc.hpp>
#include <shaderc/shaderc.h>
#include <vulkan/vulkan.h>

#include "vk_device_wrapper.h"
#include "vk_swap_chain_wrapper.h"
#include "vk_shader.h"

struct _VkShaderPipeline : VulkanWrapper  {

	_VkShaderPipeline();
	~_VkShaderPipeline();

	_VkDevice* _pDevice = nullptr;

	std::vector<_VkShader*> _vkShaders{};
	
	VkResult create();

	void createShader(const char* shaderCode, shaderc_shader_kind shaderType);

	std::vector<VkPipelineShaderStageCreateInfo> getShaderStages();

};

struct _VkFixedFunctionState : VulkanWrapper {
	
	_VkFixedFunctionState();
	~_VkFixedFunctionState();

	_VkDevice* _pDevice = nullptr;
	_VkSwapchain* _pSwapchain = nullptr;

	VkPipelineDynamicStateCreateInfo vkDynamicState{};
	std::vector<VkDynamicState> vkDynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineVertexInputStateCreateInfo vkVertexInputState{};
	VkPipelineInputAssemblyStateCreateInfo vkInputAssemblyState{};
	VkViewport vkViewport{};
	VkRect2D vkRect2D{};
	VkPipelineViewportStateCreateInfo vkViewportState{};
	VkPipelineRasterizationStateCreateInfo vkRasterizerState{};
	VkPipelineMultisampleStateCreateInfo vkMultisamplingState{};
	VkPipelineDepthStencilStateCreateInfo vkDepthStencilState{};
	VkPipelineColorBlendAttachmentState vkColorBlendAttachmentState{};
	VkPipelineColorBlendStateCreateInfo vkColorBlendingState{};
	VkPipelineLayoutCreateInfo vkPipelineLayoutInfo{};

	VkPipelineLayout vkPipelineLayout{};

	VkResult create();

};

struct _VkRenderPass : VulkanWrapper {

	_VkRenderPass();
	~_VkRenderPass();

	_VkDevice* _pDevice = nullptr;
	_VkSwapchain* _pSwapchain = nullptr;

	VkRenderPass vkRenderPass{};

	VkResult create();

};