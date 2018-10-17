#pragma once
class CameraProgram
{
public:
	CameraProgram();
	~CameraProgram();

	void Set
	(
		glm::vec3 startP, glm::vec3 endP, float speedP,
		glm::vec3 startR, glm::vec3 endR, float speedR
	);

	bool Update(float dt, glm::vec3& camPos, glm::vec3& camDir);

	bool IsSet() { return bIsSet; }

private:
	glm::vec3 startP = glm::vec3(-30.0f, 3.0f, 3.0f);
	glm::vec3 endP = glm::vec3(15.0f, 8.0f, 1.0f);
	float speedP = 0.005f;
	glm::vec3 startR = glm::vec3(20.0f, 0.0f, -90.0f);
	glm::vec3 endR = glm::vec3(-20.0f, 0.0f, 75.0f);
	float speedR = 0.0185f;
	float elapsed = 0.0f;
	float expectedDurationP = glm::length(endP - startP) / speedP;
	float expectedDurationR = glm::length(endR - startR) / speedR;

	bool bIsSet;
};


class ObjectPositionAnimation
{
public:
	ObjectPositionAnimation();
	~ObjectPositionAnimation();

	void SetTicksPerSecond(float ticks);
	void AddKeyframe(float time, glm::mat4 TRS);

	void Reset();

	glm::mat4 Update(float dt);

private:
	struct Keyframe
	{
		glm::mat4 TRS;
		float time;
	};

	std::vector<Keyframe> keyframes;

	float currentTime;
	float ticksPerSeconds;
	int currentKeyframe;
};
