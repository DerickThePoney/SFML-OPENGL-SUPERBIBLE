#include "stdafx.h"
#include "Frustum.h"


Frustum::Frustum()
{
}


Frustum::~Frustum()
{
}

void Frustum::Set(const glm::mat4 & projection)
{
	/*// Right clipping plane.
	planes[0] = Plane(projection[0][3] - projection[0][0], projection[1][3] - projection[1][0], projection[2][3] - projection[2][0], projection[3][3] - projection[3][0]);
	// Left clipping plane.
	planes[1] = Plane(projection[0][3] + projection[0][0], projection[1][3] + projection[1][0], projection[2][3] + projection[2][0], projection[3][3] + projection[3][0]);
	// Bottom clipping plane.
	planes[2] = Plane(projection[0][3] + projection[0][1], projection[1][3] + projection[1][1], projection[2][3] + projection[2][1], projection[3][3] + projection[3][1]);
	// Top clipping plane.
	planes[3] = Plane(projection[0][3] - projection[0][1], projection[1][3] - projection[1][1], projection[2][3] - projection[2][1], projection[3][3] - projection[3][1]);
	// Far clipping plane.
	planes[4] = Plane(projection[0][3] - projection[0][2], projection[1][3] - projection[1][2], projection[2][3] - projection[2][2], projection[3][3] - projection[3][2]);
	// Near clipping plane.
	planes[5] = Plane(projection[0][3] + projection[0][2], projection[1][3] + projection[1][2], projection[2][3] + projection[2][2], projection[3][3] + projection[3][2]);*/

	
	/*glm::vec4 right = glm::normalize(projection * glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f));
	float dr = -glm::dot(right, projection * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	glm::vec4 left = glm::normalize(projection * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
	float dl = -glm::dot(left, projection * glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f));

	glm::vec4 top = glm::normalize(projection * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f));
	float dt = -glm::dot(top, projection * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	glm::vec4 bottom = glm::normalize(projection * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
	float dbo = -glm::dot(bottom, projection * glm::vec4(0.0f, -1.0f, 0.0f, 1.0f));

	glm::vec4 front = glm::normalize(projection * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
	float df = -glm::dot(front, projection * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	glm::vec4 back = glm::normalize(projection * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
	float dba = -glm::dot(back, projection * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));


	
	// Right clipping plane.
	planes[0] = Plane(right.x, right.y, right.z, dr);
	// Left clipping plane.
	planes[1] = Plane(left.x, left.y, left.z, dl);
	// Bottom clipping plane.
	planes[2] = Plane(bottom.x, bottom.y, bottom.z, dbo);
	// Top clipping plane.
	planes[3] = Plane(top.x, top.y, top.z, dt);
	// Far clipping plane.
	planes[4] = Plane(front.x, front.y, front.z, dba);
	// Near clipping plane.
	planes[5] = Plane(back.x, back.y, back.z, df);*/

	glm::vec4 planesLocal[6];

	planesLocal[LEFT].x = projection[0].w + projection[0].x;
	planesLocal[LEFT].y = projection[1].w + projection[1].x;
	planesLocal[LEFT].z = projection[2].w + projection[2].x;
	planesLocal[LEFT].w = projection[3].w + projection[3].x;

	planesLocal[RIGHT].x = projection[0].w - projection[0].x;
	planesLocal[RIGHT].y = projection[1].w - projection[1].x;
	planesLocal[RIGHT].z = projection[2].w - projection[2].x;
	planesLocal[RIGHT].w = projection[3].w - projection[3].x;

	planesLocal[TOP].x = projection[0].w - projection[0].y;
	planesLocal[TOP].y = projection[1].w - projection[1].y;
	planesLocal[TOP].z = projection[2].w - projection[2].y;
	planesLocal[TOP].w = projection[3].w - projection[3].y;

	planesLocal[BOTTOM].x = projection[0].w + projection[0].y;
	planesLocal[BOTTOM].y = projection[1].w + projection[1].y;
	planesLocal[BOTTOM].z = projection[2].w + projection[2].y;
	planesLocal[BOTTOM].w = projection[3].w + projection[3].y;

	planesLocal[BACK].x = projection[0].w + projection[0].z;
	planesLocal[BACK].y = projection[1].w + projection[1].z;
	planesLocal[BACK].z = projection[2].w + projection[2].z;
	planesLocal[BACK].w = projection[3].w + projection[3].z;

	planesLocal[FRONT].x = projection[0].w - projection[0].z;
	planesLocal[FRONT].y = projection[1].w - projection[1].z;
	planesLocal[FRONT].z = projection[2].w - projection[2].z;
	planesLocal[FRONT].w = projection[3].w - projection[3].z;

	for (auto i = 0; i < 6; i++)
	{
		float length = sqrtf(planesLocal[i].x * planesLocal[i].x + planesLocal[i].y * planesLocal[i].y + planesLocal[i].z * planesLocal[i].z);
		planesLocal[i] /= length;
		planes[i] = Plane(planesLocal[i]);
	}
}
