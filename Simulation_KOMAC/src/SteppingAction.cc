#include "SteppingAction.hh"
#include "RunAction.hh"
#include "ParameterContainer.hh"

#include "G4Step.hh"


SteppingAction::SteppingAction(RunAction* runAction)
: G4UserSteppingAction(), fRunAction(runAction),

edepsum_1(0), edepsum_2(0),
//Threshold(ParameterContainer::GetInstance()->GetParDouble("Threshold")),
ID_SC1(ParameterContainer::GetInstance()->GetParInt("SC1_ID")),
ID_SC2(ParameterContainer::GetInstance()->GetParInt("SC2_ID")),
ID_Al(ParameterContainer::GetInstance()->GetParInt("Up_Shield_ID")),
ID_World(ParameterContainer::GetInstance()->GetParInt("WorldID")),
Bool_Step(ParameterContainer::GetInstance() -> GetParBool("MCStep")),
Bool_SC1(ParameterContainer::GetInstance() -> GetParBool("SC1_In")),
Bool_SC2(ParameterContainer::GetInstance() -> GetParBool("SC2_In")),
Bool_Al(ParameterContainer::GetInstance() -> GetParBool("Al_Shield"))
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{

	if(Bool_Step == true){

		G4StepStatus stat = step -> GetPostStepPoint() -> GetStepStatus();
		if(stat == fWorldBoundary){
			edepsum_1=0;
			edepsum_2=0;
			return;
		}


		G4int prevNo = step -> GetPreStepPoint()  -> GetPhysicalVolume() -> GetCopyNo(); //Ref.: pre. volumeID (step to material inside)
		G4int postNo = step -> GetPostStepPoint() -> GetPhysicalVolume() -> GetCopyNo();
		
		G4double edep = step -> GetTotalEnergyDeposit();
		
		G4int trackID = step -> GetTrack() -> GetTrackID();
		G4int trackPDG = step -> GetTrack() -> GetDefinition() -> GetPDGEncoding();
		G4ThreeVector pos = step -> GetPreStepPoint() -> GetPosition();
		G4double Step_dt = step -> GetPostStepPoint() -> GetGlobalTime();
		G4double KE_Pre = step -> GetPreStepPoint() -> GetKineticEnergy();
		G4int Index_CreatorModel = step -> GetTrack() -> GetCreatorModelIndex();
		G4int ID_Parent = step -> GetTrack() -> GetParentID(); 

		///// Step for the SC1 /////
		if(Bool_SC1 == true){

			if(prevNo == ID_SC1 &&
				postNo == ID_SC1){
		
					if(step -> GetTrack() -> GetTrackStatus() == fAlive ||
					step -> GetTrack() -> GetTrackStatus() == fStopButAlive){
						edepsum_1 +=edep;
					}

					else{
						edepsum_1 +=edep;
						fRunAction -> FillStep(trackID, trackPDG, prevNo, postNo, pos, edepsum_1,KE_Pre,Step_dt,Index_CreatorModel,ID_Parent);
						edepsum_1=0;
					}
				}

			if(prevNo == ID_SC1 &&
				postNo == ID_World){
					edepsum_1 +=edep;
					fRunAction -> FillStep(trackID, trackPDG, prevNo, postNo, pos, edepsum_1,KE_Pre,Step_dt,Index_CreatorModel,ID_Parent);
					edepsum_1=0;
			}
		}
		/////


		///// Step for the SC2 /////
		if(Bool_SC2 == true){
			if(prevNo == ID_SC2 &&
				postNo == ID_SC2){
					if(step -> GetTrack() -> GetTrackStatus() == fAlive ||
					step -> GetTrack() -> GetTrackStatus() == fStopButAlive){
						edepsum_2 +=edep;
					}
					else{
						edepsum_2 +=edep;
						fRunAction -> FillStep(trackID, trackPDG, prevNo, postNo, pos, edepsum_2,KE_Pre,Step_dt,Index_CreatorModel,ID_Parent);
						edepsum_2=0;
					}
				}

			if(prevNo == ID_SC2 &&
				postNo == ID_World){
					edepsum_2 +=edep;
					fRunAction -> FillStep(trackID, trackPDG, prevNo, postNo, pos, edepsum_2,KE_Pre,Step_dt,Index_CreatorModel,ID_Parent);
					edepsum_2=0;
			}
		}
		/////

		///// Step for the Al plate /////
		if(Bool_Al == true){
			if(prevNo == ID_Al &&
				postNo == ID_World){
					fRunAction -> FillStep(trackID, trackPDG, prevNo, postNo, pos, -1,KE_Pre,Step_dt,Index_CreatorModel,ID_Parent);
			}
		}
			
	}	


}
