#pragma once
#include "entity/ga_component.h"
#include <cstdint>

class PerlinNoise : public ga_component
{
public:
	PerlinNoise(class ga_entity* ent, const char* texture_file);
	virtual ~PerlinNoise();

	virtual void update(struct ga_frame_params* params) override;

	float Noise(int x, int y, int p);
	float SmoothNoise(float x, float y, int p);
	float LinearInterpolation(float x, float y, float t);
	float CosineInterpolation(float x, float y, float t);
	float InterpolatedNoise(float x, float y, int p, bool linear);
	float PerlinNoise_2D(float x, float y, float persistence, int numOctaves, bool linear);

private:
	class ga_material* _material;
	uint32_t _vao;
	uint32_t _vbos[2];
	uint32_t _index_count;

	int primes[9][3] = {
		{ 995615039, 600173719, 701464987 },
		{ 831731269, 162318869, 136250887 },
		{ 174329291, 946737083, 245679977 },
		{ 362489573, 795918041, 350777237 },
		{ 457025711, 880830799, 909678923 },
		{ 787070341, 177340217, 593320781 },
		{ 405493717, 291031019, 391950901 },
		{ 458904767, 676625681, 424452397 },
		{ 531736441, 939683957, 810651871 }
	};
};