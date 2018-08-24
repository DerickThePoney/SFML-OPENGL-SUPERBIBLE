#include "stdafx.h"
#include "SignedDistanceField.h"

float SignedDistanceCapsule::Evaluate(glm::vec3& point)
{
	glm::vec3 pa = point - start, ba = end - start;
	float h = glm::clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
	return length(pa - ba * h) - size;
}

SignedDistanceField::~SignedDistanceField()
{
	for (size_t i = 0; i < UnionSignedDistance.size(); ++i)
	{
		delete UnionSignedDistance[i];
	}

	for (size_t i = 0; i < SubtractSignedDistance.size(); ++i)
	{
		delete SubtractSignedDistance[i];
	}

	for (size_t i = 0; i < IntersectSignedDistance.size(); ++i)
	{
		delete IntersectSignedDistance[i];
	}

	UnionSignedDistance.clear();
	SubtractSignedDistance.clear();
	IntersectSignedDistance.clear();
}

float SignedDistanceField::Evaluate(glm::vec3& point)
{
	float fFinalValue = UnionSignedDistance[0]->Evaluate(point);;// std::numeric_limits<float>::max();

	for (size_t i = 1; i < UnionSignedDistance.size(); ++i)
	{
		float localValue = UnionSignedDistance[i]->Evaluate(point);
		fFinalValue = smin(fFinalValue, localValue, .1f);
	}

	return fFinalValue;
}

void SignedDistanceField::AddUnionDistance(ISignedDistance* distance)
{
	UnionSignedDistance.push_back(distance);
}

float ISignedDistance::smin(float a, float b, float k)
{
	float h = glm::clamp(0.5f + 0.5f*(b - a) / k, 0.0f, 1.0f);
	return glm::mix(b, a, h) - k * h*(1.0f - h);
}

float SignedDistanceDecorator::Evaluate(glm::vec3& point)
{
	return child->Evaluate(point);
}

float SignedDistanceRepetition::Evaluate(glm::vec3& point)
{
	glm::vec3 q = glm::mod(point, repetion) - 0.5f * repetion;
	return SignedDistanceDecorator::Evaluate(q);
}

float SignedDistanceDisplacement::Evaluate(glm::vec3& point)
{
	glm::vec3 q = glm::inverse(displacement) * glm::vec4(point, 1.0f);
	return SignedDistanceDecorator::Evaluate(q);
}

float SignedDistanceScale::Evaluate(glm::vec3& point)
{
	return SignedDistanceDecorator::Evaluate(point/scale) * scale;
}

SignedDistanceMonster::SignedDistanceMonster()
	: ISignedDistance()
{
	/*distToTheXAxis.start = glm::vec3(0.0f);
	distToTheXAxis.end = glm::vec3(5.0f, 0.0f, 0.0f);
	distToTheXAxis.size = .05f;*/

	distToTheXAxis.center = glm::vec3(0.0f);
	distToTheXAxis.direction = normalize(glm::vec3(1.0f, 0.0f, 0.0f));
	distToTheXAxis.size = 0.1f;

	ball.radius = 0.3f;
	ball.center = glm::vec3(0.0f, 0.25f, 0.0f);
	
}

float SignedDistanceMonster::Evaluate(glm::vec3& point)
{
	float distT = DistanceToTentacules(point);
	float distB = ball.Evaluate(point);
	float bFact = glm::smoothstep(0.0f, ball.radius, length(point));
	float dist = smin(distB, distT, .1f);
	return dist;
}



float SignedDistanceMonster::DistanceToTentacules(glm::vec3 & point)
{
	glm::vec2 pxy = glm::vec2(point.x, point.y);
	float rr = dot(pxy, pxy);

	float distance = EvaluateArm(point, 0, rr);

	for (int i = 1; i<6; i++)
	{
		float localArm = EvaluateArm(point, i, rr);
		distance = smin(distance, localArm, .01f);
	}

	return distance;
}

float SignedDistanceMonster::EvaluateArm(glm::vec3 & point, int i, float rr)
{
	const float TWOPI = (float)(3.14159265358979323846 * 2);
	glm::vec3 q = glm::rotateY(point, TWOPI*i / 6.0f + 0.4f *rr*noise2D.eval(glm::vec2(4*rr,6.3*i)));
	q.y -= 6*0.6f*rr*exp2(-10.0f*rr);
	return distToTheXAxis.Evaluate(q);
}

float SignedDistanceCylinder::Evaluate(glm::vec3& point)
{
	glm::vec3 pointPlaned = point - dot(direction, point) * direction;
	return length(pointPlaned - center) - size;
}

float SignedDistanceSphere::Evaluate(glm::vec3 & point)
{
	return length(point-center) - radius;
}

SignedDistanceTree::SignedDistanceTree()
	: curveX(CubicBezierCurve<float>({ 0.0f, 0.0f, 0.0f, 0.0f })), curveZ(CubicBezierCurve<float>({ 0.0f, 0.0f, 0.0f, 0.0f })), ISignedDistance()
{
}

SignedDistanceTree::SignedDistanceTree(TreeInstanceDNA dna) 
	: dna(dna), curveX(CubicBezierCurve<float>({0.0f, 0.0f, 0.0f, 0.0f })), curveZ(CubicBezierCurve<float>({ 0.0f, 0.0f, 0.0f, 0.0f })), ISignedDistance()
{
	//distToTheYAxis.center = glm::vec3(0.0f);
	//distToTheYAxis.direction = normalize(glm::vec3(0.0f, 1.0f, 0.0f));
	//distToTheYAxis.size = 0.2f;

	Init();
}

float SignedDistanceTree::Evaluate(glm::vec3 & point)
{
	float widthscale = (point.y < 0) ? 1 : exp(-k * point.y);// *Noise(point.y + dna.WOffset, noiseScale, dna.WFrequency, dna.WStrength, dna.WOctave);
	//float BranchWidthScale = (point.y < 0) ? 0 : widthscale;// *Noise(point.y + dna.WOffset, noiseScale, dna.WFrequency, dna.WStrength, dna.WOctave);
	float XYnoiseScale = (point.y < 0) ? 0 : glm::clamp(dna.XYNoiseScale*log(point.y+1), 0.0f,1.0f);
	glm::vec3 q = point;
	glm::vec3 qb = point;
	
	q.x /= widthscale;
	q.z /= widthscale;
	float eval;
	curveX.EvaluatePosition(point.y / (dna.height + 0.1f) , eval);
	eval = (point.y < 0) ? 0.0f : eval;
	q.x -= eval;
	curveZ.EvaluatePosition(point.y / (dna.height + 0.1f), eval);
	eval = (point.y < 0) ? 0.0f : eval;
	q.z -= eval;
	
	

	float dist = EvaluateTrunk(q, widthscale);

	glm::vec3 q2 = q;
	q2.y -= dna.height / 3;
	q2 = glm::rotateX(q2, glm::pi<float>() / 3.0f);
	

	dist = smin(dist, EvaluateTrunk(q2, widthscale, 0, .5f), 0.1f);

	glm::vec3 q3 = q;
	q3.y -= 2 * dna.height / 3;
	q3 = glm::rotateX(q3, -glm::pi<float>() / 3.0f);
	dist = smin(dist, EvaluateTrunk(q3, widthscale, 0, .5f), 0.1f);


	return dist;
}

void SignedDistanceTree::Init()
{
	capsuleTrunk.start = glm::vec3(0.0f);
	capsuleTrunk.end = glm::vec3(0.0f, dna.height, 0);
	capsuleTrunk.size = dna.width;

	noise1DX = ValueNoise1D(dna.XSeed);
	noise1DZ = ValueNoise1D(dna.ZSeed);
	noiseScale = ValueNoise1D(dna.WSeed);

	InitTrunkSections();
}

float SignedDistanceTree::Noise(float point, ValueNoise1D& noise1D, float freq, float strength, int octaves)
{
	float n = 0;

	for (int i = 0; i < octaves; ++i)
	{
		n += strength * noise1D.eval(point * freq);
		strength *= 0.5f;
		freq *= 2;
	}
	
	return n;
}

float SignedDistanceTree::EvaluateTrunk(glm::vec3 & point, float widthscale, int level, float baseScale)
{
	float scaleCache = capsuleTrunk.size;
	capsuleTrunk.size *= baseScale;
	float dist = EvaluateTrunkSection(point, trunkSections[0], trunkSections[1]);
	for (size_t i = 1; i < trunkSections.size() - 1; ++i)
	{
		dist = smin(dist, EvaluateTrunkSection(point, trunkSections[i], trunkSections[i+1]), 0.1f);
	}

	capsuleTrunk.size = scaleCache;
	return dist;
}

float SignedDistanceTree::EvaluateTrunkSection(glm::vec3 & point, glm::vec3 & start, glm::vec3 & End)
{
	capsuleTrunk.start = start;
	capsuleTrunk.end = End;
	return capsuleTrunk.Evaluate(point);
}

void SignedDistanceTree::InitTrunkSections()
{
	std::mt19937 gen(dna.trunkSectionsSeed);

	std::uniform_real_distribution<float> distrFloat(-.5f, 0.5f);
	auto randFloat = std::bind(distrFloat, gen);

	int sectionNumber = dna.numberOfSections;

	trunkSections.push_back(glm::vec3(0.0f));
	trunkSections.push_back(glm::vec3(0, dna.height, 0));
	
	std::vector<float> pos = { 0.0f,  randFloat(), randFloat(), randFloat()};
	// , 0.25f, 0.1f, 0.0f, -0.1f, -0.25f, -0.5f, -0.25f, -0.1f, 0.0f, 0.1f, 0.25f, 0.5f};
	curveX = CubicBezierCurve<float>(pos);
	pos = { 0.0f,  randFloat(), randFloat(), randFloat() };
	curveZ = CubicBezierCurve<float>(pos);

	/*for (int i = 0; i < sectionNumber; ++i)
	{
		trunkSections.push_back(glm::vec3(randFloat(), dna.height * (float)(i+1) / (sectionNumber), randFloat()));
	}*/
	sectionNumber = 1;

	k = -log(dna.topWidth / dna.width) / dna.height;
}
