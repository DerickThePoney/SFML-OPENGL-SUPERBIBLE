#pragma once

class AABB;
class Plane;
class Frustum;
class Sphere;
class Ray;

bool Intersect(const AABB& first, const AABB& second);

float Intersect(const AABB& box, const Plane& plane);
float Intersect(const Sphere& sphere, const Plane& plane);


bool Intersect(const AABB& box, const Frustum& frustum);
bool Intersect(const Sphere& sphere, const Frustum& frustum);

bool Intersect(const Sphere& sphere, const Ray& ray);