
//#include "cuda_runtime.h"
//#include "device_launch_parameters.h"
#include <stdio.h>
#include "pch.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

void annaData();
//int annaData(int x[]);
float muutaRadiaaneiksi(float x);
//int laskeGalaksit(int x);

const int N = 16;
const int blocksize = 16;
const float PI = atan(1) * 4;


int main()
{
	/*
	TODO: Parsi arvot tekstifilusta listoihin Real ja Imaginary - milloin lukea arvot? Laskemisen yhteydessä? Mihin tallentaa
		Laske kaikille näille arvoille vektoriarvot (GPU:lla)
			Tee histogrammi, vertaile, vertaile laskemalla.
	#include <fstream>
	int main(int argc, char * argv[])
	{
		std::fstream myfile("D:\\data.txt", std::ios_base::in);
		float a;
		while (myfile >> a)
		{
		printf("%f ", a);
		}
		getchar();
		return 0;
	int skipped = 1233;
	for (int i = 0; i < skipped; i++){
		float tmp;
		myfile >> tmp;
	}
		myfile >> value;
	}
	*/
	//Yritetään nyt aluksi tehdä radiaanifilu
	annaData();
}

float muutaRadiaaneiksi(float a) {
	return (float)(a * (float)(1 / 60) * (float)(PI / 180));
}
void annaData() {
	//TÄMÄ FILU PUUTTUU GITISTÄ
	std::fstream myfile("data_100k_arcmin.txt", std::ios_base::in);

	std::ofstream radfile("radianvalues.txt");
	const int N = 100;
	float a[N];
	float temp1 = 0;
	float temp2 = 0;
	int increment = 0;
	//GETLINE -> ´radiaanita -> laita filuun
	while (myfile >> temp1 >> temp2) {

		printf("%f ", temp1, " ", temp2);
		printf("%f ", muutaRadiaaneiksi(temp1), " ", muutaRadiaaneiksi(temp2));
		radfile << muutaRadiaaneiksi(temp1) << muutaRadiaaneiksi(temp2);

		increment++;
		if (increment > 10) {
			return;
		}
	}
	getchar();
	return;

	/*
	int skipped = 1233;
	for (int i = 0; i < skipped; i++) {
		float tmp;
		myfile >> tmp;
	}
	if (myfile.is_open()) {
		myfile << "This is a line.\n";
		myfile << "This is another line.\n";
		myfile.close();
	}
	else cout << "Unable to open file";
	return 'a';
	*/
}

//apufunktio kulmien ja vektorien laskemiseen, kutsuu GPU:n laskufunktiota
int laskeGalaksit() {
	//Hellossa tarpeeksi tilaa sanalle world!
	char a[N] = "Hello \0\0\0\0\0\0";
	int b[N] = { 15, 10, 6, 0, -11, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	//pointteri muuttujien alustuksia, tarkoitettu Device-muuttujiksi GPU:lle
	char* ad;
	int* bd;
	//ehkä N on arrayn koko eli arrayn koko kertaa char-muuttujan bitit
	const int csize = N * sizeof(char);
	const int isize = N * sizeof(int);

	printf("%s", a);

	/**
	//noonni, varaappa charakterin pituinen muistialue kertaa indeksien määrä tilaa tuonne globaaliin muistiin GPU:lle
	cudaMalloc((void**)&ad, csize);
	cudaMalloc((void**)&bd, isize);
	//ja sitten kopioippa j ja b tuolle alueelle prosessorilta GPU:n globaaliin muistiin joka juuri alustettiin
	cudaMemcpy(ad, a, csize, cudaMemcpyHostToDevice);
	cudaMemcpy(bd, b, isize, cudaMemcpyHostToDevice);

	//block of threads, 16 threads in the block, currently hardcoded
	dim3 dimBlock(blocksize, 1);
	//grid of blocks that have 16 threads each, currently only 1 grid, hardcoded
	dim3 dimGrid(1, 1);
	//Kutsutaan helloa tämmösillä spekseillä, ad ja bd ovat nyt muuttujia GPU:n muistissa
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

