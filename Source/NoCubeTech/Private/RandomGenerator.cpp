// Fill out your copyright notice in the Description page of Project Settings.


#include "RandomGenerator.h"

RandomGenerator::RandomGenerator()
{
}

RandomGenerator::~RandomGenerator()
{
}

int RandomGenerator::IntToInt(int a) {
	a = (a ^ 61) ^ (a >> 16);
	a = a + (a << 3);
	a = a ^ (a >> 4);
	a = a * 0x27d4eb2d;
	a = a ^ (a >> 15);
	return a;
}

float RandomGenerator::IntToFloat(int a) {
	return (IntToInt(a) & 0x000000000000FFFF) * 0.0000152587890625; // (r % 2^16) / 2^16
}

float RandomGenerator::Int_x_y_seed_ToFloat(int x, int y, int seed) {
	return IntToFloat(x * 18579 + y * 13571 + seed);
}

float RandomGenerator::Int_x_y_z_seed_ToFloat(int x, int y, int z, int seed) {
	return IntToFloat(x * 13959 + y * 15798 + z * 345341 + seed);
}

float RandomGenerator::BilinearNoiseInterpolation(float x, float y, int seed) {
	int x0 = floor(x);
	int y0 = floor(y);
	float v00 = Int_x_y_seed_ToFloat(x0, y0, seed);
	float v01 = Int_x_y_seed_ToFloat(x0, y0 + 1, seed);
	float v10 = Int_x_y_seed_ToFloat(x0 + 1, y0, seed);
	float v11 = Int_x_y_seed_ToFloat(x0 + 1, y0 + 1, seed);

	float dx = x - x0;
	float dy = y - y0;
	return
		v00 * (1 - dx) * (1 - dy) +
		v01 * (1 - dx) * dy +
		v10 * dx * (1 - dy) +
		v11 * dx * dy;

}

float RandomGenerator::TrilinearNoiseInterpolation(float x, float y, float z, int seed) {
	int x0 = floor(x);
	int y0 = floor(y);
	int z0 = floor(z);
	float v000 = Int_x_y_z_seed_ToFloat(x0, y0, z0, seed);
	float v010 = Int_x_y_z_seed_ToFloat(x0, y0 + 1, z0, seed);
	float v100 = Int_x_y_z_seed_ToFloat(x0 + 1, y0, z0, seed);
	float v110 = Int_x_y_z_seed_ToFloat(x0 + 1, y0 + 1, z0, seed);
	float v001 = Int_x_y_z_seed_ToFloat(x0, y0, z0 + 1, seed);
	float v011 = Int_x_y_z_seed_ToFloat(x0, y0 + 1, z0 + 1, seed);
	float v101 = Int_x_y_z_seed_ToFloat(x0 + 1, y0, z0 + 1, seed);
	float v111 = Int_x_y_z_seed_ToFloat(x0 + 1, y0 + 1, z0 + 1, seed);

	float dx = x - x0;
	float dy = y - y0;
	float dz = z - z0;
	return
		v000 * (1 - dx) * (1 - dy) * (1 - dz) +
		v010 * (1 - dx) * dy * (1 - dz) +
		v100 * dx * (1 - dy) * (1 - dz) +
		v110 * dx * dy * (1 - dz) +
		v001 * (1 - dx) * (1 - dy) * dz +
		v011 * (1 - dx) * dy * dz +
		v101 * dx * (1 - dy) * dz +
		v111 * dx * dy * dz;

}

float RandomGenerator::PeriodicBilinearNoiseInterpolation(float x, float y, int periodX, int periodY, int seed) {
	float xf = floor(x);
	float yf = floor(y);
	
	int x0 = ((int)xf % periodX + periodX) % periodX;
	int y0 = ((int)floor(y) % periodY + periodY) % periodY;
	int x1 = (x0 + 1) % periodX;
	int y1 = (y0 + 1) % periodY;
	float v00 = Int_x_y_seed_ToFloat(x0, y0, seed);
	float v01 = Int_x_y_seed_ToFloat(x0, y1, seed);
	float v10 = Int_x_y_seed_ToFloat(x1, y0, seed);
	float v11 = Int_x_y_seed_ToFloat(x1, y1, seed);

	float dx = x - xf;
	float dy = y - yf;
	return
		v00 * (1 - dx) * (1 - dy) +
		v01 * (1 - dx) * dy +
		v10 * dx * (1 - dy) +
		v11 * dx * dy;

}

float RandomGenerator::BicubicNoiseInterpolation(float x, float y, int seed) {
	int x0 = floor(x);
	int y0 = floor(y);
	float v00 = Int_x_y_seed_ToFloat(x0, y0, seed);

	float dx = x - x0;
	float dy = y - y0;

	float a00 = Int_x_y_seed_ToFloat(x0 - 1, y0 - 1, seed);
	float a01 = Int_x_y_seed_ToFloat(x0 - 1, y0, seed);
	float a02 = Int_x_y_seed_ToFloat(x0 - 1, y0 + 1, seed);
	float a03 = Int_x_y_seed_ToFloat(x0 - 1, y0 + 2, seed);
	float a10 = Int_x_y_seed_ToFloat(x0, y0 - 1, seed);
	float a11 = Int_x_y_seed_ToFloat(x0, y0, seed);
	float a12 = Int_x_y_seed_ToFloat(x0, y0 + 1, seed);
	float a13 = Int_x_y_seed_ToFloat(x0, y0 + 2, seed);
	float a20 = Int_x_y_seed_ToFloat(x0 + 1, y0 - 1, seed);
	float a21 = Int_x_y_seed_ToFloat(x0 + 1, y0, seed);
	float a22 = Int_x_y_seed_ToFloat(x0 + 1, y0 + 1, seed);
	float a23 = Int_x_y_seed_ToFloat(x0 + 1, y0 + 2, seed);
	float a30 = Int_x_y_seed_ToFloat(x0 + 2, y0 - 1, seed);
	float a31 = Int_x_y_seed_ToFloat(x0 + 2, y0, seed);
	float a32 = Int_x_y_seed_ToFloat(x0 + 2, y0 + 1, seed);
	float a33 = Int_x_y_seed_ToFloat(x0 + 2, y0 + 2, seed);

	float b0 = a10 + 0.5 * dx * (a20 - a00 + dx * (
		2.0 * a00 - 5.0 * a10 + 4.0 * a20 - a30 + dx * (3.0 * (a10 - a20) + a30 - a00)));
	float b1 = a11 + 0.5 * dx * (a21 - a01 + dx * (
		2.0 * a01 - 5.0 * a11 + 4.0 * a21 - a31 + dx * (3.0 * (a11 - a21) + a31 - a01)));
	float b2 = a12 + 0.5 * dx * (a22 - a02 + dx * (
		2.0 * a02 - 5.0 * a12 + 4.0 * a22 - a32 + dx * (3.0 * (a12 - a22) + a32 - a02)));
	float b3 = a13 + 0.5 * dx * (a23 - a03 + dx * (
		2.0 * a03 - 5.0 * a13 + 4.0 * a23 - a33 + dx * (3.0 * (a13 - a23) + a33 - a03)));
	return b1 + 0.5 * dy * (b2 - b0 + dy * (
		2.0 * b0 - 5.0 * b1 + 4.0 * b2 - b3 + dy * (3.0 * (b1 - b2) + b3 - b0)));

}