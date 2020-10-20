/*
Lopputyö eräälle åbo akademin kurssille, henkilökohtaista osaamistani varten.
Tarkoitus olisi laskea 100000 kertaa 100000 vektoria, omine optimointeineen ja muunnoksineen.
Lopuksi vielä muuttaa ainakin laskeKulmat() -kutsu GPU:lla ajettavaksi ja tehdä optimointia kuten muuttujien tyyppien setvimistä.
Koodi ja kommentit suomeksi, koska vähitellen tajusin että se tekee omasta työskentelystäni helpointa.
*/

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

vector<vector<float> > annaData(string a);
vector<vector <float >> laskeKulmat(vector<vector <float >> a, vector<vector <float >> b);
vector<int> laitaHistoGrammiin(vector<vector<float> > a);
float laskeJakauma(vector<int> DD, DR, RR);
float laskePistValKulma(vector<float> a, vector<float> b);
float muutaRadiaaneiksi(float a);
float muutaAsteiksi(float a);
vector<float> muutaKarteesiseksi(float a, float b);
void printToFile(vector<vector<float> > a);
void printtaaVektori(vector<int> a);
//int laskeGalaksit(int x);

const int N = 16;
const int blocksize = 16;
const float PI = atan(1) * 4;

int main()
{
	/*
	1. -> Arvot arteesiseen koordinaattijärjestelmään
	2. -> Laske kulmat jokaiselle parille (GPU:lla)
	3. -> Tee histogrammi arvoista (ehkä GPU:lla)
	4. -> CPU:lla laske tilastoarvo
	*/
	cout << 0.00436332313 * 720 << "\n";
	vector<vector<float> > reaaliDataVektori = annaData("data_100k_arcmin.txt");
	vector<vector<float> > randomDataVektori = annaData("flat_100k_arcmin.txt");
    
	vector<vector<float> > kulmavektoriDD = laskeKulmat(reaaliDataVektori, reaaliDataVektori);
    vector<vector<float> > kulmavektoriDR = laskeKulmat(reaaliDataVektori, randomDataVektori);
    vector<vector<float> > kulmavektoriRR = laskeKulmat(randomDataVektori, randomDataVektori);
	
    vector<int> histogrammiVektoriDD = laitaHistoGrammiin(kulmavektoriDD);
    vector<int> histogrammiVektoriDR = laitaHistoGrammiin(kulmavektoriDR);
    vector<int> histogrammiVektoriRR = laitaHistoGrammiin(kulmavektoriRR);
    
    //Lasketaan kahden kalaksijoukon arvojen distribuutio
    vector<int> distribuutiot = laskeJakauma(histogrammiVektoriDD, histogrammiVektoriDR, histogrammiVektoriRR);
    
    //printToFile(kulmavektoriDD);
	cout << "\n Histogrammin koko:" << histogrammiVektori.size();
	printtaaVektori(histogrammiVektori);
}

//Apufunktio alkuperaisten arvojen lukemiseen tiedostosta ja preprosessointi karteesisiksi arvoiksi.
//Palauttaa vektorin jossa on alivektoreissa karteesiset arvot (x, y, z) radiaaneina.
vector<vector<float> > annaData(string filename) {
	ifstream myfile(filename, ios_base::in);

	const int size = 400;
	int increment = 0;
	//Two dimensional matrix for array values
	vector<float> valueVec(size);
	vector<vector<float> > mainVec(size, valueVec);
	string line;

	while (getline(myfile, line) && increment < size) {
		istringstream iss(line);
		float temp1, temp2;

		//Skip a line in the file if there's only one value in the line
		if (!(iss >> temp1 >> temp2)) { continue; }
		if (increment == 1) {
			cout << "ekat temp arvot: " << temp1 << " " << temp2;
		}
		//right ascension in degrees turned into spherical coordinates
		temp1 = muutaRadiaaneiksi(muutaAsteiksi(temp1));
		//declination in degrees = 90 - declination for the angle in spherical
		temp2 = muutaRadiaaneiksi(90 - muutaAsteiksi(temp2));
		//than turned into carthesian coordinates
		mainVec[increment] = muutaKarteesiseksi(temp1, temp2);
		if (increment == (size - 1)) {
			cout << "ekat temp arvot: " << temp1 << " " << temp2;
			cout << "\n karteesiset temp arvot: ";
			cout << (mainVec[increment])[0] << " " << mainVec[increment][1] << " " << mainVec[increment][2] << "\n";
			cout << "\n lopullinen kulma [0][0]: ";
			cout << laskePistValKulma(mainVec[0], mainVec[0]) << "\n";
		}
		increment++;
	}
	cout << "Data transfer completed " << increment;
	cout << " size of vector: " << mainVec.size() << " and the first value: " << mainVec[0].size();
	return mainVec;
}

//Apufunktio joka laskee GPU:lla argumenttivektoriensa kaikkien pisteiden väliset kulmat
//Ottaa kaksi vektoria arvoja karteesisessa koordinaattijärjestelmässä
//Palauttaa kaksitasoisen vektorin joka on täynnä kulma-arvoja
vector<vector <float >> laskeKulmat (vector<vector <float >> karteesiArvot1, vector<vector <float >> karteesiArvot2) {

	vector<vector <float >> kulmaVektori(karteesiArvot1.size());

	for (vector<float> &a : karteesiArvot1) {
		for (int i = 0; karteesiArvot1.size() > i; i++) {
			kulmaVektori[i].push_back(laskePistValKulma(a, karteesiArvot2[i]));
		}
	}
	cout << "\n Kulmavektorin ekan alkion koko: " << kulmaVektori[1].size();
	cout << "\n Kulmavektorin koko: " << kulmaVektori.size();
	return kulmaVektori;
}

//Funktio joka saa parametrikseen kaksitasoisen vektorin ja jakaa sen arvot histogrammiin
//Palauttaa vektorin täynnä int arvoja
vector<int> laitaHistoGrammiin(vector<vector<float> > arvot) {
	// 1 sailio = 0.25 astetta. 180 asteen haitari siis 4*180
	int sailiot = 720;
	vector<int> histogrammiVektori;
	//Treshold on tarkoitettu tämänhetkisen histogrammi-indeksin tarkastelua varten
	float treshold = 0.00436332313;
	float currentTreshold = 0.00436332313;
	float prevTreshold = 0;
	
	for (int i = 0; i < 720; i++) {
		int j = 0;
		for (vector<float> &a : arvot) {
			for (float& b : a) {
				
				if (b < currentTreshold && b >= prevTreshold) { //annetaan toistaiseksi duplikaattien olla, tarkastetaan entrymäärä, sama nollille
					j++;
				}
			}

		}
		histogrammiVektori.push_back(j);
		//Siirrytään tarkastelemaan seuraavaa histogrammin alkiota
		prevTreshold = prevTreshold + treshold;
		currentTreshold = currentTreshold + treshold;
		cout << currentTreshold << " ";
	}
	return histogrammiVektori;
}

float laskeJakauma(vector<int> DD, DR, RR) {
    
    for (int i = 0; i < dd.length(); i++) {
        (DD[i] - 2*DR[i] + RR[i])/(RR[i])
    }
    
	return 0.1;
}

//funktio kahden pisteen välisen kulman laskemiselle karteesisessa koordinaattijärjestelmässä.
//Saa argumenteikseen kaksi vektoria
float laskePistValKulma(vector<float> a, vector<float> b) {
	float x = acos(a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
	return x;
}

//funktio joka saa arvon asteina ja palauttaa sitä vastaavan arvon radiaaneina
float muutaRadiaaneiksi(float a) {
	return a * (PI / (float)180);
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

void printtaaVektori(vector<int> a) {
	cout << "Vektorin sisalto: \n";
	for (int i = 0; i < a.size(); i++) {
		cout << a[i] << ", ";
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

