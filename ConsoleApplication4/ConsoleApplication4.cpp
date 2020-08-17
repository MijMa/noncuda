
//#include "cuda_runtime.h"
//#include "device_launch_parameters.h"
#include <stdio.h>
#include "pch.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

std::vector<float> muutaKarteesiseksi();
std::vector<std::vector<float> > annaData();
//int annaData(int x[]);
float muutaRadiaaneiksi(float a);
//int laskeGalaksit(int x);

const int N = 16;
const int blocksize = 16;
const float PI = atan(1) * 4;

int main()
{
	/*
	1. -> Arvot arteesiseen koordinaattij‰rjestelm‰‰n
	2. -> Laske kulmat jokaiselle parille
	3. -> Tee histogremmi arvoista
	4. -> CPU:lla laske tilastoarvo
	*/
	//Yritet‰‰n nyt aluksi tehd‰ radiaanifilu
	std::vector<std::vector<float> > dataVektori = annaData();
	std::cout << dataVektori.size() << " [1][0] = " << dataVektori[1][0];
}

float muutaRadiaaneiksi(float a) {
	//std::cout << a << "\n";
	//std::cout << a * (1 / 60) << " " << PI / 180;
	return a * ((float)1 / (float)60) * (PI / (float)180);
}

//This function turns spherical coordinate values into carthesian ones
std::vector<float> muutaKarteesiseksi(float a, float b) {
	// a = theta(pistinveitsi alaspain), b = omega
	float x = sin(b) * cos(a);
	float y = sin(b) * sin(a);
	float z = cos(b);
	std::vector<float> karteesiVec = {x, y, z};
	return karteesiVec;
}

void printToFile(std::vector<float> a) {
	std::ofstream outFile("valuecheckfile.txt");
	for (const auto &e : a) {
		outFile << e << "\n";
	}
}

std::vector<std::vector<float> > annaData() {
	std::ifstream myfile("data_100k_arcmin.txt", std::ios_base::in);

	const int size = 1000;
	int increment = 0;
	//Two dimensional matrix for array values
	std::vector<float> valueVec(1000);
	std::vector<std::vector<float> > radVec(1000, valueVec);
	std::string line;

	while (std::getline(myfile, line) && increment < 1000) {
		std::istringstream iss(line);
		float temp1, temp2;

		//Skip a line in the file if there's only one value in the line
		if (!(iss >> temp1 >> temp2)) { continue; }

		//right ascension in radians turned into spherical coordinates
		temp1 = muutaRadiaaneiksi(temp1);
		//declination in radians = 90 - declination for the angle in spherical
		temp2 = 90 - muutaRadiaaneiksi(temp2);
		//than turned into carthesian coordinates
		radVec[increment] = muutaKarteesiseksi(temp1, temp2);
		printToFile(radVec[increment]); //  <-- NEGATIIVISIA ARVOJA, somethings real off mate
		increment++;
	}
	std::cout << "Data transfer completed " << increment;
	return radVec;
}

//apufunktio kulmien ja vektorien laskemiseen, kutsuu GPU:n laskufunktiota
int laskeGalaksit() {
	//Hellossa tarpeeksi tilaa sanalle world!
	char a[N] = "Hello \0\0\0\0\0\0";
	int b[N] = { 15, 10, 6, 0, -11, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	//pointteri muuttujien alustuksia, tarkoitettu Device-muuttujiksi GPU:lle
	char* ad;
	int* bd;
	//ehk‰ N on arrayn koko eli arrayn koko kertaa char-muuttujan bitit
	const int csize = N * sizeof(char);
	const int isize = N * sizeof(int);

	printf("%s", a);

	/**
	//noonni, varaappa charakterin pituinen muistialue kertaa indeksien m‰‰r‰ tilaa tuonne globaaliin muistiin GPU:lle
	cudaMalloc((void**)&ad, csize);
	cudaMalloc((void**)&bd, isize);
	//ja sitten kopioippa j ja b tuolle alueelle prosessorilta GPU:n globaaliin muistiin joka juuri alustettiin
	cudaMemcpy(ad, a, csize, cudaMemcpyHostToDevice);
	cudaMemcpy(bd, b, isize, cudaMemcpyHostToDevice);

	//block of threads, 16 threads in the block, currently hardcoded
	dim3 dimBlock(blocksize, 1);
	//grid of blocks that have 16 threads each, currently only 1 grid, hardcoded
	dim3 dimGrid(1, 1);
	//Kutsutaan helloa t‰mmˆsill‰ spekseill‰, ad ja bd ovat nyt muuttujia GPU:n muistissa
	hello << <dimGrid, dimBlock >> > (ad, bd);
	//nyt kopioidaan GPU:lta takaisin prosessorin muistiiin
	cudaMemcpy(a, ad, csize, cudaMemcpyDeviceToHost);
	//Free memory from GPU
	cudaFree(ad);
	cudaFree(bd);

	//rivi alaspain ja printtaa 'a'
	printf("%s\n", a);
	//cout << a;
	return EXIT_SUCCESS;
	*/
	return 1;
}

