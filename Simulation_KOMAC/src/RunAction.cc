#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#include "ParameterContainer.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <utility>
#include <algorithm>
#include <functional>
#include <map>


RunAction::RunAction(ParameterContainer* par)
: G4UserRunAction(),
Time_Window(ParameterContainer::GetInstance()->GetParDouble("Time_Window")*CLHEP::ns),
	PC(par)
{ 
	F = new TFile(PC -> GetParString("OutName"),"recreate");
	T = new TTree("G4sim","G4sim");

	init_Tree();
}


RunAction::~RunAction()
{
//	PC -> PrintParameter("ALL");
	map<G4String,G4String> map_input_para = PC->GetInputParameters(nevnts);
	for(auto iter : map_input_para)
	{
		G4String parName = iter.first;
		G4String parValue = iter.second;
		TNamed* input_par = new TNamed(parName.data(),parValue.data());
		fInputParameters.Add(input_par);
	}
	fInputParameters.Write("InputParameters",TObject::kSingleKey);
	F -> Write();
	F -> Close();
}


void RunAction::init_Tree()
{
	G4bool bMCTrack = PC -> GetParBool("MCTrack");
	G4bool bMCPostTrack = PC -> GetParBool("MCPostTrack");
	G4bool bStep = PC -> GetParBool("MCStep");

	if(bMCTrack)
	{
		T -> Branch("nTrack",&nTrack);
		T -> Branch("TrackID",TrackID,"TrackID[nTrack]/I");
		T -> Branch("ParentID",ParentID,"ParentID[nTrack]/I");
		T -> Branch("TrackPDG",TrackPDG,"TrackPDG[nTrack]/I");
		T -> Branch("TrackDetID",TrackDetID,"TrackDetID[nTrack]/I");

		T -> Branch("TrackPX",TrackPX,"TrackPX[nTrack]/D");
		T -> Branch("TrackPY",TrackPY,"TrackPY[nTrack]/D");
		T -> Branch("TrackPZ",TrackPZ,"TrackPZ[nTrack]/D");
		T -> Branch("TrackVX",TrackVX,"TrackVX[nTrack]/D");
		T -> Branch("TrackVY",TrackVY,"TrackVY[nTrack]/D");
		T -> Branch("TrackVZ",TrackVZ,"TrackVZ[nTrack]/D");
		T -> Branch("TrackEnergy",TrackEnergy,"TrackEnergy[nTrack]/D");
		T -> Branch("TrackKEnergy",TrackKEnergy,"TrackKEnergy[nTrack]/D");
		T -> Branch("TrackTime",TrackTime,"TrackTime[nTrack]/D");
	}
	if(bMCPostTrack)
	{
		T -> Branch("nPostTrack",&nPostTrack);
		T -> Branch("PostTrackID",PostTrackID,"PostTrackID[nPostTrack]/I");
		T -> Branch("PostTrackPDG",PostTrackPDG,"PostTrackPDG[nPostTrack]/I");
		T -> Branch("PostTrackDetID",PostTrackDetID,"PostTrackDetID[nPostTrack]/I");

		T -> Branch("PostTrackPX",PostTrackPX,"PostTrackPX[nPostTrack]/D");
		T -> Branch("PostTrackPY",PostTrackPY,"PostTrackPY[nPostTrack]/D");
		T -> Branch("PostTrackPZ",PostTrackPZ,"PostTrackPZ[nPostTrack]/D");
		T -> Branch("PostTrackVX",PostTrackVX,"PostTrackVX[nPostTrack]/D");
		T -> Branch("PostTrackVY",PostTrackVY,"PostTrackVY[nPostTrack]/D");
		T -> Branch("PostTrackVZ",PostTrackVZ,"PostTrackVZ[nPostTrack]/D");
		T -> Branch("PostTrackEnergy",PostTrackEnergy,"PostTrackEnergy[nPostTrack]/D");
		T -> Branch("PostTrackKEnergy",PostTrackKEnergy,"PostTrackKEnergy[nPostTrack]/D");
		T -> Branch("PostTrackTime",PostTrackTime,"PostrackTime[nTrack]/D");
	}
	if(bStep)
	{
		T -> Branch("nStep",&nStep);
		T -> Branch("StepTrackID",StepTrackID,"StepTrackID[nStep]/I");
		T -> Branch("StepTrackPDG",StepTrackPDG,"StepTrackPDG[nStep]/I");
		T -> Branch("StepDetID",StepDetID,"StepDetID[nStep]/I");

		T -> Branch("StepVX",StepVX,"StepVX[nStep]/D");
		T -> Branch("StepVY",StepVY,"StepVY[nStep]/D");
		T -> Branch("StepVZ",StepVZ,"StepVZ[nStep]/D");
		T -> Branch("StepEdep",StepEdep,"StepEdep[nStep]/D");
		T -> Branch("StepKE_Pre",StepKE_Pre,"StepKE_Pre[nStep]/D");
		T -> Branch("Step_t",Step_t,"Step_t[nStep]/D");
		T -> Branch("StepDetID",StepDetID,"StepDetID[nStep]/I");
		T -> Branch("StepModelIndex",StepModelIndex,"StepModelIndex[nStep]/I");
		T -> Branch("StepParentID",StepParentID,"StepParentID[nStep]/I");
		
	}
}

void RunAction::BeginOfRunAction(const G4Run*)
{ 
}

void RunAction::EndOfRunAction(const G4Run*)
{
}

void RunAction::clear_data()
{
	nTrack = 0;
	fill_n(TrackID,max_tracks,0);
	fill_n(ParentID,max_tracks,0);
	fill_n(TrackPDG,max_tracks,0);
	fill_n(TrackDetID,max_tracks,0);
	fill_n(TrackPX,max_tracks,0);
	fill_n(TrackPY,max_tracks,0);
	fill_n(TrackPZ,max_tracks,0);
	fill_n(TrackVX,max_tracks,0);
	fill_n(TrackVY,max_tracks,0);
	fill_n(TrackVZ,max_tracks,0);
	fill_n(TrackEnergy,max_tracks,0);
	fill_n(TrackKEnergy,max_tracks,0);
	fill_n(TrackTime,max_tracks,0);

	nPostTrack = 0;
	fill_n(PostTrackID,max_tracks,0);
	fill_n(PostTrackPDG,max_tracks,0);
	fill_n(PostTrackDetID,max_tracks,0);
	fill_n(PostTrackPX,max_tracks,0);
	fill_n(PostTrackPY,max_tracks,0);
	fill_n(PostTrackPZ,max_tracks,0);
	fill_n(PostTrackVX,max_tracks,0);
	fill_n(PostTrackVY,max_tracks,0);
	fill_n(PostTrackVZ,max_tracks,0);
	fill_n(PostTrackEnergy,max_tracks,0);
	fill_n(PostTrackKEnergy,max_tracks,0);
	fill_n(PostTrackTime,max_tracks,0);

	nStep = 0;
	fill_n(StepTrackID,max_steps,0);
	fill_n(StepTrackPDG,max_steps,0);
	fill_n(StepDetID,max_steps,0);
	fill_n(StepVX,max_steps,0);
	fill_n(StepVY,max_steps,0);
	fill_n(StepVZ,max_steps,0);
	fill_n(StepEdep,max_steps,0);
	fill_n(StepKE_Pre,max_steps,0);
	fill_n(Step_t,max_steps,0);
	fill_n(StepModelIndex,max_steps,0);
	fill_n(StepParentID,max_steps,0);
	//EdepSumBox = 0;
}

void RunAction::FillTrack
(G4int opt, G4int trkID, G4int parentID, G4int pdg, G4int detID,
 G4ThreeVector p, G4ThreeVector v, G4double totenergy, G4double kinenergy, G4double time)
	// invoked in TrackingAction
{
	if(opt == MCTrack)
	{
		TrackID[nTrack] = trkID;
		ParentID[nTrack] = parentID;
		TrackPDG[nTrack] = pdg;
		TrackDetID[nTrack] = detID;
		TrackPX[nTrack] = p.x();
		TrackPY[nTrack] = p.y();
		TrackPZ[nTrack] = p.z();
		TrackVX[nTrack] = v.x();
		TrackVY[nTrack] = v.y();
		TrackVZ[nTrack] = v.z();
		TrackEnergy[nTrack] = totenergy;
		TrackKEnergy[nTrack] = kinenergy;
		TrackTime[nTrack] = time;
		nTrack++;
	}
	else if(opt == MCPostTrack)
	{
		PostTrackID[nPostTrack] = trkID;
		PostTrackPDG[nPostTrack] = pdg;
		PostTrackDetID[nPostTrack] = detID;
		PostTrackPX[nPostTrack] = p.x();
		PostTrackPY[nPostTrack] = p.y();
		PostTrackPZ[nPostTrack] = p.z();
		PostTrackVX[nPostTrack] = v.x();
		PostTrackVY[nPostTrack] = v.y();
		PostTrackVZ[nPostTrack] = v.z();
		PostTrackEnergy[nPostTrack] = totenergy;
		PostTrackKEnergy[nPostTrack] = kinenergy;
		PostTrackTime[nPostTrack] = time;
		nPostTrack++;
	}
	else
	{
		G4cout << "Error in RunAction::FillTrack()" << G4endl;
	}
}

void RunAction::FillStep
(G4int trkID, G4int pdg, G4int prev_detID, G4int post_detID, G4ThreeVector v, G4double edep, G4double KE_Pre, G4double Step_dt, G4int Index_Model, G4int ID_Parent)
{
	// if(prev_detID==-4){
	// 	if(nStep>0){
	// 		if(Time_Trg1>0 && Step_t_Trg1[nStep-1] >0 ){
	// 			if(Time_Trg1>Step_t_Trg1[nStep-1]+Time_Window)
	// 			Step_t_Trg1[nStep]=Time_Trg1;
	// 			else
	// 			Step_t_Trg1[nStep]=Step_t_Trg1[nStep-1];
	// 		}
	// 		else{
	// 			Step_t_Trg1[nStep]=Step_t_Trg1[nStep-1];
	// 		}

	// 			StepDetID[nStep] = prev_detID;
	// 			StepTrackID[nStep] = trkID;
	// 			StepTrackPDG[nStep] = pdg;
	// 			StepVX[nStep] = v.x();
	// 			StepVY[nStep] = v.y();
	// 			StepVZ[nStep] = v.z();
	// 			StepEdep[nStep] = edep;
	// 			Step_t[nStep] = Step_dt;
		
	// 		}
	
	// 	else{
	// 		if(Time_Trg1>0)
	// 			Step_t_Trg1[nStep]=Time_Trg1;
	// 		else
	// 			Step_t_Trg1[nStep]=0;
				
	// 		StepDetID[nStep] = prev_detID;
	// 		StepTrackID[nStep] = trkID;
	// 		StepTrackPDG[nStep] = pdg;
	// 		StepVX[nStep] = v.x();
	// 		StepVY[nStep] = v.y();
	// 		StepVZ[nStep] = v.z();
	// 		StepEdep[nStep] = edep;
	// 		Step_t[nStep] = Step_dt;

	// 	}

	// 	nStep++;

	// }

	StepDetID[nStep] = prev_detID;
	StepTrackID[nStep] = trkID;
	StepTrackPDG[nStep] = pdg;
	StepVX[nStep] = v.x();
	StepVY[nStep] = v.y();
	StepVZ[nStep] = v.z();
	StepEdep[nStep] = edep;
	StepKE_Pre[nStep] = KE_Pre;
	Step_t[nStep] = Step_dt;
	StepModelIndex[nStep] = Index_Model;
	StepParentID[nStep] = ID_Parent;
	nStep++;
}
