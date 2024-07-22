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

		// Box collimator
	if (PC->GetParBool("BoxIn"))
	{
		G4Material* Collmat = new G4Material("Acrylic0",1.19*g/cm3,3,kStateSolid, fLabTemp);
		Collmat -> AddElement(elC,5);
		Collmat -> AddElement(elH,8);
		Collmat -> AddElement(elO,2);

		G4int boxID = PC -> GetParInt("BoxID");
		G4double box_sizeX = PC -> GetParDouble("Box_sizeX");
		G4double box_sizeY = PC -> GetParDouble("Box_sizeY");
		G4double box_sizeZ = PC -> GetParDouble("Box_sizeZ");
		G4double box_Zpos = PC -> GetParDouble("Box_Zpos");

		G4Box* solidBox = new G4Box("Box",0.5*box_sizeX,0.5*box_sizeY,0.5*box_sizeZ);
		G4LogicalVolume* logicBox = new G4LogicalVolume(solidBox,Collmat,"Box");

		G4VisAttributes* attBox = new G4VisAttributes(G4Colour(G4Colour::Cyan()));
		attBox -> SetVisibility(true);
		attBox -> SetForceWireframe(true);
		logicBox -> SetVisAttributes(attBox);

		G4ThreeVector posCollX(0, 0, box_Zpos + box_sizeZ/2.);
		new G4PVPlacement(0, posCollX, logicBox, "Box", logicWorld, false, boxID, checkOverlaps);
    }

		if (PC->GetParBool("Filter_In"))
	{ 
		for(int i =0; i < PC->GetParInt("Filter_Number"); i++){
			
		//	G4Material* Boxmat = nist->FindOrBuildMaterial("G4_AIR");
			G4Material* Boxmat = nist->FindOrBuildMaterial("G4_Galactic");
			G4int boxID[20];
			boxID[i] = (PC -> GetParInt("FilterID"))*(i+1);
			G4double box_sizeZ = PC -> GetParDouble("Filter_sizeZ");
			G4double box_Zpos = PC -> GetParDouble("Filter_Zpos");
			G4double boxs_dist = PC -> GetParDouble("Filters_Dist");

			G4Box* solidBox = new G4Box("Box",0.5*world_sizeX, 0.5*world_sizeY,0.5*box_sizeZ);
			G4LogicalVolume* logicBox[20];
			logicBox[i] = new G4LogicalVolume(solidBox,Boxmat,"Filtermat");

			G4VisAttributes* attBox = new G4VisAttributes(G4Colour(G4Colour::Cyan()));
			attBox -> SetVisibility(true);
			attBox -> SetForceWireframe(true);
			logicBox[i] -> SetVisAttributes(attBox);
			
			G4double zpos = i*abs(box_sizeZ+boxs_dist*mm) + box_Zpos + box_sizeZ/2.;
			G4ThreeVector posCollX(0, 0, zpos);
			new G4PVPlacement(0, posCollX, logicBox[i], "Filter", logicWorld, false, boxID[i], checkOverlaps);
		}
	}

	if (PC->GetParBool("Al_Shield"))
	{
		G4Material* Up_shield_mat =  nist->FindOrBuildMaterial("G4_Al");

		G4int boxID = PC -> GetParInt("Up_Shield_ID");
		G4double box_sizeX = PC -> GetParDouble("Up_Shield_sizeX");
		G4double box_sizeY = PC -> GetParDouble("Up_Shield_sizeY");
		G4double box_sizeZ = PC -> GetParDouble("Up_Shield_sizeZ");

		G4double box_posX  = PC ->GetParDouble("Up_Shield_posX");
		G4double box_posY  = PC ->GetParDouble("Up_Shield_posY");
		G4double box_posZ = PC -> GetParDouble("Up_Shield_posZ");

		G4double box_slit_posX = PC ->GetParDouble("Up_Shield_slit_posX");
		G4double box_slit_posY = PC ->GetParDouble("Up_Shield_slit_posY");
		G4double box_slitX = PC ->GetParDouble("Up_Shield_slitX");
		G4double box_slitY = PC ->GetParDouble("Up_Shield_slitY");

		G4Box* solidBoxColl = new G4Box("solidBoxColl", box_sizeX/2., box_sizeY/2., box_sizeZ/2.);
		G4Box* solidSubCollX = new G4Box("solidSubCollX", box_slitX/2., box_slitY/2. , box_sizeZ/2.);

		G4ThreeVector slit_pos(box_slit_posX, box_slit_posY, 0);
		G4SubtractionSolid* solidCollX = new G4SubtractionSolid("solidCollX", solidBoxColl, solidSubCollX, 0, slit_pos);
		G4LogicalVolume* logicCollX = new G4LogicalVolume(solidCollX, Up_shield_mat, "logicCollimatorX");

		G4VisAttributes* attBox = new G4VisAttributes(G4Colour(G4Colour::Blue()));
		attBox -> SetVisibility(true);
		attBox -> SetForceWireframe(true);
		logicCollX -> SetVisAttributes(attBox);

		G4ThreeVector posCollX(box_posX, box_posY, box_posZ + box_sizeZ/2.);
		new G4PVPlacement(0, posCollX, logicCollX, "Box", logicWorld, false, boxID, checkOverlaps);
    }

	

	//Target
	if (PC->GetParBool("Target_In"))
	{
		G4Material* TargetMat = nist->FindOrBuildMaterial("G4_C");

		G4int TargetID = PC -> GetParInt("TargetID");
		G4double Target_sizeX = PC -> GetParDouble("Target_sizeX");
		G4double Target_sizeY = PC -> GetParDouble("Target_sizeY");
		G4double Target_sizeZ = PC -> GetParDouble("Target_sizeZ");
		G4double Target_Zpos = PC -> GetParDouble("Target_Zpos");

		G4Box* solidTarget = new G4Box("Target",0.5*Target_sizeX,0.5*Target_sizeY,0.5*Target_sizeZ);
		G4LogicalVolume* logicTarget = new G4LogicalVolume(solidTarget,TargetMat,"Target");

		G4VisAttributes* attTarget = new G4VisAttributes(G4Colour(G4Colour::Yellow()));
		attTarget -> SetVisibility(true);
		attTarget -> SetForceWireframe(true);
		logicTarget -> SetVisAttributes(attTarget);

		G4ThreeVector posTarget(0, 0, Target_Zpos + Target_sizeZ/2.);
		new G4PVPlacement(0, posTarget, logicTarget, "Target", logicWorld, false, TargetID, checkOverlaps);
    }

		// Collimator
	if (PC->GetParBool("Collimator_In"))
	{
		G4Material* Collmat1 = new G4Material("Acrylic1",1.19*g/cm3,3,kStateSolid, fLabTemp);
		Collmat1 -> AddElement(elC,5);
		Collmat1 -> AddElement(elH,8);
		Collmat1 -> AddElement(elO,2);

		G4int	 CollID	   = PC ->GetParInt("Collimator_ID");

		G4double CollDimX  = PC ->GetParDouble("Collimator_sizeX");	// one brick [] 
		G4double CollDimY  = PC ->GetParDouble("Collimator_sizeY");
		G4double CollDimZ  = PC ->GetParDouble("Collimator_sizeZ");

		G4double CollslitX = PC ->GetParDouble("Collimator_slitX");
		G4double CollslitY = PC ->GetParDouble("Collimator_slitY");

		G4double CollPosX  = PC ->GetParDouble("Collimator_Xpos");
		G4double CollPosY  = PC ->GetParDouble("Collimator_Ypos");
		G4double CollPosZ  = PC ->GetParDouble("Collimator_Zpos");

		//Volumes
		G4Box* solidBoxColl = new G4Box("solidBoxColl", CollDimX/2., CollDimY/2., CollDimZ/2.);

		G4Box* solidSubCollX = new G4Box("solidSubCollX", CollslitX/2., CollslitY/2. , CollDimZ/2.);
		G4SubtractionSolid* solidCollX = new G4SubtractionSolid("solidCollX", solidBoxColl, solidSubCollX, 0, fZero);
		G4LogicalVolume* logicCollX = new G4LogicalVolume(solidCollX, Collmat1, "logicCollimatorX");

		//vis attributes
		G4VisAttributes* attColl = new G4VisAttributes(G4Colour(G4Colour::Cyan()));
		attColl->SetVisibility(true);
		attColl->SetForceWireframe(true);
		logicCollX->SetVisAttributes(attColl);

		//Position
		G4ThreeVector posCollX(CollPosX, CollPosY, CollPosZ + CollDimZ/2.);
		new G4PVPlacement(0, posCollX, logicCollX, "CollimatorX", logicWorld, false, CollID, checkOverlaps);
	}

	if (PC->GetParBool("Collimator2_In"))
	{
		G4Material* Collmat2 = new G4Material("Acrylic2",1.19*g/cm3,3,kStateSolid, fLabTemp);
		Collmat2 -> AddElement(elC,5);
		Collmat2 -> AddElement(elH,8);
		Collmat2 -> AddElement(elO,2);

		// G4Material *fBP = nist ->FindOrBuildMaterial("G4_Pb");

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
		G4LogicalVolume* logicCollX = new G4LogicalVolume(solidCollX, Collmat2, "logicCollimatorX");

		//vis attributes
		G4VisAttributes* attColl = new G4VisAttributes(G4Colour(G4Colour::Cyan()));
		attColl->SetVisibility(true);
		attColl->SetForceWireframe(true);
		logicCollX->SetVisAttributes(attColl);

		//Position
		G4ThreeVector posCollX(CollPosX, CollPosY, CollPosZ + CollDimZ/2.);
		new G4PVPlacement(0, posCollX, logicCollX, "CollimatorX", logicWorld, false, CollID, checkOverlaps);
	}


	if (PC->GetParBool("Absorber_In"))
	{
		G4Material* H_Poly = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
		G4Material* Boron = nist->FindOrBuildMaterial("G4_B");
		G4Material *fBP = new G4Material("BoratedPolyethylene",1.11*g/cm3,2);
		fBP -> AddMaterial(H_Poly,50*perCent);
		fBP -> AddMaterial(Boron,50*perCent);

		G4int	 CollID	   = PC ->GetParInt("SBoxID");

		G4double CollDimX  = PC ->GetParDouble("SBox_sizeX");	// one brick [] 
		G4double CollDimY  = PC ->GetParDouble("SBox_sizeY");
		G4double CollDimZ  = PC ->GetParDouble("SBox_sizeZ");


		G4double CollPosX  = PC ->GetParDouble("SBox_Xpos");
		G4double CollPosY  = PC ->GetParDouble("SBox_Ypos");
		G4double CollPosZ  = PC ->GetParDouble("SBox_Zpos");

		//Volumes
		G4Box* solidBoxColl = new G4Box("solidBoxColl", CollDimX/2., CollDimY/2., CollDimZ/2.);

		G4LogicalVolume* logicCollX = new G4LogicalVolume(solidBoxColl, fBP, "logicCollX");


		//vis attributes
		G4VisAttributes* attColl = new G4VisAttributes(G4Colour(G4Colour::Gray()));

		attColl->SetVisibility(true);
		attColl->SetForceWireframe(true);

		logicCollX->SetVisAttributes(attColl);

		//Position
		G4ThreeVector posCollX(CollPosX, CollPosY, CollPosZ + CollDimZ/2.);
		new G4PVPlacement(0, posCollX, logicCollX, "CollimatorX", logicWorld, false, CollID, checkOverlaps);
	}



	if (PC->GetParBool("SC1_In")) // box style
	{
		G4Material* Collmat = nist->FindOrBuildMaterial("G4_POLYSTYRENE");

		G4int	 CollID	   = PC ->GetParInt("SC1_ID");
		G4double CollDimX  = PC ->GetParDouble("SC1_sizeX");	// one brick [] 
		G4double CollDimY  = PC ->GetParDouble("SC1_sizeY");
		G4double CollDimZ  = PC ->GetParDouble("SC1_sizeZ");
		G4double CollPosX  = PC ->GetParDouble("SC1_Xpos");
		G4double CollPosY  = PC ->GetParDouble("SC1_Ypos");
		G4double CollPosZ  = PC ->GetParDouble("SC1_Zpos");

		G4Box* solidBox = new G4Box("SC1",0.5*CollDimX,0.5*CollDimY,0.5*CollDimZ); // 100 * 100 * 0.5
		G4LogicalVolume* logicBox = new G4LogicalVolume(solidBox,Collmat,"SC1");

		G4VisAttributes* attBox = new G4VisAttributes(G4Colour(G4Colour::Red()));
		attBox -> SetVisibility(true);
		attBox -> SetForceWireframe(true);
		logicBox -> SetVisAttributes(attBox);

		G4ThreeVector posCollX(CollPosX,CollPosY, CollPosZ + CollDimZ/2.);
		G4double theta = 0.*deg;  
		G4double phi = 0.*deg;    
		G4RotationMatrix* rotm  = new G4RotationMatrix(phi, -theta, 0.);
		new G4PVPlacement(rotm, posCollX, logicBox, "SC1", logicWorld, false, CollID, checkOverlaps);
	}

	if (PC->GetParBool("SC2_In"))
	{
		//G4Material* Collmat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
		G4Material* Collmat = nist->FindOrBuildMaterial("G4_POLYSTYRENE");
		
 		G4int	 CollID	   = PC ->GetParInt("SC2_ID");
		G4double CollDimX  = PC ->GetParDouble("SC2_sizeX");	// one brick [] 
		G4double CollDimY  = PC ->GetParDouble("SC2_sizeY");
		G4double CollDimZ  = PC ->GetParDouble("SC2_sizeZ");
		G4double CollPosX  = PC ->GetParDouble("SC2_Xpos");
		G4double CollPosY  = PC ->GetParDouble("SC2_Ypos");
		G4double CollPosZ  = PC ->GetParDouble("SC2_Zpos");

		G4Box* solidBox = new G4Box("SC2",0.5*CollDimX,0.5*CollDimY,0.5*CollDimZ); // 100 * 100 * 0.5
		G4LogicalVolume* logicBox = new G4LogicalVolume(solidBox,Collmat,"SC2");

		G4VisAttributes* attBox = new G4VisAttributes(G4Colour(G4Colour::Red()));
		attBox -> SetVisibility(true);
		attBox -> SetForceWireframe(true);
		logicBox -> SetVisAttributes(attBox);

		G4ThreeVector posCollX(CollPosX,CollPosY, CollPosZ + CollDimZ/2.);
		G4double theta = 0.*deg; 
		G4double phi = 0.*deg;   
		G4RotationMatrix* rotm  = new G4RotationMatrix(phi, -theta, 0.);
		new G4PVPlacement(rotm, posCollX, logicBox, "SC2", logicWorld, false, CollID, checkOverlaps);
	}

	



	return physWorld;
}
