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
		printf("Usage: %s <input.bmp> <output.bmp>\n", argv[0]);
		return 1;
	}

	FILE* fin = fopen(argv[1], "rb");

	if(!fin){
		perror("Error opening input file");
		return 1;
	}

	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fin);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fin);

	if(fileHeader.bfType != 0x4D42 || infoHeader.biBitCount != 24){
		printf("Only a 24-bit BMP file supported!\n");
		fclose(fin);
		return 1;
	}


	//moving the pixel array
	fseek(fin, fileHeader.bfOffsetBits, SEEK_SET);

	int width = infoHeader.biWidth;
	int height = infoHeader.biHeight;
	int rowSize = (width * 3 + 3) & ~3; // padded to multiple of 4
	uint8_t* row = malloc(rowSize);

	//output file
	FILE *fout = fopen(argv[2], "wb");
	if(!fout){
		perror("Error opening output file");
		free(row);
		fclose(fin);

		return 1;
	}

	fwrite(&fileHeader, sizeof(fileHeader), 1, fout);
	fwrite(&infoHeader, sizeof(infoHeader), 1, fout);

	//processsing pixel rows
	for(int y = 0; y < height; y++){ 
		fread(row, 1, rowSize, fin);
		
		for (int x = 0; x < width; x++){ 
			uint8_t b = row [x * 3 + 0];
			uint8_t g = row [x * 3 + 1];
			uint8_t r = row [x * 3 + 2];

			uint8_t gray = (r + g + b) / 3;

			row [x * 3 + 0] = gray;
			row [x * 3 + 1] = gray;
			row [x * 3 + 2] = gray;

		}

		fwrite(row, 1, rowSize, fout);

	}

	free(row);
	fclose(fin);
	fclose(fout);

	printf("Grayscale image written to %s\n", argv[2]);
	return 0;

}
