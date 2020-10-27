/*
Lopputy� er��lle �bo akademin kurssille, henkil�kohtaista osaamistani varten.
Tarkoitus olisi laskea 100000 kertaa 100000 vektoria, omine optimointeineen ja muunnoksineen.
Lopuksi viel� muuttaa ainakin laskeKulmat() -kutsu GPU:lla ajettavaksi ja tehd� optimointia kuten muuttujien tyyppien setvimist�.
Koodi ja kommentit suomeksi, koska v�hitellen tajusin ett� se tekee omasta ty�skentelyst�ni helpointa.
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
#include <numeric>

using namespace std;

vector<vector<float> > annaData(string a);
vector<vector <float >> laskeKulmat(vector<vector <float >> a, vector<vector <float >> b);
vector<int> laitaHistoGrammiin(vector<vector<float> > a);
vector<float> laskeJakauma(vector<int> DD, vector<int> DR, vector<int> RR);
float laskePistValKulma(vector<float> a, vector<float> b);
float muutaRadiaaneiksi(float a);
float muutaAsteiksi(float a);
vector<float> muutaKarteesiseksi(float a, float b);
void printToFile(vector<vector<float> > a);
void printtaaVektori(vector<int> a);
template<typename T>;
void printVector(const T& t);
//int laskeGalaksit(int x);



const int N = 16;
const int blocksize = 16;
const float PI = atan(1) * 4;

int main()
{
	/*
	1. -> Arvot arteesiseen koordinaattij�rjestelm��n
	2. -> Laske kulmat jokaiselle parille (GPU:lla)
	3. -> Tee histogrammi arvoista (ehk� GPU:lla)
	4. -> CPU:lla laske tilastoarvo
	*/
	vector<vector<float> > reaaliDataVektori = annaData("data_100k_arcmin.txt");
	vector<vector<float> > randomDataVektori = annaData("flat_100k_arcmin.txt");
    
	vector<vector<float> > kulmavektoriDD = laskeKulmat(reaaliDataVektori, reaaliDataVektori);
    vector<vector<float> > kulmavektoriDR = laskeKulmat(reaaliDataVektori, randomDataVektori);
    vector<vector<float> > kulmavektoriRR = laskeKulmat(randomDataVektori, randomDataVektori);
	
    vector<int> histogrammiVektoriDD = laitaHistoGrammiin(kulmavektoriDD);
    vector<int> histogrammiVektoriDR = laitaHistoGrammiin(kulmavektoriDR);
    vector<int> histogrammiVektoriRR = laitaHistoGrammiin(kulmavektoriRR);
    
    //Lasketaan kahden galaksijoukon arvojen distribuutio
    vector<float> distribuutiot = laskeJakauma(histogrammiVektoriDD, histogrammiVektoriDR, histogrammiVektoriRR);
    
    //printToFile(kulmavektoriDD);
	int kulmienMaara = (kulmavektoriDD[1].size() * kulmavektoriDD.size());
	cout << "Histogrammin DD koko:" << histogrammiVektoriDD.size() << "\n";
	cout << "Laskutoimituksia pitaisi olla 400 * 400 eli " << 400 * 400 << "\n";
	cout << "Kulma-alkioita on: " << kulmienMaara << "\n";
	cout << "Kulma-alkioita histogrammissa: " << accumulate(histogrammiVektoriDD.begin(), histogrammiVektoriDD.end(), 0) << "\n";
	printVector(distribuutiot);

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
		//right ascension in degrees turned into spherical coordinates
		temp1 = muutaRadiaaneiksi(muutaAsteiksi(temp1));
		//declination in degrees = 90 - declination for the angle in spherical
		temp2 = muutaRadiaaneiksi(90 - muutaAsteiksi(temp2));
		//than turned into carthesian coordinates
		mainVec[increment] = muutaKarteesiseksi(temp1, temp2);
		/*if (increment == (size - 1)) {
			cout << "ekat temp arvot: " << temp1 << " " << temp2;
			cout << "\n karteesiset temp arvot: ";
			cout << (mainVec[increment])[0] << " " << mainVec[increment][1] << " " << mainVec[increment][2] << "\n";
			cout << "\n lopullinen kulma [0][0]: ";
			cout << laskePistValKulma(mainVec[0], mainVec[0]) << "\n";
		}*/
		increment++;
	}
	cout << "Data transfer completed " << increment << "\n";
	return mainVec;
}

//Apufunktio joka laskee GPU:lla argumenttivektoriensa kaikkien pisteiden v�liset kulmat, laskenta-aika kasvaa parametrim��r�n neli�n�
//Ottaa kaksi vektoria arvoja karteesisessa koordinaattij�rjestelm�ss�
//Palauttaa kaksitasoisen vektorin joka on t�ynn� kulma-arvoja
vector<vector <float >> laskeKulmat (vector<vector <float >> karteesiArvot1, vector<vector <float >> karteesiArvot2) {

	vector<vector <float >> kulmaVektori(karteesiArvot1.size());

	for (vector<float> &a : karteesiArvot1) {
		for (int i = 0; karteesiArvot1.size() > i; i++) {
			kulmaVektori[i].push_back(laskePistValKulma(a, karteesiArvot2[i]));
		}
	}
	//cout << "\n Kulmavektorin ekan alkion koko: " << kulmaVektori[1].size();
	//cout << "\n Kulmavektorin koko: " << kulmaVektori.size();
	return kulmaVektori;
}

//Funktio joka saa parametrikseen kaksitasoisen vektorin ja jakaa sen arvot histogrammiin
//Palauttaa vektorin t�ynn� int arvoja
vector<int> laitaHistoGrammiin(vector<vector<float> > arvot) {
	// 1 sailio = 0.25 astetta. 90 asteen haitari siis 4*90
	int sailiot = 360;
	vector<int> histogrammiVektori;
	//Treshold on tarkoitettu t�m�nhetkisen histogrammi-indeksin tarkastelua varten
	float treshold = 0.00436332313;
	float currentTreshold = 0.00436332313;
	float prevTreshold = 0;
	
	for (int i = 0; i < sailiot; i++) {
		int j = 0;
		for (vector<float> &a : arvot) {
			for (float& b : a) {
				
				if (b < currentTreshold && b >= prevTreshold) { //annetaan toistaiseksi duplikaattien olla, tarkastetaan entrym��r�, sama nollille
					j++;
				}
			}

		}
		histogrammiVektori.push_back(j);
		//Siirryt��n tarkastelemaan seuraavaa histogrammin alkiota
		prevTreshold = prevTreshold + treshold;
		currentTreshold = currentTreshold + treshold;
		//cout << currentTreshold << " ";
	}
	return histogrammiVektori;
}

//Saa parametreikseen kolme histogrammivektoria t�ynn� int arvoja
//Palauttaa vektorina parametrivektoriensa jakauma-arvot v�lilt� -0.5-0.5
//Arvot l�hell� ykk�st� todistaisivat ett� meille on annettu satunnainen joukko oikeita galakseja
vector<float> laskeJakauma(vector<int> DD, vector<int> DR, vector<int> RR) {
	vector<float> arvoVektori;
	int hukkaArvot = 0;
    for (int i = 0; i < DD.size(); i++) {
		//if (RR[i] == 0) {
		//	hukkaArvot++;
		//	continue;
		//}
		float jakauma = (DD[i] - 2 * DR[i] + RR[i]) / (RR[i]);
		arvoVektori.push_back(jakauma);
    }
	//cout << "Hukattuja arvoja "<< hukkaArvot << "\n";
	return arvoVektori;
}

//funktio kahden pisteen v�lisen kulman laskemiselle karteesisessa koordinaattij�rjestelm�ss�.
//Saa argumenteikseen kaksi vektoria
float laskePistValKulma(vector<float> a, vector<float> b) {
	float x = acos(a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
	return x;
}

//funktio joka saa arvon asteina ja palauttaa sit� vastaavan arvon radiaaneina
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

template<typename T>
void printVector(const T& t) {
	std::copy(t.cbegin(), t.cend(), std::ostream_iterator<typename T::value_type>(std::cout, ", "));
}
/*
void printtaaVektori(vector<int> a) {
	cout << "Vektorin sisalto: \n";
	for (int i = 0; i < a.size(); i++) {
		cout << a[i] << ", ";
	}
}
*/
//apufunktio kulmien ja vektorien laskemiseen, kutsuu GPU:n laskufunktiota
int laskeGalaksit() {
	//Hellossa tarpeeksi tilaa sanalle world!
	char a[N] = "Hello \0\0\0\0\0\0";
	int b[N] = { 15, 10, 6, 0, -11, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	//pointteri muuttujien alustuksia, tarkoitettu Device-muuttujiksi GPU:lle
	char* ad;
	int* bd;
	//ehk� N on arrayn koko eli arrayn koko kertaa char-muuttujan bitit
	const int csize = N * sizeof(char);
	const int isize = N * sizeof(int);

	printf("%s", a);

	/**
	//noonni, varaappa charakterin pituinen muistialue kertaa indeksien m��r� tilaa tuonne globaaliin muistiin GPU:lle
	cudaMalloc((void**)&ad, csize);
	cudaMalloc((void**)&bd, isize);
	//ja sitten kopioippa j ja b tuolle alueelle prosessorilta GPU:n globaaliin muistiin joka juuri alustettiin
	cudaMemcpy(ad, a, csize, cudaMemcpyHostToDevice);
	cudaMemcpy(bd, b, isize, cudaMemcpyHostToDevice);

	//block of threads, 16 threads in the block, currently hardcoded
	dim3 dimBlock(blocksize, 1);
	//grid of blocks that have 16 threads each, currently only 1 grid, hardcoded
	dim3 dimGrid(1, 1);
	//Kutsutaan helloa t�mm�sill� spekseill�, ad ja bd ovat nyt muuttujia GPU:n muistissa
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