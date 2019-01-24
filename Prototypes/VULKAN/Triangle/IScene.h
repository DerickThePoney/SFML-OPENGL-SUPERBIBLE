#pragma once
class VulkanCommandBuffer;
class Frustum;

class IScene
{
public:
	virtual void Initialise() {}
	virtual void InitialiseFromFile(const std::string& filename) = 0;
	virtual void Destroy() = 0;
	virtual void Draw(VulkanCommandBuffer& buffer, const Frustum& frustum) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void DrawOverlays() = 0;
};