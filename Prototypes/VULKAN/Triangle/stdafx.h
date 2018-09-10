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
#include <map>
#include <unordered_map>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <chrono>
#include <array>

#include "VulkanHelpers.h"

#define NONCOPYABLE(classname) 	public: \
		classname(const classname& other) = delete; \
		void operator=(const classname& other) = delete;