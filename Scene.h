#pragma once
#include "GameObject.h"
#include "Camera.h"

class Scene
{
public:
	Scene();
	~Scene();

	void Initialise();
	void Terminate();

	void Update(double deltaTime);

	void OnResize(unsigned int width, unsigned int height);

	//Scene API
	Camera& GetCamera() { return m_kCamera; }
	void AddRootObject(GameObject* kObject) { m_akRootObjects.push_back(kObject); }

	void ExtractVisibleObjectList(std::vector<GameObject *>& akObjects);

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(CEREAL_NVP(m_kCamera));

		for (size_t i = 0; i < m_akRootObjects.size(); ++i)
		{
			std::stringstream sstr;
			sstr << "GameObject_" << i;
			archive(cereal::make_nvp(sstr.str(), (*m_akRootObjects[i])));
		}
	}


private:
	void ExtractVisibleObjectsInHierarchy(GameObject* base, std::vector<GameObject *>& akObjects);

private:

	
	Camera m_kCamera;
	std::vector<GameObject*> m_akRootObjects;

	OGLTexture2D m_kTexture;
};

