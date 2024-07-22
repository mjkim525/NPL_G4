#include "Make_G4.h"
#include <algorithm>
using namespace std;

void Make_G4Step()
{
    TFile* G = new TFile("Make_Out_Step.root", "recreate");

    const int Index_PID = 6;
    
    TH3D* SC1_h_PID_Time_Edep;
    TH3D* SC2_h_PID_Time_Edep;

    SC1_h_PID_Time_Edep = new TH3D("SC1_h_PID_Time_Edep","",6,0,6,500,0,1000,200,0,20);
    SC2_h_PID_Time_Edep = new TH3D("SC2_h_PID_Time_Edep","",6,0,6,500,0,1000,200,0,20);


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

        int nStep;
        T->SetBranchAddress("nStep", &nStep);

        int t_StepTrackPDG[12000];
        T->SetBranchAddress("StepTrackPDG",t_StepTrackPDG);
        int t_StepDetID[12000];
        T->SetBranchAddress("StepDetID",t_StepDetID);
        double t_StepEdep[12000];
        T->SetBranchAddress("StepEdep",t_StepEdep);
        double t_Step_t[12000];
        T->SetBranchAddress("Step_t",t_Step_t);


        for (int a=0; a < nEvents; a++)
        {
            T->GetEntry(a);

            //for secondaries
			vector<int> v_StepDetID;
            vector<float> v_StepTrackPDG;
            vector<float> v_StepEdep;
            vector<float> v_Step_t;

            for(int b=0; b<nStep; b++){
                v_StepDetID.push_back(t_StepDetID[b]);
                v_StepTrackPDG.push_back(t_StepTrackPDG[b]);
                v_StepEdep.push_back(t_StepEdep[b]);
                v_Step_t.push_back(t_Step_t[b]);

            }



            for(int b=0; b<v_StepDetID.size(); b++){

                for(int c=0; c<Index_PID; c++){

                    if(v_StepDetID[b]==-4 && c == PDGtoIndex(v_StepTrackPDG[b]))
                        SC1_h_PID_Time_Edep -> Fill(c,v_Step_t[b],v_StepEdep[b]);

                    if(v_StepDetID[b]==-6 && c == PDGtoIndex(v_StepTrackPDG[b]))
                        SC2_h_PID_Time_Edep -> Fill(c,v_Step_t[b],v_StepEdep[b]);
                }

            }


            v_StepDetID.clear();
            v_StepTrackPDG.clear();
            v_StepEdep.clear();
            v_Step_t.clear();

        }
    
    F->Close();

    }


    G->Write();
	G->Close();




	return;


}
