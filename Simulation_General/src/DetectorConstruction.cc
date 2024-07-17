#include "DetectorConstruction.hh"
#include "ParameterContainer.hh"

#include "G4AutoDelete.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4Orb.hh"
#include "G4PVPlacement.hh"
#include "G4RunManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

DetectorConstruction::DetectorConstruction(ParameterContainer* par)
: G4VUserDetectorConstruction(),
	PC(par)
{
}

DetectorConstruction::~DetectorConstruction()
{ }

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
		// Get nist material manager
		G4NistManager* nist = G4NistManager::Instance();

		// Option to switch on/off checking of volumes overlaps
		G4bool checkOverlaps = true;

		G4ThreeVector fZero(0, 0, 0);
		G4double fSTPTemp = 273.15 * kelvin;
		G4double fLabTemp = fSTPTemp + 20 * kelvin;

		G4Element* elH = new G4Element("Hydrogen", "H", 1., 1.00794 * g/mole);
		G4Element* elC = new G4Element("Carbon", "C", 6., 12.011 * g/mole);
		G4Element* elO = new G4Element("Oxygen", "O", 8., 16.000 * g/mole);

		// World
		G4int worldID = PC -> GetParInt("WorldID");
		G4double world_sizeX = PC -> GetParDouble("World_sizeX");
		G4double world_sizeY = PC -> GetParDouble("World_sizeY");
		G4double world_sizeZ = PC -> GetParDouble("World_sizeZ");
		G4Material* world_mat;

		if(PC->GetParBool("World_vacuum"))
		world_mat = nist->FindOrBuildMaterial("G4_Galactic");

		if(PC->GetParBool("World_air"))
		world_mat = nist->FindOrBuildMaterial("G4_AIR");

		G4Box* solidWorld =    
			new G4Box("World",0.5*world_sizeX, 0.5*world_sizeY, 0.5*world_sizeZ);
		G4LogicalVolume* logicWorld =                         
			new G4LogicalVolume(solidWorld,world_mat,"World");
		G4VPhysicalVolume* physWorld = 
			new G4PVPlacement(0,G4ThreeVector(),logicWorld,"World",0,false,worldID,checkOverlaps);  

		G4Material* Acryl = new G4Material("Acrylic",1.19*g/cm3,3,kStateSolid, fLabTemp);
		Acryl -> AddElement(elC,5);
		Acryl -> AddElement(elH,8);
		Acryl -> AddElement(elO,2);

		G4Material *G4Pb = nist ->FindOrBuildMaterial("G4_Pb");

		// Box collimator
	if (PC->GetParBool("Box_In"))
	{

		G4int boxID = PC -> GetParInt("BoxID");

		G4double box_sizeX = PC -> GetParDouble("Box_sizeX");
		G4double box_sizeY = PC -> GetParDouble("Box_sizeY");
		G4double box_sizeZ = PC -> GetParDouble("Box_sizeZ");

		G4double box_Xpos = PC -> GetParDouble("Box_Xpos");
		G4double box_Ypos = PC -> GetParDouble("Box_Ypos");
		G4double box_Zpos = PC -> GetParDouble("Box_Zpos");

		G4Box* solidBox = new G4Box("Box",0.5*box_sizeX,0.5*box_sizeY,0.5*box_sizeZ);

		G4LogicalVolume* logicBox = new G4LogicalVolume(solidBox,Acryl,"Box");

		G4VisAttributes* attBox = new G4VisAttributes(G4Colour(G4Colour::Cyan()));
		attBox -> SetVisibility(true);
		attBox -> SetForceWireframe(true);
		logicBox -> SetVisAttributes(attBox);

		G4ThreeVector posCollX(0, 0, box_Zpos + box_sizeZ/2.);
		new G4PVPlacement(0, posCollX, logicBox, "Box", logicWorld, false, boxID, checkOverlaps);
    }

	if (PC->GetParBool("Box_Slited_In"))
	{

		G4int	 CollID	   = PC ->GetParInt("SBoxID");

		G4double CollDimX  = PC ->GetParDouble("SBox_sizeX");	// one brick [] 
		G4double CollDimY  = PC ->GetParDouble("SBox_sizeY");
		G4double CollDimZ  = PC ->GetParDouble("SBox_sizeZ");

		G4double CollslitX = PC ->GetParDouble("SBox_slitX");
		G4double CollslitY = PC ->GetParDouble("SBox_slitY");

		G4double CollPosX  = PC ->GetParDouble("SBox_Xpos");
		G4double CollPosY  = PC ->GetParDouble("SBox_Ypos");
		G4double CollPosZ  = PC ->GetParDouble("SBox_Zpos");

		//Volumes
		G4Box* solidBoxColl = new G4Box("solidBoxColl", CollDimX/2., CollDimY/2., CollDimZ/2.);
		G4Box* solidSubCollX = new G4Box("solidSubCollX", CollslitX/2., CollslitY/2. , CollDimZ/2.);
		G4SubtractionSolid* solidCollX = new G4SubtractionSolid("solidCollX", solidBoxColl, solidSubCollX, 0, fZero);

		G4LogicalVolume* logicCollX = new G4LogicalVolume(solidCollX, G4Pb, "logicCollimatorX");

		G4VisAttributes* attColl = new G4VisAttributes(G4Colour(G4Colour::Cyan()));
		attColl->SetVisibility(true);
		attColl->SetForceWireframe(true);
		logicCollX->SetVisAttributes(attColl);

		//Position
		G4ThreeVector posCollX(CollPosX, CollPosY, CollPosZ + CollDimZ/2.);
		new G4PVPlacement(0, posCollX, logicCollX, "CollimatorX", logicWorld, false, CollID, checkOverlaps);
	}


	return physWorld;
}
