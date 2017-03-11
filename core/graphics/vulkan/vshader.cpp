#include "vshader.h"

#include "container.h"
#include "util/file_util.h"
#include "log.h"

namespace oak::graphics {

	VShader::VShader(const VDevice *device, VkShaderStageFlagBits stage) : device_{ device }, module_{ VK_NULL_HANDLE }, stage_{ stage } {

	}

	VShader::~VShader() {
		destroy();
	}

	VShader::VShader(VShader &&other) : device_{ other.device_ }, module_{ other.module_ }, stage_{ other.stage_ } {
		other.module_ = VK_NULL_HANDLE;
	}

	void VShader::load(const oak::string &path) {
		//if the shader is already loaded do not attemp to load another
		if (module_ != VK_NULL_HANDLE) { return; }
		//read code from file
		const oak::vector<char> code = util::readFile(path);

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkResult result = vkCreateShaderModule(*device_, &createInfo, nullptr, &module_);
		if (result != VK_SUCCESS) {
			log::cout << "failed to create shader module: " << path << std::endl;
		}
	} 

	void VShader::destroy() {
		if (module_ != VK_NULL_HANDLE) {
			vkDestroyShaderModule(*device_, module_, nullptr);
			module_ = VK_NULL_HANDLE;
		}
	}

	VShader::operator const VkPipelineShaderStageCreateInfo() const {
		VkPipelineShaderStageCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		info.stage = stage_;
		info.module = module_;
		info.pName = "main";

		return info;
	}


}