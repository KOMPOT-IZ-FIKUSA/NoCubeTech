// Fill out your copyright notice in the Description page of Project Settings.


#include "TextureReadonlySource.h"


TextureReadonlySource::TextureReadonlySource(UTexture2D* texture) {
	data = TArray<SimpleTextureContainer>();
	int initialWidth = texture->GetSizeX();
	int initialHeight = texture->GetSizeY();
	FByteBulkData* bulk = &texture->GetPlatformData()->Mips[0].BulkData;
	FColor* data1 = (FColor*)bulk->Lock(LOCK_READ_ONLY);
	FColor* initialColors = new FColor[initialWidth* initialHeight];
	for (int i = 0; i < initialWidth * initialHeight; i++) {
		initialColors[i] = data1[i];
	}
	bulk->Unlock();

	int width = initialWidth;
	int height = initialHeight;
	FColor* buf = initialColors;
	
	while (width > 1 && height > 1) { // when one of those is equal to 2 or 3 it is the last iteration
		SimpleTextureContainer createdContainer(width, height, buf);
		data.Add(createdContainer);
		width /= 2;
		height /= 2;
		buf = new FColor[width * height];
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int i = y * width + x;
				
				FColor color1 = createdContainer.AccessByXY(x * 2, y * 2);
				FColor color2 = createdContainer.AccessByXY(x * 2 + 1, y * 2);
				FColor color3 = createdContainer.AccessByXY(x * 2, y * 2 + 1);
				FColor color4 = createdContainer.AccessByXY(x * 2 + 1, y * 2 + 1);

				uint8 R = static_cast<uint8>((color1.R + color2.R + color3.R + color4.R)/4);
				uint8 G = static_cast<uint8>((color1.G + color2.G + color3.G + color4.G)/4);
				uint8 B = static_cast<uint8>((color1.B + color2.B + color3.B + color4.B)/4);
				uint8 A = static_cast<uint8>((color1.A + color2.A + color3.A + color4.A)/4);

				buf[i] = FColor(R, G, B, A);
			}
		}
	}
	data.Add(SimpleTextureContainer(width, height, buf));
}

TextureReadonlySource::~TextureReadonlySource()
{
	for (int i = 0; i < data.Num(); i++) {
		delete[] data[i].colors;
	}
}

FColor TextureReadonlySource::GetColorNearest(float x, float y, float scale) {
	int i = FastIntLogarithm(scale);
	if (i >= data.Num()) {
		i = data.Num() - 1;
	}
	const SimpleTextureContainer& container = data[i];
	float p = FastIntPower(i);
	x /= p;
	y /= p;
	int width = container.width;
	int height = container.height;
	int xi = floor(x);
	int yi = floor(y);
	xi = ((xi % width) + width) % width;
	yi = ((yi % height) + height) % height;
	return container.colors[width * yi + xi];
}

/*
FColor TextureReadonlySource::GetColorBilinear(float x, float y) {
	int xi = floor(x);
	int yi = floor(y);
	float dx = xi - x;
	float dy = yi - y;
	xi = xi >= 0 ? xi % width : width + xi % width;
	yi = yi >= 0 ? yi % height : height + yi % height;
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
*/