#include "PerlinNoise.h"
#include "entity/ga_entity.h"
#include "../graphics/ga_material.h"

#define GLEW_STATIC
#include <GL/glew.h>

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>

PerlinNoise::PerlinNoise(ga_entity* ent, const char* texture_file) : ga_component(ent)
{
	_material = new ga_unlit_texture_material(texture_file);
	_material->init();

	int numOctaves = 9;
	float persistence = 0.2;

	float amplitude = 6;
	float seaLevel = 2.5;
	
	float perlinNoiseArray[128][128];

	float max = -1;
	float min = 999;
	for (int i = 0; i < 128; i++)
	{
		for (int j = 0; j < 128; j++)
		{
			perlinNoiseArray[i][j] = PerlinNoise_2D(i, j, persistence, numOctaves, false);
			if (perlinNoiseArray[i][j] > max) { max = perlinNoiseArray[i][j]; }
			if (perlinNoiseArray[i][j] < min) { min = perlinNoiseArray[i][j]; }
		}
	}

	for (int i = 0; i < 128; i++)
	{
		for (int j = 0; j < 128; j++)
		{
			perlinNoiseArray[i][j] += std::abs(min);
			perlinNoiseArray[i][j] /= (std::abs(min) + max);
			perlinNoiseArray[i][j] *= amplitude;
			if (perlinNoiseArray[i][j] < seaLevel) { perlinNoiseArray[i][j] = seaLevel; }
			//std::cout << perlinNoiseArray[i][j] << "    ";
		}
		//std::cout << std::endl;
	}

	GLfloat vertices[128*128*3];
	GLushort indices[127*127*4 + 127*2 + 127*2];

	for (int i = 0; i < 128; i++)
	{
		for (int j = 0; j < 128; j++)
		{
			vertices[i*128*3 + j*3 + 0] = (float)j;
			vertices[i*128*3 + j*3 + 1] = perlinNoiseArray[j][i];
			vertices[i*128*3 + j*3 + 2] = (float)i;
		}
	}

	int indicesIndex = 0;

	for (int i = 0; i < 127; i++)
	{
		for (int j = 0; j < 127; j++)
		{
			indices[indicesIndex] = (unsigned short)i*128 + j;		indicesIndex++;
			indices[indicesIndex] = (unsigned short)i*128 + j + 1;	indicesIndex++;
			indices[indicesIndex] = (unsigned short)i*128 + j;		indicesIndex++;
			indices[indicesIndex] = (unsigned short)i*128 + j + 128;	indicesIndex++;
		}
	}

	for (int i = 127; i < 128; i++)
	{
		for (int j = 0; j < 127; j++)
		{
			indices[indicesIndex] = (unsigned short)i*128 + j;		indicesIndex++;
			indices[indicesIndex] = (unsigned short)i*128 + j + 1;	indicesIndex++;
		}
	}

	for (int i = 0; i < 127; i++)
	{
		for (int j = 127; j < 128; j++)
		{
			indices[indicesIndex] = (unsigned short)i*128 + j;		indicesIndex++;
			indices[indicesIndex] = (unsigned short)i*128 + j + 128;	indicesIndex++;
		}
	}

	/*static GLfloat vertices[] = {
		0.0, perlinNoiseArray[0][0], 0.0,
		1.0, perlinNoiseArray[1][0], 0.0,
		2.0, perlinNoiseArray[2][0], 0.0,
		3.0, perlinNoiseArray[3][0], 0.0,

		0.0, perlinNoiseArray[0][1], 1.0,
		1.0, perlinNoiseArray[1][1], 1.0,
		2.0, perlinNoiseArray[2][1], 1.0,
		3.0, perlinNoiseArray[3][1], 1.0,

		0.0, perlinNoiseArray[0][2], 2.0,
		1.0, perlinNoiseArray[1][2], 2.0,
		2.0, perlinNoiseArray[2][2], 2.0,
		3.0, perlinNoiseArray[3][2], 2.0,

		0.0, perlinNoiseArray[0][3], 3.0,
		1.0, perlinNoiseArray[1][3], 3.0,
		2.0, perlinNoiseArray[2][3], 3.0,
		3.0, perlinNoiseArray[3][3], 3.0
	};
	static GLushort indices[] = {
		0, 1,		0, 4,
		1, 2,		1, 5,
		2, 3,		2, 6,		3, 7,
		
		4, 5,		4, 8,
		5, 6,		5, 9,
		6, 7,		6, 10,		7, 11,

		8, 9,		8, 12,
		9, 10,		9, 13,
		10, 11,		10, 14,		11, 15,		

		12, 13,
		13, 14,
		14, 15
	};*/

	_index_count = uint32_t(sizeof(indices) / sizeof(*indices));

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(2, _vbos);

	glBindBuffer(GL_ARRAY_BUFFER, _vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

PerlinNoise::~PerlinNoise()
{
	glDeleteBuffers(2, _vbos);
	glDeleteVertexArrays(1, &_vao);

	delete _material;
}

void PerlinNoise::update(ga_frame_params* params)
{
	/*float dt = std::chrono::duration_cast<std::chrono::duration<float>>(params->_delta_time).count();
	ga_quatf axis_angle;
	axis_angle.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(60.0f) * dt);
	get_entity()->rotate(axis_angle);*/

	ga_static_drawcall draw;
	draw._name = "PerlinNoise";
	draw._vao = _vao;
	draw._index_count = _index_count;
	draw._transform = get_entity()->get_transform();
	draw._draw_mode = GL_LINES;
	draw._material = _material;

	while (params->_static_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_static_drawcalls.push_back(draw);
	params->_static_drawcall_lock.clear(std::memory_order_release);
}

float PerlinNoise::Noise(int x, int y, int p)
{
	int n = x + y * 57;
	n = (n << 13) ^ n;
	return (float)(1.0 - ((n * (n * n * primes[p][0] + primes[p][1]) + primes[p][2]) & 0x7fffffff) / 1073741824.0);
}

float PerlinNoise::SmoothNoise(float x, float y, int p)
{
	float corners = (Noise(x - 1, y - 1, p) + Noise(x + 1, y - 1, p) + Noise(x - 1, y + 1, p) + Noise(x + 1, y + 1, p)) / 16;
	float sides = (Noise(x - 1, y, p) + Noise(x + 1, y, p) + Noise(x, y - 1, p) + Noise(x, y + 1, p)) / 8;
	float center = Noise(x, y, p) / 4;
	return corners + sides + center;
}

float PerlinNoise::LinearInterpolation(float x, float y, float t)
{
	return x * (1 - t) + y * t;
}

float PerlinNoise::CosineInterpolation(float x, float y, float t)
{
	float ft = t * 3.1415927;
	float f = (1 - cos(ft)) * 0.5;

	return x * (1 - f) + y * f;
}

float PerlinNoise::InterpolatedNoise(float x, float y, int p, bool linear)
{
	int integer_X = int(x);
	float fractional_X = x - integer_X;

	int integer_Y = int(y);
	float fractional_Y = y - integer_Y;

	float v1 = SmoothNoise(integer_X, integer_Y, p);
	float v2 = SmoothNoise(integer_X + 1, integer_Y, p);
	float v3 = SmoothNoise(integer_X, integer_Y + 1, p);
	float v4 = SmoothNoise(integer_X + 1, integer_Y + 1, p);

	if (linear)
	{
		float i1 = LinearInterpolation(v1, v2, fractional_X);
		float i2 = LinearInterpolation(v3, v4, fractional_X);

		return LinearInterpolation(i1, i2, fractional_Y);
	}
	else
	{
		float i1 = CosineInterpolation(v1, v2, fractional_X);
		float i2 = CosineInterpolation(v3, v4, fractional_X);

		return CosineInterpolation(i1, i2, fractional_Y);
	}
}

float PerlinNoise::PerlinNoise_2D(float x, float y, float persistence, int numOctaves, bool linear)
{
	float total = 0;

	for (int i = 0; i < numOctaves; i++)
	{
		float frequency = pow(2, i);
		float amplitude = pow(persistence, i);

		total += (InterpolatedNoise(x * frequency, y * frequency, 1, linear) * amplitude);
		//std::cout << "(" << x << "," << y << "):    " << total;
	}
	//std::cout << std::endl;

	return total;

	/*float total = 0;
	float frequency = pow(2, numOctaves);
	float amplitude = 1;
	for (int i = 0; i < numOctaves; ++i) 
	{
		frequency /= 2;
		amplitude *= persistence;
		total += InterpolatedNoise(x / frequency, y / frequency, i%9, linear) * amplitude;
	}
	return total / frequency;*/
}