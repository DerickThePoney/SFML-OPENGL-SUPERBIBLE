#pragma once

class AABB;
class Plane;
class Frustum;
class BoundingSphere;

bool Intersect(const AABB& first, const AABB& second);

float Intersect(const AABB& box, const Plane& plane);
float Intersect(const BoundingSphere& sphere, const Plane& plane);


bool Intersect(const AABB& box, const Frustum& frustum);
bool Intersect(const BoundingSphere& sphere, const Frustum& frustum);