#include "stdafx.h"
#include "CameraProgram.h"


CameraProgram::CameraProgram()
	: bIsSet(false)
{
}


CameraProgram::~CameraProgram()
{
}

void CameraProgram::Set(glm::vec3 startP, glm::vec3 endP, float speedP, glm::vec3 startR, glm::vec3 endR, float speedR)
{
	this->startP = startP;
	this->endP = endP;
	this->speedP = speedP;
	this->startR = startR;
	this->endR = endR;
	this->speedR = speedR;

	elapsed = 0.0f;
	expectedDurationP = glm::length(endP - startP) / speedP;
	expectedDurationR = glm::length(endR - startR) / speedR;
	bIsSet = true;
}

bool CameraProgram::Update(float dt, glm::vec3 & camPos, glm::vec3 & camDir)
{
	elapsed += dt;
	float interpP = glm::clamp(elapsed / expectedDurationP, 0.0f, 1.0f);
	float interpR = glm::clamp(elapsed / expectedDurationR, 0.0f, 1.0f);

	camPos = (1 - interpP) * startP + interpP * endP;
	glm::vec3 camO = glm::radians((1 - interpR) * startR + interpR * endR);

	//vDir->X = sin(yaw);
	//vDir->Y = -(sin(pitch)*cos(yaw));
	//vDir->Z = -(cos(pitch)*cos(yaw));
	/*camDir.x = -cos(camO.x)*sin(camO.z)*sin(camO.y) - sin(camO.x)*cos(camO.y);
	camDir.y = -sin(camO.x)*sin(camO.z)*sin(camO.y) + cos(camO.x)*cos(camO.y);
	camDir.z = cos(camO.z)*sin(camO.y);*/

	camDir.x = sin(camO.x);
	camDir.y = -(sin(camO.y) * cos(camO.x));
	camDir.z = cos(camO.y)*cos(camO.x);


	if (interpP >= 1.0f && interpR >= 1.0f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

ObjectPositionAnimation::ObjectPositionAnimation()
{
}

ObjectPositionAnimation::~ObjectPositionAnimation()
{
}

void ObjectPositionAnimation::SetTicksPerSecond(float ticks)
{
	ticksPerSeconds = ticks;
}

void ObjectPositionAnimation::AddKeyframe(float time, glm::mat4 TRS)
{
	Keyframe key;
	key.time = time;
	key.TRS = TRS;
	keyframes.push_back(key);
}

void ObjectPositionAnimation::Reset()
{
	currentTime = 0;
	currentKeyframe = 0;
}

glm::mat4 ObjectPositionAnimation::Update(float dt)
{
	if (currentKeyframe == keyframes.size() - 1) return keyframes[keyframes.size() - 1].TRS;
	//float timeToNextKeyframes = (keyframes[currentKeyframe +1].time - keyframes[currentKeyframe].time);

	//update current time
	currentTime += dt * ticksPerSeconds;

	//get interpolant
	float t = currentTime - keyframes[currentKeyframe].time;

	//check if gone over to the next keyframe
	if (t >= 1)
	{
		currentKeyframe++;
		t -= 1;
	}
	
	//check if last keyframe
	if (currentKeyframe == keyframes.size() - 1) return keyframes[keyframes.size() - 1].TRS;

	//lerp the matrices
	return (1 - t) * keyframes[currentKeyframe].TRS + t * keyframes[currentKeyframe + 1].TRS;
}
