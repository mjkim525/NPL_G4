#include "TBox.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TExec.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TLegend.h"
#include "TLine.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TTree.h"
#include "TPad.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <utility>
#include <map>
#include <vector>
#include <set>
#include <string>
using namespace std;

// Constants, FIXED
//-------------------------------------------------------------------------

const float AMU = 931.5; // Atomic mass unit, a unit = 1.6605 x 10^-27 kg = 931.5 MeV

const int pID_e = 11;		// electron
const int pID_g = 22;		// gamma
const int pID_n = 2112;		// neutron
const int pID_p = 2212;		// proton
const int pID_6Li = 1000030060;
const int pID_10B = 1000050100;
const int pID_11B = 1000050110;
const int pID_11C = 1000060110;
const int pID_12C = 1000060120;
const int pID_15O = 1000060150;
const int pID_16O = 1000060160;

vector <string> elTable = {"H","He","Li","Be","B","C","N","O"};
vector <int> COLOR = {1,2,4,6,31,8,9,11,12};

//-------------------------------------------------------------------------
int PDGtoIndex(const int pID)
{
	enum pIDorder {pro,neu,gam,ele,pos,ELS};
	int index;
	if		(pID == pID_p) index = pro;
	else if (pID == pID_n) index = neu;
	else if (pID == pID_g) index = gam;
	else if (pID == pID_e) index = ele;
	else if (pID ==-pID_e) index = pos;
	else				   index = ELS;
	return index;
}

//-------------------------------------------------------------------------
pair<int,int> PDGtoAZ(const int pID)
{
	int numA;
	int numZ;
	if		(pID <  pID_n) { numZ = 0; numA = 0; }
	else if	(pID == pID_n) { numZ = 0; numA = 1; }
	else if (pID == pID_p) { numZ = 1; numA = 0; }
	else	// for ions
	{
		const int numZA = pID%1000000000;
		numZ = (numZA / 10000);
		numA = (numZA % 10000)/10;
	}
	return pair<int,int>{numA,numZ};
}

//-------------------------------------------------------------------------
float PDGtoMass(const int pID) //for e-,e+, p, n, gamma, ions
{
	float mass;
	if		(pID == pID_e) mass = 0.511;	// e- [MeV]
	else if (pID ==-pID_e) mass = 0.511;	// e+
	else if (pID == pID_g) mass = 0.000;	// gamma
	else if (pID == pID_p) mass = 938.272;	// proton
	else if (pID == pID_n) mass = 939.562;	// neutron
	else	// for ion (w/o binding energy, just mass of A + mass of Z)
	{
		pair<int,int> numAZ = PDGtoAZ(pID);
		const int numA = numAZ.first;
//		const int numZ = numAZ.second();
		mass = numA*AMU;
	}
	return mass;
}
//-------------------------------------------------------------------------
string IndextoLabel(const int index)
{
	enum pIDorder {pro,neu,gam,ele,pos,ELS};
	string name;
	if		(index == pro) {name = "p";}
	else if (index == neu) {name = "n";}
	else if (index == ele) {name = "electron";}
	else if (index == pos) {name = "positron";}
	else if (index == gam) {name = "photon";}
	else				   {name = "ELS";}
	return name;
}
