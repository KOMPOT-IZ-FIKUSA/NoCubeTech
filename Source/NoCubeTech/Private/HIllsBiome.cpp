// Fill out your copyright notice in the Description page of Project Settings.


#include "HIllsBiome.h"
#include "RandomGenerator.h"

float periodic_1(float a) {
    return 4 * abs(a - floor(a) - 0.5) - 1;
}


float FikusDistanceToLineSegment(float x, float y, float x1, float y1, float x2, float y2, float blur) {

    float dx = x2 - x1;
    float dy = y2 - y1;
    float lenSquared = dx * dx + dy * dy;

    float dist;

    if (lenSquared == 0.0) {
        // Line segment is a point
        dist = sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));
    }
    else {
        float t = fmax(0.0, fmin(1.0, ((x - x1) * dx + (y - y1) * dy) / lenSquared));
        float projX = x1 + t * dx;
        float projY = y1 + t * dy;
        dist = sqrt((x - projX) * (x - projX) + (y - projY) * (y - projY));
    }

    float val = fmax(0.0, fmin(1.0, dist / blur));
    return 1.0 - val;
}

float FikusMountainsGenerator1(int xAnchor, int yAnchor, float x, float y, int seed) {

    int seed0 = seed;
    seed += xAnchor * 12345 + yAnchor * 12346;

    x = (x - float(xAnchor)) * 2.0 - 1.0;
    y = (y - float(yAnchor)) * 2.0 - 1.0;

    x += RandomGenerator::BicubicNoiseInterpolation(0.7 * x, 0.7 * y, seed) * 0.8 - 0.4;
    y += RandomGenerator::BicubicNoiseInterpolation(0.7 * x, 0.7 * y, seed + 1) * 0.8 - 0.4;


    float result2 = 0.0;

    float minDistance = 10000000.0;

    float branch1XPrev = 0.;
    float branch1YPrev = 0.;
    float angle = RandomGenerator::BilinearNoiseInterpolation(float(xAnchor) * 0.4, float(yAnchor) * 0.4, seed0) * 6.28;

    float branch1X = cos(angle) * 1.8;
    float branch1Y = sin(angle) * 1.8;

    float result1 = FikusDistanceToLineSegment(x, y, branch1X, branch1Y, -branch1X, -branch1Y, 0.1 + RandomGenerator::IntToFloat(seed - 1));

    result1 = (result1 + result1 * result1) * 0.5;

    int subBranches = RandomGenerator::IntToInt(seed) % 20;

    for (int i = 0; i < subBranches; i++) {
        if (RandomGenerator::IntToInt(seed + i * 100) % 3 == 0) continue;

        float k = float(i) / float(subBranches - 1) * 2.0 - 1.0;

        float branch2X1 = branch1X * k;
        float branch2Y1 = branch1Y * k;

        float angle3 = 1.0;
        if (i % 2 == 0) {
            angle3 = angle + 3.141592 / 2.0 - 1.3 * k;
        }
        else {
            angle3 = angle - 3.141592 / 2.0 + 1.3 * k;
        }
        angle3 += (RandomGenerator::IntToFloat(seed + 4 + i) - 0.5) / 2.0;

        float length_ = 0.2 + 0.8 * RandomGenerator::IntToFloat(seed + i * 12 + 166);

        float branch2X2 = branch2X1 + cos(angle3) * length_;
        float branch2Y2 = branch2Y1 + sin(angle3) * length_;

        float distortedX = x + RandomGenerator::BicubicNoiseInterpolation(2.0 * x, 2.0 * y, seed + 5 + i) * 0.4 - 0.2;
        float distortedY = y + RandomGenerator::BicubicNoiseInterpolation(2.0 * x, 2.0 * y, seed + 6 + i) * 0.4 - 0.2;

        float val = FikusDistanceToLineSegment(distortedX, distortedY, branch2X1, branch2Y1, branch2X2, branch2Y2, 0.6 + 1.2 * RandomGenerator::IntToFloat(seed + i * 124 + 12));
        val = val * val * val * (0.5 + RandomGenerator::IntToFloat(seed + i * 13 + 14) * 0.5);
        result2 = fmax(result2, val);

    }
    float threshold1 = 0.5 + RandomGenerator::IntToFloat(seed - 3);
    float threshold2 = 0.8 + RandomGenerator::IntToFloat(seed - 4) * 0.2;
    if (result1 > threshold1) result1 = threshold1;
    if (result2 > threshold2) result2 = threshold2;

    float res = fmax(result1, result2 * 0.8);
    res = fmax(0.0, fmin(1.0, res));

    return res;

}



float calculate3(float x, float y, int seed) {
    float x0 = floor(x);
    float y0 = floor(y);

    int x0i = int(x0);
    int y0i = int(y0);

    float dx = x - x0;
    float dy = y - y0;

    int coordResultX0;
    int coordResultY0;

    if (dx >= 0.5) {
        coordResultX0 = x0i;
        dx -= 0.5;
    }
    else {
        coordResultX0 = x0i - 1;
        dx += 0.5;
    }

    if (dy >= 0.5) {
        coordResultY0 = y0i;
        dy -= 0.5;
    }
    else {
        coordResultY0 = y0i - 1;
        dy += 0.5;
    }


    float v00, v01, v10, v11;

    v00 = FikusMountainsGenerator1(coordResultX0, coordResultY0, x, y, seed);
    v01 = FikusMountainsGenerator1(coordResultX0, coordResultY0 + 1, x, y, seed);
    v10 = FikusMountainsGenerator1(coordResultX0 + 1, coordResultY0, x, y, seed);
    v11 = FikusMountainsGenerator1(coordResultX0 + 1, coordResultY0 + 1, x, y, seed);

    return
        (1.0 - dx) * (1.0 - dy) * v00 +
        (1.0 - dx) * dy * v01 +
        dx * (1.0 - dy) * v10 +
        dx * dy * v11;
}


float HillsBiome::GenerateHeight(float x, float y, int64 seed) {

    return
        70000 * calculate3(x / 250000, y / 250000, seed) +
        100000 * (RandomGenerator::BicubicNoiseInterpolation(x / 100000000, y / 100000000, seed + 1)) +
        0 * 30000 * (RandomGenerator::BilinearNoiseInterpolation(x / 40000, y / 40000, seed) - 0.5);

}
