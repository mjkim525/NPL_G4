#include "Make_G4.h"
#include <algorithm>
using namespace std;

void Make_G4()
{
    TFile* G = new TFile("Make_Out.root", "recreate");

    const int DetID = 1;
    const int Target_PDG = 2212;
    
    TH3D* Hist_Vx_Vy_KE = new TH3D("Hist_Vx_Vy_KE","",250,-250,250,250,-250,250,100,0,100);

    ifstream flist;
	char fname[300];
	snprintf(fname,300,"file.lst");
	flist.open(fname);

	while ( flist >> fname ){

		TFile* F = TFile::Open(fname);
		if (!F || F->IsZombie()){ 
			cout <<"Cannot open " <<fname <<endl;
            return; 
		}else{
			cout << "OPEN: " << fname << endl;
		}
		TTree* T = (TTree*)F->Get("G4sim");

        const int nEvents = T -> GetEntriesFast();

        int nTrack;
        T->SetBranchAddress("nTrack", &nTrack);

        int t_TrackDetID[10000];
        T->SetBranchAddress("TrackDetID",t_TrackDetID);
        int t_TrackPDG[10000];
        T->SetBranchAddress("TrackPDG",t_TrackPDG);

        double t_TrackKEnergy[10000];
        T->SetBranchAddress("TrackKEnergy",t_TrackKEnergy);

        double t_TrackVX[10000];
        T->SetBranchAddress("TrackVX",t_TrackVX);
        double t_TrackVY[10000];
        T->SetBranchAddress("TrackVY",t_TrackVY);

        for (int a=0; a < nEvents; a++)
        {
            T->GetEntry(a);

            vector<float> v_TrackKE;
            
            vector<float> v_TrackVX;
            vector<float> v_TrackVY;

            for(int b=0; b<nTrack; b++){

                if(t_TrackDetID[b] == DetID && t_TrackPDG[b] == 2112){
                    v_TrackKE.push_back(t_TrackKEnergy[b]);
                    v_TrackVX.push_back(t_TrackVX[b]);
                    v_TrackVY.push_back(t_TrackVY[b]);
                }

            }



            for(int b=0; b<v_TrackVX.size(); b++){
                Hist_Vx_Vy_KE -> Fill(v_TrackVX[b],v_TrackVY[b],v_TrackKE[b]);

            }


            v_TrackKE.clear();
            v_TrackVX.clear();
            v_TrackVY.clear();

        }
    
    F->Close();

    }


    G->Write();
	G->Close();




	return;


}
