#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>
#include "imgui.h"
#include"imgui_impl_glfw_vulkan.h"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <chrono>
#include <array>

#include "Timer.h"

#include "VulkanHelpers.h"

#define NONCOPYABLE(classname) 	public: \
		classname(const classname& other) = delete; \
		void operator=(const classname& other) = delete;