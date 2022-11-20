#pragma once

#include <vector>
#include <map>

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include "vk_logger.h"
#include "vk_debug_wrapper.h"
#include "vk_device_wrapper.h"
#include "vk_surface_wrapper.h"
#include "vk_validation_wrapper.h"
#include "vk_swap_chain_wrapper.h"
#include "vk_pipeline_wrapper.h"
#include "vk_cmd_wrapper.h"
#include "vk_sync_wrapper.h"

/*
* Naming Conventions =>
* vk = Vulkan class
* _vk prefix = Vulkan wrapper class (classes in this API)
* camelCase
*/

class VulkanAPI {

	public:

		VulkanAPI(GLFWwindow& _window);
		~VulkanAPI();

		void onNewFrame();

		void init();

	private:

		GLFWwindow& glfwWindow;
		VkInstance vkInstance{};

		_VkValidation _vkValidation{};
		VkDebugWrapper* _vkDebugWrapper = nullptr;
		_VkLogger& _vkLogger;

		_VkSurface* _vkSurface = nullptr;

		_VkDevice* _vkDevice = nullptr;
		_VkSwapchain* _vkSwapchain = nullptr;
		_VkPipeline* _vkPipeline = nullptr;
		
		_VkCmdPool* _vkCmdPool = nullptr;
		_VkCmdBuffer* _vkCmdBuffer = nullptr;

		_VkRenderSync* _vkRenderSync = nullptr;

		void initInstance();
		void initSurface();
		void initDevice();
		void initSwapChain();
		void initPipeline();
		void initCmdWrapper();
		void initSync();

		std::vector<const char*> getRequiredExtensions();

};