
//#include "cuda_runtime.h"
//#include "device_launch_parameters.h"
#include <stdio.h>
#include "pch.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

vector<vector<float> > annaData();
vector<vector <float >> laskeKulmat(vector<vector <float >> a, vector<vector <float >> b);
int laitaHistoGrammiin();
float laskePistValKulma(vector<float> a, vector<float> b);
float muutaRadiaaneiksi(float a);
float muutaAsteiksi(float a);
vector<float> muutaKarteesiseksi(float a, float b);
void printToFile(vector<vector<float> > a);
//int laskeGalaksit(int x);

const int N = 16;
const int blocksize = 16;
const float PI = atan(1) * 4;

int main()
{
	/*
	1. -> Arvot arteesiseen koordinaattijärjestelmään
	2. -> Laske kulmat jokaiselle parille
	3. -> Tee histogrammi arvoista
	4. -> CPU:lla laske tilastoarvo
	*/

	vector<vector<float> > dataVektori = annaData();
	printToFile(laskeKulmat(dataVektori, dataVektori));
	cout << dataVektori.size() << " [1][0] = " << dataVektori[1][0];
}

//Apufunktio alkuperaisten arvojen lukemiseen tiedostosta ja preprosessointi karteesisiksi arvoiksi.
//Palauttaa vektorin jossa on alivektoreissa karteesiset arvot (x, y, z).
vector<vector<float> > annaData() {
	ifstream myfile("data_100k_arcmin.txt", ios_base::in);

	const int size = 1000;
	int increment = 0;
	//Two dimensional matrix for array values
	vector<float> valueVec(1000);
	vector<vector<float> > mainVec(1000, valueVec);
	string line;

	while (getline(myfile, line) && increment < 1000) {
		istringstream iss(line);
		float temp1, temp2;

		//Skip a line in the file if there's only one value in the line
		if (!(iss >> temp1 >> temp2)) { continue; }

		//right ascension in degrees turned into spherical coordinates
		temp1 = muutaAsteiksi(temp1);
		//declination in degrees = 90 - declination for the angle in spherical
		temp2 = 90 - muutaAsteiksi(temp2);
		//than turned into carthesian coordinates
		mainVec[increment] = muutaKarteesiseksi(temp1, temp2);
		increment++;
	}
	//printToFile(mainVec); //  <-- NEGATIIVISIA ARVOJA, saattaa olla sopimaton.
	cout << "Data transfer completed " << increment;
	cout << " size of vector: " << mainVec.size() << " and the first value: " << mainVec[0].size();
	return mainVec;
}

//Apufunktio joka laskee GPU:lla argumenttivektoriensa kaikkien pisteiden väliset kulmat
//Ottaa kaksi vektoria arvoja karteesisessa koordinaattijärjestelmässä
//Palauttaa vektorin joka on täynnä kulma-arvoja
vector<vector <float >> laskeKulmat (vector<vector <float >> karteesiArvot1, vector<vector <float >> karteesiArvot2) {

	vector<vector <float >> kulmaVektori(karteesiArvot1.size());

	for (vector<float> &a : karteesiArvot1) {
		for (int i = 0; karteesiArvot1.size() > i; i++) {
			//A ON NYT SIIS PERUS KOLMEN VEKTORI, verrataan A:ta jokaiseen muuhun vektoriin(paitsi a:han) a X 100000
			kulmaVektori[i].push_back(laskePistValKulma(a, karteesiArvot2[i]));
		}
	}
	cout << "\n Kulmavektorin ekan alkion koko: " << kulmaVektori[1].size();
	cout << "\n Kulmavektorin koko: " << kulmaVektori.size();
	return kulmaVektori;
}

//Saa satoja tuhansia kulma-arvoja? -joo vektoriin vaan ja sit tälle kutsulle
//TÄÄ GPU:LLA
int laitaHistoGrammiin(vector<vector<float> > arvot) {
	int sailiot = 720;
	// 1 sailio = 0.25 astetta. 180 asteen haitari
	vector<int> histogrammi[720];
	
	//jokainen vektorin arvo pitäisi kertoa jokaisella toisella vektorin arvolla
	/*
	for (int i : arvot) {
		for (int z : arvot) {
		laskePistValKulma(i, z);
		}
	}
	*/
	return 0;
}

//funktio kahden pisteen välisen kulman laskemiselle karteesisessa koordinaattijärjestelmässä.
float laskePistValKulma(vector<float> a, vector<float> b) {
	float x = acos(a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
	return x;
}

float muutaRadiaaneiksi(float a) {
	//cout << a << "\n";
	//cout << a * (1 / 60) << " " << PI / 180;
	return a * ((float)1 / (float)60) * (PI / (float)180);
}

float muutaAsteiksi(float a) {
	return(a / 60);
}

//This function turns spherical coordinate values into carthesian ones
//Returns a vector with three values; x, y, z
vector<float> muutaKarteesiseksi(float a, float b) {
	// a = theta(pistinveitsi alaspain), b = omega
	float x = sin(b) * cos(a);
	float y = sin(b) * sin(a);
	float z = cos(b);
	vector<float> karteesiVec = {x, y, z};
	return karteesiVec;
}

void printToFile(vector<vector<float> > a) {
	ofstream outFile("valuecheckfile.txt");
	for (const auto &b : a) {
		int i = 0;
		for (const auto &c : b) {
			outFile << c << " ";
			i++;
			if (i == 3){
				outFile << "\n";
			}
		}
	}
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

