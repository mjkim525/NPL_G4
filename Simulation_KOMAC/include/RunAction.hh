#ifndef RunAction_h
#define RunAction_h 1

#define max_tracks 3000
#define max_steps  10000

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include "TFile.h"
#include "TTree.h"

#include <vector>
#include <map>
using namespace std;

class ParameterContainer;

class G4Run;

enum {MCTrack,MCPostTrack};	// Opt for FillTrack

class RunAction : public G4UserRunAction
{
	public:
    RunAction(ParameterContainer* par);
   	virtual ~RunAction();

		void init_Tree();

    // virtual G4Run* GenerateRun();
   	virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);

		void clear_data();

		void FillTrack
			(G4int opt, G4int trkID, G4int parentID, G4int pdg, G4int detID,
			 G4ThreeVector p, G4ThreeVector v, G4double totenergy, G4double kinenergy, G4double time);

		void FillStep
			(G4int trkID, G4int pdg, G4int prev_detID, G4int post_detID,
			 G4ThreeVector v, G4double edep, G4double KE_Pre, G4double Step_dt, G4int Index_Model, G4int ID_Parent);

		void update_Tree()
			{T -> Fill();}

		void CountEvent()
			{nevnts++;}

	private:
		ParameterContainer* PC;

		TFile* F;
		TTree* T;
		TList fInputParameters;

		G4int nevnts;
		G4double Time_Window;

		// Track data
		G4int nTrack;
		G4int TrackID[max_tracks];
		G4int ParentID[max_tracks];
		G4int TrackPDG[max_tracks];
		G4int TrackDetID[max_tracks];
		G4double TrackPX[max_tracks];
		G4double TrackPY[max_tracks];
		G4double TrackPZ[max_tracks];
		G4double TrackVX[max_tracks];
		G4double TrackVY[max_tracks];
		G4double TrackVZ[max_tracks];
		G4double TrackEnergy[max_tracks];
		G4double TrackKEnergy[max_tracks];
		G4double TrackTime[max_tracks];


		// PostTrack data
		G4int nPostTrack;
		G4int PostTrackID[max_tracks];
		G4int PostTrackPDG[max_tracks];
		G4int PostTrackDetID[max_tracks];
		G4double PostTrackPX[max_tracks];
		G4double PostTrackPY[max_tracks];
		G4double PostTrackPZ[max_tracks];
		G4double PostTrackVX[max_tracks];
		G4double PostTrackVY[max_tracks];
		G4double PostTrackVZ[max_tracks];
		G4double PostTrackEnergy[max_tracks];
		G4double PostTrackKEnergy[max_tracks];
		G4double PostTrackTime[max_tracks];

		// Step data
		G4int nStep;
		G4int StepTrackID[max_steps];
		G4int StepTrackPDG[max_steps];
		G4int StepDetID[max_steps];
		G4double StepVX[max_steps];
		G4double StepVY[max_steps];
		G4double StepVZ[max_steps];
		G4double StepEdep[max_steps];
		G4double StepKE_Pre[max_steps];
		G4double Step_t[max_steps];
		G4int StepModelIndex[max_steps];
		G4int StepParentID[max_steps];
		//G4double EdepSumBox;

};
#endif
