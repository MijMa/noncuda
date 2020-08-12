
//#include "cuda_runtime.h"
//#include "device_launch_parameters.h"
#include <stdio.h>
#include "pch.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

void annaData();
//int annaData(int x[]);
float muutaRadiaaneiksi(float a);
//int laskeGalaksit(int x);

const int N = 16;
const int blocksize = 16;
const float PI = atan(1) * 4;

int main()
{
	/*
	TODO: Parsi arvot tekstifilusta listoihin Real ja Imaginary - milloin lukea arvot? Laskemisen yhteydess‰? Mihin tallentaa
		Laske kaikille n‰ille arvoille vektoriarvot (GPU:lla)
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
	//Yritet‰‰n nyt aluksi tehd‰ radiaanifilu
	annaData();
}

//PALAUTTAA LIIAN PIENEN ARVON - Lasku 1 / 60 menee automaattisesti p‰in helvetti‰ 0.016... sijaan
float muutaRadiaaneiksi(float a) {
	//std::cout << a << "\n";
	//std::cout << (float)1 / (float)60 << "\n";
	//std::cout << a * (1 / 60) << " " << PI / 180;
	return a * ((float)1 / (float)60) * (PI / (float)180);
}

void annaData() {
	std::ifstream myfile("data_100k_arcmin.txt", std::ios_base::in);

	//std::ofstream radfile("radianvalues.txt");
	const int size = 100000;
	//Two dimensional matrix for array values, have to use 2 variables as hardware limits array size;
	//int* radArray = new int[size];
	//float radArray[1][size];
	//float radArrayL2[size];
	//float radArrayL1[size];
	std::vector<float> valueVec(100000);
	std::vector<std::vector<float> > radVec(2, valueVec);
	int increment = 0;
	
	std::string line;
	while (std::getline(myfile, line) && increment < 100000) {
		std::istringstream iss(line);
		float temp1, temp2;
		//Skip a line in the file if there's only one value in the line
		if (!(iss >> temp1 >> temp2)) { continue; }

		//radArrayL1[increment] = muutaRadiaaneiksi(temp1);
		//radArrayL2[increment] = muutaRadiaaneiksi(temp2);
		radVec[0][increment] = muutaRadiaaneiksi(temp1);
		radVec[1][increment] = muutaRadiaaneiksi(temp2); // <---THIS ONE IS THE PROBLEM!?

		if (increment == 6) {
			//std::cout << radArrayL2[0];
			//std::cout << "starting values:   " << temp1 << " " << temp2 << "\n";
			//std::cout << "Arvot radiaaneina: " << muutaRadiaaneiksi(temp1) << " " << muutaRadiaaneiksi(temp2) << "\n";
			//std::cout << typeid(temp1).name() << '\n';

			std::cout << radVec[0][0];
			std::cout << "starting values:   " << temp1 << " " << temp2 << "\n";
			std::cout << "Arvot radiaaneina: " << muutaRadiaaneiksi(temp1) << " " << muutaRadiaaneiksi(temp2) << "\n";
			std::cout << typeid(temp1).name() << '\n';
		}

		increment++;
	}
	std::cout << "Data transfer completed " << increment;
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

