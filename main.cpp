#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool imageToData(const char file[], unsigned char *image, int x, int y, int comp);

int main(int argc, char *argv[])
{
	//Check to make sure file was given.
	if (argc == 1)
	{
		cout << "No filed specified for converting!\n"
			<< "Drag a file onto the exe or specify it in commandline."
			<< endl;
		return 1;
	}
	//and not more
	if (argc > 2)
	{
		cout << "More parameters specified than supported!" << endl;
		return 1;
	}

	//Load image
	char *filename = argv[argc - 1];
	unsigned char *image = NULL;
	int x = 0;
	int y = 0;
	int comp = 0;
	image = stbi_load(filename, &x, &y, &comp, 0);
	if (image == NULL)
	{
		cout << "Tried to load image, does not exist!" << endl;
		return 1;
	}

	string newFile(filename);
	unsigned int dotLoc = newFile.find_last_of('.');
	newFile = newFile.substr(0, newFile.size() - dotLoc) + ".e";
	if (imageToData(newFile.c_str(), image, x, y, comp))
	{
		cout << "Error occured while exporting!" << endl;
		stbi_image_free(image);
		return 1;
	}

	cout << x << " " << y << " " << comp << endl;
	for (int j = 0; j < y; j++)
	{
		for (int i = 0; i < x; i++)
		{
			cout << "0x";
			for (int k = 0; k < comp; k++)
				cout << setw(2) << setfill('0') << hex << (int)image[j*x*comp + i*comp + k];
			cout << " ";
		}
		cout << endl;
	}



	system("pause");

	stbi_image_free(image);
	return 0;
}

bool imageToData(const char file[], unsigned char *image, int x, int y, int comp)
{
	ofstream outFile;
	outFile.open(file);
	if (!outFile.good())
		return true;

	//Write address of label, width and height
	outFile << "modifyLabel\t.data\t" << x << endl;
	outFile << "\t.data\t" << y << endl;

	//Loop through all data, assemble words
	long pixel = 0;
	bool pixelFlip = 0;
	for (int j = 0; j < y; j++)
	{
		for (int i = 0; i < x; i++)
		{
			pixel = pixel << 1;
			for (int k = 0; k < comp; k++)
			{
				int color = image[j*x*comp + i*comp + k] / 8;
				pixel = pixel << 5;
				pixel = pixel | color;
				//cout << setw(2) << setfill('0') << hex << (int)image[j*x*comp + i*comp + k];
			}
			pixelFlip = !pixelFlip;
			if (!pixelFlip)
			{
				outFile << "\t.data\t" << pixel << endl;
				pixel = 0;
			}
		}
	}
	if (pixelFlip)
	{
		pixel = pixel << 16;
		outFile << "\t.data\t" << pixel << endl;
	}

	outFile.close();
	return false;
}