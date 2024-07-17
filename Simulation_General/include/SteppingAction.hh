#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4StepStatus.hh"

class RunAction;

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(RunAction*);
    virtual ~SteppingAction();

    virtual void UserSteppingAction(const G4Step*);

  private:
		RunAction* fRunAction;
    G4double edepsum_1;
	  G4double edepsum_2;
    G4double Threshold;
    G4double ID_SC1;
    G4double ID_SC2;
    G4double ID_Al;
    G4double ID_World;
    G4bool Bool_Step;
    G4bool Bool_SC1;
    G4bool Bool_SC2;
    G4bool Bool_Al;
    
  

};

#endif
