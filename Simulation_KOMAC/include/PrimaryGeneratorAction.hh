#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include <fstream>
using namespace std;

class G4Event;

class ParameterContainer;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction(ParameterContainer* par);    
    virtual ~PrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);

		void GeneratePrimariesOpt0(G4Event*);	// for G4 particle generator
		void GeneratePrimariesOpt1(G4Event*); // for input data file
		void ReadInputFile();
  
    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
  
  private:
    G4ParticleGun*  fParticleGun; // pointer a to G4 gun class
		ParameterContainer* PC;
		G4String fInputName;
		ifstream fInputFile;

		// container for input data file
		vector<G4int> vec_eventID;
//		vector<G4int> vec_nTracks;
		vector<G4int> vec_PDG;
		vector<G4double> vec_vx, vec_vy, vec_vz, vec_px, vec_py, vec_pz, vec_time;
};

#endif
