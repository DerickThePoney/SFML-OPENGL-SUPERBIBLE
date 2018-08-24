#pragma once
#include "ValueNoise2D.h"
#include "BezierSpline.h"

class ISignedDistance
{
public:
	virtual float Evaluate(glm::vec3& point) = 0;

public:
	/** helpers */
	static float smin(float a, float b, float k);
};

class SignedDistanceSphere : public ISignedDistance
{
public:
	SignedDistanceSphere() : ISignedDistance() {}
	// Hérité via ISignedDistance
	virtual float Evaluate(glm::vec3& point) override;

	glm::vec3 center;
	float radius;
};

class SignedDistanceCapsule : public ISignedDistance
{
public:
	SignedDistanceCapsule() : ISignedDistance() {}
	// Hérité via ISignedDistance
	virtual float Evaluate(glm::vec3& point) override;

	glm::vec3 start;
	glm::vec3 end;
	float size;
};

class SignedDistanceCylinder : public ISignedDistance
{
public:
	SignedDistanceCylinder() : ISignedDistance() {}
	// Hérité via ISignedDistance
	virtual float Evaluate(glm::vec3& point) override;

	glm::vec3 center;
	glm::vec3 direction;
	float size;
};



class SignedDistanceField : public ISignedDistance
{
public:
	SignedDistanceField() : ISignedDistance() {}
	~SignedDistanceField();
	virtual float Evaluate(glm::vec3& point) override;

	void AddUnionDistance(ISignedDistance* distance);

protected:
	std::vector<ISignedDistance*> UnionSignedDistance;
	std::vector<ISignedDistance*> SubtractSignedDistance;
	std::vector<ISignedDistance*> IntersectSignedDistance;
};

class SignedDistanceDecorator : public ISignedDistance
{
public:
	SignedDistanceDecorator() : ISignedDistance() {};
	~SignedDistanceDecorator() { delete child; };
	// Hérité via ISignedDistance
	virtual float Evaluate(glm::vec3& point) override;

	ISignedDistance* child;
};

class SignedDistanceRepetition : public SignedDistanceDecorator
{
public:
	SignedDistanceRepetition() : SignedDistanceDecorator() {};
	~SignedDistanceRepetition() { delete child; };
	// Hérité via ISignedDistance
	virtual float Evaluate(glm::vec3& point) override;

	glm::vec3 repetion;
	ISignedDistance* child;
};

class SignedDistanceDisplacement : public SignedDistanceDecorator
{
public:
	SignedDistanceDisplacement() : SignedDistanceDecorator() {};
	~SignedDistanceDisplacement() { delete child; };
	// Hérité via ISignedDistance
	virtual float Evaluate(glm::vec3& point) override;

	glm::mat4 displacement;
	ISignedDistance* child;
};


class SignedDistanceScale : public SignedDistanceDecorator
{
public:
	SignedDistanceScale(): SignedDistanceDecorator() {};
	~SignedDistanceScale() { delete child; };
	// Hérité via ISignedDistance
	virtual float Evaluate(glm::vec3& point) override;

	float scale;
	ISignedDistance* child;
};

class SignedDistanceMonster : public ISignedDistance
{
public:
	SignedDistanceMonster();
	// Hérité via ISignedDistance
	virtual float Evaluate(glm::vec3& point) override;

protected:
	float DistanceToTentacules(glm::vec3& point);
	float EvaluateArm(glm::vec3& point, int i, float rr);

private:
	SignedDistanceCylinder distToTheXAxis;
	SignedDistanceSphere ball;
	ValueNoise2D noise2D;
	
};

enum BranchingPattern
{
	HELICOIDAL,
	REGULAR
};

struct TreeSpeciesDNA
{
	unsigned int seed;

	//Trunk
	//sections
	int minNumberOfSections;
	int maxNumberOfSections;

	//height of the trunk
	float minHeight;
	float maxHeight;

	//width of the trunk
	float minWidth;
	float maxWidth;

	//Top width Scaling
	float maxTopWidth;
	float minTopWidth;

	//X&Z noise scaling as function of height
	float minXYNoiseScale;
	float maxXYNoiseScale;

	//X Noise
	float minXOffset;
	float maxXOffset;
	float minXFrequency;
	float maxXFrequency;
	float minXStrength;
	float maxXStrength;
	int minXOctave;
	int maxXOctave;

	//Z Noise
	float minZOffset;
	float maxZOffset;
	float minZFrequency;
	float maxZFrequency;
	float minZStrength;
	float maxZStrength;
	int minZOctave;
	int maxZOctave;

	//W Noise
	float minWOffset;
	float maxWOffset;
	float minWFrequency;
	float maxWFrequency;
	float minWStrength;
	float maxWStrength;
	int minWOctave;
	int maxWOctave;
};

struct TreeInstanceDNA
{
	const TreeSpeciesDNA* dna;

	unsigned int seed;

	//Trunk
	//sections
	unsigned int trunkSectionsSeed;
	unsigned int numberOfSections;

	//height of the trunk
	float height;

	//width of the trunk
	float width;

	//Top width Scaling
	float topWidth;

	//X&Z noise scaling as function of height
	float XYNoiseScale;

	//X Noise
	unsigned int XSeed;
	float XOffset;
	float XFrequency;
	float XStrength;
	unsigned int XOctave;

	//Z Noise
	unsigned int ZSeed;
	float ZOffset;
	float ZFrequency;
	float ZStrength;
	unsigned int ZOctave;

	//W Noise
	unsigned int WSeed;
	float WOffset;
	float WFrequency;
	float WStrength;
	unsigned int WOctave;

	//Braching pattern
};

class SignedDistanceTree : public ISignedDistance
{
public:
	SignedDistanceTree();
	SignedDistanceTree(TreeInstanceDNA dna);
	// Hérité via ISignedDistance
	virtual float Evaluate(glm::vec3& point) override;

	void SetDNA(const TreeInstanceDNA& dna) { this->dna = dna; Init(); }
	
protected:
	void Init();

	float Noise(float point, ValueNoise1D& noise1D, float freq, float strength, int octaves);
	float EvaluateTrunk(glm::vec3& point, float widthscale, int level = 0, float baseScale = 1.0f);
	float EvaluateTrunkSection(glm::vec3& point, glm::vec3 & start, glm::vec3 & End);
	void InitTrunkSections();




private:
	TreeInstanceDNA dna;
	
	SignedDistanceCapsule capsuleTrunk;
	//SignedDistanceSphere ball;
	ValueNoise2D noise2D;
	ValueNoise1D noise1DX;
	ValueNoise1D noise1DZ;
	ValueNoise1D noiseScale;

	std::vector<glm::vec3>  trunkSections;
	float k;
	CubicBezierCurve<float> curveX;
	CubicBezierCurve<float> curveZ;
};