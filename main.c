#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#pragma pack(push, 1)
typedef struct {
	uint16_t bfType;
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffsetBits;
} BITMAPFILEHEADER;

typedef struct {
	uint32_t biSize;
	int32_t biWidth;
	int32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t biXPelsPerMeter;
	int32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;

} BITMAPINFOHEADER;
#pragma pack(pop)

int main(int argc, char *argv[]){
	if(argc < 2){
		printf("Usage: %s <bmpfile>\n", argv[0]);
		return 1;
	}

	FILE* f = fopen(argv[1], "rb");

	if(!f){
		perror("Error opening file");
		return 1;
	}

	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, f);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, f);

	if(fileHeader.bfType != 0x4D42){
		printf("Not a BMP  file!\n");
		fclose(f);
		return 1;
	}


	printf("Width: %d px\n", infoHeader.biWidth);
	printf("Heigth: %d px\n", infoHeader.biHeight);
	printf("Bits per pixel: %d\n", infoHeader.biBitCount);

	if(infoHeader.biBitCount != 24){
		printf("Only 24-bit BMP supported in this demo.\n");
		fclose(f);
		return 1;
	}

	//moving the pixel array
	fseek(f, fileHeader.bfOffsetBits, SEEK_SET);

	int width = infoHeader.biWidth;
	int height = infoHeader.biHeight;
	int rowSize = (width * 3 + 3) & ~3; // padded to multiple of 4

	uint8_t* row = malloc(rowSize);

	printf("\nFirst few pixels:\n");
	for(int y = 0; y < (height < 3 ? height : 3); y++){ //reading the first 3 rows
		fread(row, 1, rowSize, f);
		for (int x = 0; x < (width < 5 ? width : 5 ); x++){ // printing the first 5 pixels
			//it uses BGR not RGB format
			uint8_t b = row [x * 3 + 0];
			uint8_t g = row [x * 3 + 1];
			uint8_t r = row [x * 3 + 2];
			printf("(%3d,%d,%3d) ", r, g, b);
		}
		printf("\n");
	}

	free(row);
	fclose(f);
	return 0;

}
