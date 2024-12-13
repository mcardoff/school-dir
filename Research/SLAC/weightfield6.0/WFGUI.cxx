#include "WFGUI.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>


///-----------------------------------------------------\\ VARIABLES INITIAL VALUES //-------------------------------------------------------------///

std::map<std::string, double> GetParameters(const char* ifname)
{
	std::map<std::string, double> valueMap;

	// prepare map

	//#########################	
	//#--Detector Properties--#
	//#########################
	//--Dimensions--
	valueMap["STRIP_NUMB"] = 3.;
	valueMap["DETECT_HEIGHT"] = 200.;
	valueMap["STR_PITCH"] = 300.;
	valueMap["STR_WIDTH"] = 200.;
	valueMap["GAIN_SCL"] = 1.3;
	valueMap["SGAIN_SCL"] = 1;
	valueMap["DOP_LEV"] = 1.2;
	valueMap["HE_GAIN_RAT"] = 0.;
	valueMap["GAIN_LYR_RSS"] = 10.;
	valueMap["YPOSITION"] = 100.;
	
	valueMap["IRRADIATION"] = 10.;
	valueMap["BETA_ELECTRONS"] = 4.9;
	valueMap["BETA_HOLES"] = 6.2;
	valueMap["DOUBLEJUNCTION"] = 1.;
	valueMap["NA_OVER_ND"] = 0.5;

	valueMap["QUENCH"] = 1;
	valueMap["PRINT"] = 40;

	//--Voltage--
	valueMap["BIAS_VOLTAGE"] = 200.;
	valueMap["DEPL_VOLTAGE"] = 10.;
 
	//--Electronics--
	valueMap["CAPACITANCE"] = 2.;
	valueMap["INDUCTANCE"] = 2.;
	valueMap["IMPEDANCE"] = 50.;
	valueMap["OSCOPE_BW"] = 2.;
	valueMap["SHPR_INT_TIME"] = 3.5; 
	valueMap["SHPR_DCY_TIME"] = 5.;
	valueMap["SHPR_TRANS"] = 4.;
	valueMap["SHPR_NOISE"] = 1.;
	valueMap["VTH"] = 10.;
	valueMap["BBBW"] = 2.5;
	valueMap["BBGAIN"] = 100.;
	valueMap["BBVTH"] = 10.;
	valueMap["BB_NOISE"] = 2.;
	valueMap["BB_IMP"] = 50.;
	
	//#############
	//#--Control--#
	//#############
	valueMap["PRECISION"] = 5.;
	valueMap["SAMPLING"] = 1.;
	valueMap["STEPX"] = 1.;
	valueMap["STEPY"] = 0.1;

	//--Select Particles--
	valueMap["SET_RANGE"] = 3.;
	valueMap["CALIB"] = 5.;
	valueMap["USERQ"] = 70;
	valueMap["NUMBERP"] = 1;
    
	//--Currents--
	valueMap["TEMPERATURE"] = 300.;

	// -- Doping -- 
	valueMap["DOPING_GL"] = 0.;
	valueMap["KIND_GL"] = 0.;
	valueMap["SHAPE_GL"] = 0.;




	ifstream ifile(ifname);
	if (!ifile) {
		cout<< "\n***WARNING: Could not open parameter file " << ifname <<endl;
		cout<< "using DEFAULT values\n" <<endl;
		
		return valueMap;
	}
	cout<< "Loading file " << ifname <<endl;
	
	std::string line;

	while (getline(ifile,line)) {
        std::stringstream ss(line);
		std::string key = "";
		ss >> key;
		if (key.size() == 0) continue;// this line is empty
		if (key.c_str()[0] == '#') continue;// this is a comment
        std::map<std::string,double>::iterator found = valueMap.find(key);
		if (found != valueMap.end()) {
            ss >> found->second;
		}
	}

	return valueMap;
}

///---------------------------------------------------------------------------------------------------------------------------------------------///


///------------------------------------------------------------\\ GUI SETUP //--------------------------------------------------------------------///

WFGUI::WFGUI(const TGWindow *p, UInt_t w, UInt_t h, TApplication *app): TGMainFrame(p,w,h), dwpot(Potentials(300,-1,200,30)) {

    //take intial varible names from parameters file
  //  const char* ifname = "parameters.dat";
  string fname1  = "parameters";
  string fname1WF  = "WF";
  string ifname;
  string userFile;

  fCanvas = new TGCanvas(this, w, h, kFixedWidth | kFixedHeight);
  
  if(app->Argc() < 2)
    {
      fname1 = "parameters.dat";
    }
  else
    {
      fname1 = app->Argv(1);
    }
  string fname;

  std::string delimiter = ".";
  
  if(fname1.find(delimiter)>100 || fname1.find(delimiter)<1)  fname1 +=".dat";

  delimiter = "ta/";
   if (fname1.find(delimiter) != 10)  fname = "sensors/data/"+fname1;
   else fname = fname1;
  //"sensors/data/";

   ifname =  fname+fname1;
  //  ifname +=".dat";
  
  
  std::map<std::string, double> valueMap = GetParameters(fname.c_str());
  this->UserValues = valueMap;

  if(app->Argc() == 2)
    {
      //    LoadData();
    }
  
  // mainframes initial settings
  radiobuttonstatus=MIPunif;//radio button mip set as default
  BatchOn=false;
  fileName = "";
  Connect("CloseWindow()", "WFGUI", this, "CloseWindow()");// connect exit-button of window to CloseWindow()
  
  SetLayoutManager(new TGHorizontalLayout(this));
  
  
  ///**************************************************** GRAPHS ********************************************************///
    
  PotentialTab = new TGTab(this,600,220);//General Tab

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //////DRIFT POTENTIAL TAB//////////////////////////////////////////////////////////////////////
   
    DriftPTab = PotentialTab->AddTab("Drift Potential");
    DriftPTab->SetLayoutManager(new TGVerticalLayout(DriftPTab));
    
    /////////CALCULATING LABEL/////////////
    //  CalculatingLabel = new TGLabel(DriftPTab, new TGString("First press 'Set' then 'Potential' and finally 'Currents' "));
    // DriftPTab->AddFrame(CalculatingLabel, new TGLayoutHints(kLHintsExpandX | kLHintsTop,0,0,1,1));
    ////////////////////////////////////////////

    
    ////////////////////////////////////////////
    /////////DRIFT CANVAS/////////////
    driftcanvas = new TRootEmbeddedCanvas("DriftCanvas",DriftPTab,600,140); //canvas for drift potential
    DriftPTab->AddFrame(driftcanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,1,1,1,1));
    ////////////////////////////////////////////
    
    DriftPTabFrame = new TGHorizontalFrame(DriftPTab,600,400);
    
    ////////////////////////////////////////////
    /////////DRIFT CANVAS/////////////
    driftpcanvas = new TRootEmbeddedCanvas("DriftPCanvas",DriftPTabFrame,600,380); // canvas for cut potential
    DriftPTabFrame->AddFrame(driftpcanvas,new TGLayoutHints(kLHintsExpandY | kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////
    
    DriftPTab->AddFrame(DriftPTabFrame,new TGLayoutHints( kLHintsBottom | kLHintsExpandX,1,1,1,1));
    
    ////////////////////////////////////////////
    /////////LABELS TAB FRAME/////////////
    //create frames
    LabelsTabFrame = new TGHorizontalFrame(DriftPTab, 600,20);
    
    //create entries
    DriftCutLabel = new TGLabel(LabelsTabFrame, new TGString("Plotting at:"));
    OnStripsButton = new TGTextButton(LabelsTabFrame,new TGHotString("On Strips"));
    BetweenStripsButton = new TGTextButton(LabelsTabFrame,new TGHotString("Between Strips"));
    WhereCut = new TGNumberEntry(LabelsTabFrame, (Double_t) 0,8,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELLimitMinMax);
    DrawCutsUserEntry = new TGTextButton(LabelsTabFrame, new TGHotString("Draw"));
    FieldLabel = new TGLabel(LabelsTabFrame, new TGString("Field:"));
    ExButton = new TGTextButton(LabelsTabFrame,new TGHotString("Ex"));
    EyButton = new TGTextButton(LabelsTabFrame,new TGHotString("Ey"));

    //develop entries
    OnStripsButton->SetTextColor(0xff0000);
    OnStripsButton->Connect("Clicked()", "WFGUI", this, "SetCutOnStrips()");
    BetweenStripsButton->Connect("Clicked()", "WFGUI", this, "SetCutBetweenStrips()");
    BetweenStripsButton->SetTextColor(0x0000ff);
    DrawCutsUserEntry->Connect("Clicked()", "WFGUI", this, "DrawAll()");
    ExButton->Connect("Clicked()", "WFGUI", this, "DrawFieldsAbsX()");
    EyButton->Connect("Clicked()", "WFGUI", this, "DrawFieldsAbsY()");
    OnStripsButton->SetEnabled(kFALSE);
    BetweenStripsButton->SetEnabled(kFALSE);
    ExButton->SetEnabled(kFALSE);
    EyButton->SetEnabled(kFALSE);
    DrawCutsUserEntry->SetEnabled(kFALSE);
    
    //add entries
    LabelsTabFrame->AddFrame(DriftCutLabel, new TGLayoutHints(kLHintsLeft,1,1,5,1));
    LabelsTabFrame->AddFrame(OnStripsButton, new TGLayoutHints(kLHintsLeft,5,1,1,1));
    LabelsTabFrame->AddFrame(BetweenStripsButton, new TGLayoutHints(kLHintsLeft,5,1,1,1));
    LabelsTabFrame->AddFrame(WhereCut, new TGLayoutHints(kLHintsLeft,5,1,1,1));
    LabelsTabFrame->AddFrame(DrawCutsUserEntry, new TGLayoutHints(kLHintsLeft,1,1,1,1));
    LabelsTabFrame->AddFrame(ExButton, new TGLayoutHints(kLHintsRight,5,1,1,1));
    LabelsTabFrame->AddFrame(EyButton, new TGLayoutHints(kLHintsRight,5,1,1,1));
    LabelsTabFrame->AddFrame(FieldLabel, new TGLayoutHints(kLHintsRight,30,1,5,1));

    //add frames
    DriftPTab->AddFrame(LabelsTabFrame, new TGLayoutHints(kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////
    
    //////END OF DRIFT TAB/////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
  
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //////WEIGHTING POTENTIAL TAB//////////////////////////////////////////////////////////////////
   
    WeightingPTab = PotentialTab->AddTab("Weighting Potential");
    WeightingPTab->SetLayoutManager(new TGVerticalLayout(WeightingPTab));

    ////////////////////////////////////////////
    /////////CALCULATING LABEL/////////////
    CalculatingLabel2 = new TGLabel(WeightingPTab, new TGString(" Press Button 'Calculate' to calculate Potentials and Fields"));
    // WeightingPTab->AddFrame(CalculatingLabel2, new TGLayoutHints(kLHintsExpandX | kLHintsTop,0,0,1,1));
    ////////////////////////////////////////////

    ////////////////////////////////////////////
    /////////WEIGHTING CANVAS/////////////
    weightcanvas = new TRootEmbeddedCanvas("weightCanvas",WeightingPTab,600,220); // canvas for cut potential
    WeightingPTab->AddFrame(weightcanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,1,1,1,1));
    ////////////////////////////////////////////

    WeightPTabFrame = new TGHorizontalFrame(WeightingPTab,600,400);
    
    ////////////////////////////////////////////
    /////////LABELS TAB FRAME/////////////
    //create frames
    LabelsTabFrame2 = new TGHorizontalFrame(WeightingPTab, 600,20);
    
    //create entries
    WeightingCutLabel = new TGLabel(LabelsTabFrame2, new TGString("Plotting at:"));
    OnStripsButton2 = new TGTextButton(LabelsTabFrame2,new TGHotString("On Strips"));
    BetweenStripsButton2 = new TGTextButton(LabelsTabFrame2,new TGHotString("Between Strips"));
    WhereCut2 = new TGNumberEntry(LabelsTabFrame2, (Double_t) 0,8,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELLimitMinMax);
    DrawCutsUserEntry2 = new TGTextButton(LabelsTabFrame2, new TGHotString("Draw"));
    FieldLabel2 = new TGLabel(LabelsTabFrame2, new TGString("Field:"));
    ExButton2 = new TGTextButton(LabelsTabFrame2,new TGHotString("|Ex|"));
    EyButton2 = new TGTextButton(LabelsTabFrame2,new TGHotString("|Ey|"));
    
    //develop entries
    OnStripsButton2->SetTextColor(0xff0000);
    OnStripsButton2->Connect("Clicked()", "WFGUI", this, "SetCutOnStrips()");
    BetweenStripsButton2->SetTextColor(0x0000ff);
    BetweenStripsButton2->Connect("Clicked()", "WFGUI", this, "SetCutBetweenStrips()");
    DrawCutsUserEntry2->Connect("Clicked()", "WFGUI", this, "DrawAllw()");
    ExButton2->Connect("Clicked()", "WFGUI", this, "DrawFieldsAbsX()");
    EyButton2->Connect("Clicked()", "WFGUI", this, "DrawFieldsAbsY()");
    OnStripsButton2->SetEnabled(kFALSE);
    BetweenStripsButton2->SetEnabled(kFALSE);
    ExButton2->SetEnabled(kFALSE);
    EyButton2->SetEnabled(kFALSE);
    DrawCutsUserEntry2->SetEnabled(kFALSE);
    
    //add entries
    LabelsTabFrame2->AddFrame(WeightingCutLabel, new TGLayoutHints(kLHintsLeft,1,1,5,1));
    LabelsTabFrame2->AddFrame(OnStripsButton2, new TGLayoutHints(kLHintsLeft,5,1,1,1));
    LabelsTabFrame2->AddFrame(BetweenStripsButton2, new TGLayoutHints(kLHintsLeft,5,1,1,1));
    LabelsTabFrame2->AddFrame(WhereCut2, new TGLayoutHints(kLHintsLeft,5,1,1,1));
    LabelsTabFrame2->AddFrame(DrawCutsUserEntry2, new TGLayoutHints(kLHintsLeft,1,1,1,1));
    LabelsTabFrame2->AddFrame(ExButton2, new TGLayoutHints(kLHintsRight,5,1,1,1));
    LabelsTabFrame2->AddFrame(EyButton2, new TGLayoutHints(kLHintsRight,5,1,1,1));
    LabelsTabFrame2->AddFrame(FieldLabel2, new TGLayoutHints(kLHintsRight,30,1,5,1));
    
    //add frames
    WeightingPTab->AddFrame(LabelsTabFrame2, new TGLayoutHints(kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////

    ////////////////////////////////////////////
    /////////WEIGHTING CANVAS/////////////
    weightpcanvas = new TRootEmbeddedCanvas("weightPCanvas",WeightPTabFrame,600,300); // canvas for cut potential
    WeightPTabFrame->AddFrame(weightpcanvas,new TGLayoutHints(kLHintsExpandY | kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////
    
    WeightingPTab->AddFrame(WeightPTabFrame, new TGLayoutHints(kLHintsExpandX,1,1,1,1));
    
    //////END OF WEIGHTING POTENTIAL TAB///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
	
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //////CURRENTS TAB/////////////////////////////////////////////////////////////////////////////
    
    CurrentsTab = PotentialTab->AddTab("Currents and Oscilloscope");
    CurrentsTab->SetLayoutManager(new TGVerticalLayout(CurrentsTab));
    
    ////////////////////////////////////////////
    /////////CURRENTS CANVAS/////////////
	currentscanvas = new TRootEmbeddedCanvas("CurrentsCanvas",CurrentsTab,600,520);	// canvas for graphs of currents
	CurrentsTab->AddFrame(currentscanvas,new TGLayoutHints(kLHintsExpandY | kLHintsExpandX,1,1,1,1));	
    ///////////////////////////////////////////
    
    CurrentsInfoFrame = new TGVerticalFrame(CurrentsTab,100,200);

    ////////////////////////////////////////////
    /////////CURRENTS LABEL FRAME/////////////
    //create frames
    CurrentsLabelFrame = new TGHorizontalFrame(CurrentsInfoFrame,100,50);
    
    //create entries
    ElectronsLabel =  new TGLabel(CurrentsLabelFrame, new TGString("Electrons"));
    GainElectronsLabel =  new TGLabel(CurrentsLabelFrame, new TGString("Gain El."));
    HolesLabel =  new TGLabel(CurrentsLabelFrame, new TGString("Holes"));
    GainHolesLabel =  new TGLabel(CurrentsLabelFrame, new TGString("Gain Holes"));
    TotalLabel =  new TGLabel(CurrentsLabelFrame, new TGString("Total"));
    
    //develop entries
    ElectronsLabel->SetBackgroundColor(0xff0000);
    GainElectronsLabel->SetBackgroundColor(0xff00ff);
    HolesLabel->SetBackgroundColor(0x0066ff);
    GainHolesLabel->SetBackgroundColor(0x00ffff);
    TotalLabel->SetBackgroundColor(0x00ff00);
    
    //add entries
    CurrentsLabelFrame->AddFrame(ElectronsLabel,new TGLayoutHints(kLHintsTop | kLHintsExpandX,1,1,1,1));
    CurrentsLabelFrame->AddFrame(GainElectronsLabel,new TGLayoutHints(kLHintsTop | kLHintsExpandX,1,1,1,1));
    CurrentsLabelFrame->AddFrame(HolesLabel,new TGLayoutHints(kLHintsTop | kLHintsExpandX,1,1,1,1));
    CurrentsLabelFrame->AddFrame(GainHolesLabel,new TGLayoutHints(kLHintsTop | kLHintsExpandX,1,1,1,1));
    CurrentsLabelFrame->AddFrame(TotalLabel,new TGLayoutHints(kLHintsTop | kLHintsExpandX,1,1,1,1));
    
    //add frames
    CurrentsInfoFrame->AddFrame(CurrentsLabelFrame, new TGLayoutHints(kLHintsExpandX | kLHintsTop,1,1,1,1));
    ////////////////////////////////////////////
    
    
    ////////////////////////////////////////////
    /////////ELECTRONICS LABEL FRAME/////////////
    //create frames
    ElectronicsLabelFrame = new TGHorizontalFrame(CurrentsInfoFrame,100,50);
    
    //create entries
    RCLabel =  new TGLabel(ElectronicsLabelFrame, new TGString("- - -  Current at CSA input [A]"));
    ScopeLabel =  new TGLabel(ElectronicsLabelFrame, new TGString("- Oscilloscope [mV]"));
    
    //develop entries
    RCLabel->SetBackgroundColor(0xffffff);
    ScopeLabel->SetBackgroundColor(0xffffff);
    
    //add entries
    //    ElectronicsLabelFrame->AddFrame(RCLabel,new TGLayoutHints(kLHintsTop | kLHintsExpandX,1,1,1,1));
    ElectronicsLabelFrame->AddFrame(ScopeLabel,new TGLayoutHints(kLHintsTop | kLHintsExpandX,1,1,1,1));
    
    //add frames
    CurrentsInfoFrame->AddFrame(ElectronicsLabelFrame, new TGLayoutHints(kLHintsExpandX | kLHintsTop,1,1,1,1));
    ////////////////////////////////////////////
    
    
    ////////////////////////////////////////////
    /////////CURRENT PROGRESS BAR FRAME/////////////
    // CurrentsProgressBar = new TGHProgressBar(CurrentsInfoFrame,TGProgressBar::kStandard,500);
    //    CurrentsProgressBar->ShowPosition(kTRUE,kTRUE);
    //CurrentsProgressBar->Percent(0);
    //CurrentsInfoFrame->AddFrame(CurrentsProgressBar,new TGLayoutHints(kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////
 
    
    ////////////////////////////////////////////
    /////////CHARGE COLLECTION FRAME/////////////
    //create frames
    ChargeCollectionFrame = new TGGroupFrame(CurrentsInfoFrame, "Charge Collection [k ]", kHorizontalFrame);
    ChargeLabelFrame1 = new TGVerticalFrame(ChargeCollectionFrame);
    ChargeLabelFrame2 = new TGVerticalFrame(ChargeCollectionFrame);
    ChargeLabelFrame3 = new TGVerticalFrame(ChargeCollectionFrame);
    ChargeNumberFrame1 = new TGVerticalFrame(ChargeCollectionFrame, 150, 200);
    ChargeNumberFrame2 = new TGVerticalFrame(ChargeCollectionFrame, 150, 200);
    ChargeNumberFrame3 = new TGVerticalFrame(ChargeCollectionFrame, 150, 200);

	//create entries
    QETextLabel =  new TGLabel(ChargeLabelFrame1, new TGString("e- charges (e): "));//electrons charge label
    QELabel = new TGLabel(ChargeNumberFrame1, new TGString("0"));
    QHTextLabel =  new TGLabel(ChargeLabelFrame2, new TGString(" h+ charges (e): "));//holes charge label
    QHLabel = new TGLabel(ChargeNumberFrame2, new TGString("0"));
    QEHTextLabel =  new TGLabel(ChargeLabelFrame3, new TGString(" e- + h+ charges (e): "));//gain electrons charge label
    QEHLabel = new TGLabel(ChargeNumberFrame3, new TGString("0"));
    QEGTextLabel =  new TGLabel(ChargeLabelFrame1, new TGString("Gain e- charges (e): "));//gain holes charge label
    QEGLabel = new TGLabel(ChargeNumberFrame1, new TGString("0"));
    QHGTextLabel =  new TGLabel(ChargeLabelFrame2, new TGString(" Gain h+ charges (e): "));//total charge label
    QHGLabel = new TGLabel(ChargeNumberFrame2, new TGString("0"));
    QEHGTextLabel =  new TGLabel(ChargeLabelFrame3, new TGString(" Gain e- + h+ charges (e): "));//gain electrons charge label
    QEHGLabel = new TGLabel(ChargeNumberFrame3, new TGString("0"));
    QETotTextLabel =  new TGLabel(ChargeLabelFrame1, new TGString("Total e- charges (e): "));//gain holes charge label
    QETotLabel = new TGLabel(ChargeNumberFrame1, new TGString("0"));
    QHTotTextLabel =  new TGLabel(ChargeLabelFrame2, new TGString(" Total h+ charges (e): "));//total charge label
    QHTotLabel = new TGLabel(ChargeNumberFrame2, new TGString("0"));
    QTotTextLabel =  new TGLabel(ChargeLabelFrame3, new TGString(" Total Charges (e): "));
    QTotLabel = new TGLabel(ChargeNumberFrame3, new TGString("0"));
    
	//develop entries
    QELabel->SetBackgroundColor(0xffffff);
    QHLabel->SetBackgroundColor(0xffffff);
    QEHLabel->SetBackgroundColor(0xffffff);
    QEGLabel->SetBackgroundColor(0xffffff);
    QHGLabel->SetBackgroundColor(0xffffff);
    QEHGLabel->SetBackgroundColor(0xffffff);
    QETotLabel->SetBackgroundColor(0xffffff);
    QHTotLabel->SetBackgroundColor(0xffffff);
    QTotLabel->SetBackgroundColor(0xffffff);
    QELabel->Resize(QELabel->GetWidth()+220, QELabel->GetHeight());
    
    /*QHLabel->SetBackgroundColor(0xffffff);
    QEHLabel->SetBackgroundColor(0xffffff);
    QEGLabel->SetBackgroundColor(0xffffff);
    QHGLabel->SetBackgroundColor(0xffffff);
    QEHGLabel->SetBackgroundColor(0xffffff);
    QETotLabel->SetBackgroundColor(0xffffff);
    QHTotLabel->SetBackgroundColor(0xffffff);
    QTotLabel->SetBackgroundColor(0xffffff);*/

    //add entries
    ChargeLabelFrame1->AddFrame(QETextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop,1,1,1,1));
    ChargeNumberFrame1->AddFrame(QELabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,1,1,1,1));
    ChargeLabelFrame2->AddFrame(QHTextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop,1,1,1,1));
    ChargeNumberFrame2->AddFrame(QHLabel, new TGLayoutHints(kLHintsLeft  | kLHintsTop | kLHintsExpandX,1,1,1,1));
    ChargeLabelFrame3->AddFrame(QEHTextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop,1,1,1,1));
    ChargeNumberFrame3->AddFrame(QEHLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,1,1,1,1));
    ChargeLabelFrame1->AddFrame(QEGTextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop,1,1,1,1));
    ChargeNumberFrame1->AddFrame(QEGLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,1,1,1,1));
    ChargeLabelFrame2->AddFrame(QHGTextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop,1,1,1,1));
    ChargeNumberFrame2->AddFrame(QHGLabel, new TGLayoutHints(kLHintsLeft  | kLHintsTop| kLHintsExpandX,1,1,1,1));
    ChargeLabelFrame3->AddFrame(QEHGTextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop,1,1,1,1));
    ChargeNumberFrame3->AddFrame(QEHGLabel, new TGLayoutHints(kLHintsLeft  | kLHintsTop| kLHintsExpandX,1,1,1,1));
    ChargeLabelFrame1->AddFrame(QETotTextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop| kLHintsExpandX,1,1,1,1));
    ChargeNumberFrame1->AddFrame(QETotLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop| kLHintsExpandX,1,1,1,1));
    ChargeLabelFrame2->AddFrame(QHTotTextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop,1,1,1,1));
    ChargeNumberFrame2->AddFrame(QHTotLabel, new TGLayoutHints(kLHintsLeft  | kLHintsTop| kLHintsExpandX,1,1,1,1));
    ChargeLabelFrame3->AddFrame(QTotTextLabel, new TGLayoutHints(kLHintsLeft| kLHintsTop,1,1,1,1));
    ChargeNumberFrame3->AddFrame(QTotLabel, new TGLayoutHints(kLHintsLeft  | kLHintsTop| kLHintsExpandX,1,1,1,1));
    
    //add frames
    ChargeCollectionFrame->AddFrame(ChargeLabelFrame1, new TGLayoutHints(kLHintsLeft,0,0,0,0));
    ChargeCollectionFrame->AddFrame(ChargeNumberFrame1, new TGLayoutHints(kLHintsLeft | kLHintsExpandX ,0,0,0,0));
    ChargeCollectionFrame->AddFrame(ChargeLabelFrame2, new TGLayoutHints(kLHintsLeft,0,0,0,0));
    ChargeCollectionFrame->AddFrame(ChargeNumberFrame2, new TGLayoutHints(kLHintsLeft | kLHintsExpandX,0,0,0,0));
    ChargeCollectionFrame->AddFrame(ChargeLabelFrame3, new TGLayoutHints(kLHintsLeft,0,0,0,0));
    ChargeCollectionFrame->AddFrame(ChargeNumberFrame3, new TGLayoutHints(kLHintsLeft | kLHintsExpandX,0,0,0,0));
    CurrentsInfoFrame->AddFrame(ChargeCollectionFrame, new TGLayoutHints(kLHintsExpandX | kLHintsTop,1,1,1,1));
    ////////////////////////////////////////////
    
    
    ////////////////////////////////////////////
    /////////LORENTZ FRAME/////////////
    //create frames
    LorentzInfoFrame = new TGGroupFrame(CurrentsInfoFrame, "Lorentz Drift", kHorizontalFrame);
    
    //create entries
    LorentzeTextLabel =  new TGLabel(LorentzInfoFrame, new TGString("e- Lorentz Angle [D]: "));
    LorentzeLabel = new TGLabel(LorentzInfoFrame, new TGString("0"));
    LorentzhTextLabel =  new TGLabel(LorentzInfoFrame, new TGString("h+ Lorentz Angle [D]: "));
    LorentzhLabel = new TGLabel(LorentzInfoFrame, new TGString("0"));

	//develop entries
    LorentzeLabel->SetBackgroundColor(0xffffff);
    LorentzhLabel->SetBackgroundColor(0xffffff);
    
    //add entries
    LorentzInfoFrame->AddFrame(LorentzeTextLabel, new TGLayoutHints(kLHintsLeft,1,1,1,1));
    LorentzInfoFrame->AddFrame(LorentzeLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop |kLHintsExpandX,1,1,1,1));
    LorentzInfoFrame->AddFrame(LorentzhTextLabel, new TGLayoutHints(kLHintsLeft,1,1,1,1));
    LorentzInfoFrame->AddFrame(LorentzhLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,1,1,1,1));
    
    //add frames
    CurrentsInfoFrame->AddFrame(LorentzInfoFrame,new TGLayoutHints(kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////
    
    //    CurrentsTab->AddFrame(CurrentsInfoFrame,new TGLayoutHints(kLHintsExpandY | kLHintsExpandX,1,1,1,1));


    /////////Lifetime FRAME/////////////
    //create frames
    LTInfoFrame = new TGGroupFrame(CurrentsInfoFrame, "Carriers lifetime", kHorizontalFrame);
    
    //create entries
    LTeTextLabel =  new TGLabel(LTInfoFrame, new TGString("e- lifetime [ns]: "));
    LTeLabel = new TGLabel(LTInfoFrame, new TGString("0"));
    LThTextLabel =  new TGLabel(LTInfoFrame, new TGString("h+ lifetime [ns]: "));
    LThLabel = new TGLabel(LTInfoFrame, new TGString("0"));
    
    //develop entries
    LTeLabel->SetBackgroundColor(0xffffff);
    LThLabel->SetBackgroundColor(0xffffff);
    
    //add entries
    LTInfoFrame->AddFrame(LTeTextLabel, new TGLayoutHints(kLHintsLeft,1,1,1,1));
    LTInfoFrame->AddFrame(LTeLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop |kLHintsExpandX,1,1,1,1));
    LTInfoFrame->AddFrame(LThTextLabel, new TGLayoutHints(kLHintsLeft,1,1,1,1));
    LTInfoFrame->AddFrame(LThLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,1,1,1,1));
    
    //add frames
    CurrentsInfoFrame->AddFrame(LTInfoFrame,new TGLayoutHints(kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////
    
    CurrentsTab->AddFrame(CurrentsInfoFrame,new TGLayoutHints(kLHintsExpandY | kLHintsExpandX,1,1,1,1));

    
    //////END OF CURRENTS TAB//////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////

    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////

    //////OSCILLOSCOPE TAB/////////////////////////////////////////////////////////////////////////
    
    OscilloscopeTab = PotentialTab->AddTab("Electronics I");
    OscilloscopeTab->SetLayoutManager(new TGVerticalLayout(OscilloscopeTab));
    PotentialTab->SetTab(0);

    ////////////////////////////////////////////
    /////////OSCILLOSCOPE CANVAS/////////////
    oscilloscopecanvas = new TRootEmbeddedCanvas("OscilloscopeCanvas",OscilloscopeTab,600,520);
    OscilloscopeTab->AddFrame(oscilloscopecanvas,new TGLayoutHints(kLHintsExpandY | kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////
    
    //////END OF OSCILLOSCOPE TAB///////////////////////////////////////////////////////////////////

    //////Electronics II TAB/////////////////////////////////////////////////////////////////////////
    
    ElectronicsIITab = PotentialTab->AddTab("Electronics II");
    ElectronicsIITab->SetLayoutManager(new TGVerticalLayout(ElectronicsIITab));
    PotentialTab->SetTab(0);

    ////////////////////////////////////////////
    /////////OSCILLOSCOPE CANVAS/////////////
    electronicsIIcanvas = new TRootEmbeddedCanvas("ElectronicsIICanvas",ElectronicsIITab,600,520);
    ElectronicsIITab->AddFrame(electronicsIIcanvas,new TGLayoutHints(kLHintsExpandY | kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////
    
    //////End ElectronicsII TAB///////////////////////////////////////////////////////////////////
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    AddFrame(PotentialTab,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2)); // Add tabs to mainframe

    ///************************************************* END OF GRAPHS *****************************************************///

    
    
    
	//da leggere
	//http://root.cern.ch/root/html/TGCompositeFrame.html
	//http://root.cern.ch/root/html/TGLayoutManager.html
	//http://www.eclipsezone.com/eclipse/forums/t99010.html
	//http://www.eclipse.org/eclipse/platform-core/documents/3.1/debug.html
    
    
    
    
    ///*************************************************** SETTINGS *******************************************************///
    
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///START OF SETTINGS FRAME////////////////////////////////////////////////////////////////////////////////////////////////
    
    SettingsGlobalFrame = new TGVerticalFrame(this,400,90);
    SettingsFrame2 = new TGHorizontalFrame(SettingsGlobalFrame, 400, 90);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //////START OF CONTROL FRAME///////////////////////////////////////////////////////////////////
   
    //    ControlFrame = new TGGroupFrame(SettingsFrame2,"Control",kVerticalFrame);
    ControlFrame = new TGVerticalFrame(SettingsFrame2);

    ////////////////////////////////////////////
    /////////RUN FRAME/////////////
    //create frames
    ButtonPotFrame = new TGGroupFrame(ControlFrame,"Run",kHorizontalFrame);
    //
    // place here "run", "done"..

    CalculatingLabel = new TGLabel(ControlFrame, new TGString("Press 'Potential' and then 'Currents' "));
    CalculatingLabel->SetBackgroundColor(0x00ff00);
    ControlFrame->AddFrame(CalculatingLabel, new TGLayoutHints(kLHintsExpandX | kLHintsTop,0,0,1,1));
    //create entries
    SetButton = new TGTextButton(ButtonPotFrame,"Set");
    CalcPotButton = new TGTextButton(ButtonPotFrame, "Potentials");
    CalculateButton = new TGTextButton(ButtonPotFrame,"Currents");
    StopButton = new TGTextButton(ButtonPotFrame," Stop ");
    ExitButton = new TGTextButton(ButtonPotFrame," Exit ");


    //develop entries
    SetButton->Associate(this);
    SetButton->SetTextColor(1,kFALSE);
    SetButton->Connect("Clicked()","WFGUI",this,"CallBoundaryConditions())");//Connect Set Button to CallBoundary Conditions()
    SetButton->SetEnabled(kTRUE);
    CalcPotButton->SetTextColor(1,kFALSE);
    CalcPotButton->Connect("Clicked()","WFGUI",this,"ThreadstartPotential()");//Connect button with threadstart method
    CalcPotButton->SetEnabled(kTRUE);
    CalculateButton->SetTextColor(1, kFALSE);
    CalculateButton->Connect("Clicked()","WFGUI",this,"ThreadstartCurrents()"); // connect calculate button to threadstart
    CalculateButton->SetEnabled(kFALSE);
    StopButton->Associate(this);
    StopButton->SetBackgroundColor(0xff0000);
    StopButton->Connect("Clicked()","WFGUI",this,"SetStopOn()");
    StopButton->SetEnabled(kTRUE);
    ExitButton->Associate(this);
    ExitButton->SetBackgroundColor(0x66cc66);
    //    ExitButton->SetCommand("gApplication->Terminate(0)");
    ExitButton->Connect("Clicked()","WFGUI",this,"CloseWindow()");
    //ExitButton->SetCommand("CloseWindow()");
    ExitButton->SetEnabled(kTRUE);

    //add entries
    ButtonPotFrame->AddFrame(SetButton, new TGLayoutHints(kLHintsCenterX,1,1,1,1));
	ButtonPotFrame->AddFrame(CalcPotButton, new TGLayoutHints(kLHintsCenterX,1,1,1,1));
	ButtonPotFrame->AddFrame(CalculateButton,  new TGLayoutHints(kLHintsCenterX,1,1,1,1));
	ButtonPotFrame->AddFrame(StopButton,  new TGLayoutHints(kLHintsCenterX,1,1,1,1));
	ButtonPotFrame->AddFrame(ExitButton,  new TGLayoutHints(kLHintsCenterX,1,1,1,1));
    
    //add frames
	ControlFrame->AddFrame(ButtonPotFrame,new TGLayoutHints(kLHintsTop | kLHintsCenterY | kLHintsExpandX ,1,1,1,1));
    ////////////////////////////////////////////

    
    ////////////////////////////////////////////
    /////////PRECISION FRAME/////////////
    //create frames
    PrecisionGroupFrame = new TGGroupFrame(ControlFrame,"Precision",kVerticalFrame);
    PrecisionFrame = new TGHorizontalFrame(PrecisionGroupFrame);
    SamplingFrame = new TGHorizontalFrame(PrecisionGroupFrame);

    //create entries
    PrecisionLabel = new TGLabel(PrecisionFrame,"eh pairs followed (1= Most precise, 100 = Fastest):");
    PrecisionEntry = new TGNumberEntry(PrecisionFrame, valueMap["PRECISION"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,1,100);
    SamplingLabel = new TGLabel(SamplingFrame,"Time Step [ps]:");
    StepLabel = new TGLabel(SamplingFrame,"Step x,y [micron]: ");
    SamplingEntry = new TGNumberEntry(SamplingFrame, valueMap["SAMPLING"],4,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0.1,100);
    StepxEntry = new TGNumberEntry(SamplingFrame, valueMap["STEPX"],4,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0.01,2);
    StepyEntry = new TGNumberEntry(SamplingFrame, valueMap["STEPY"],4,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0.01,2);
    
    //develop entries
    SamplingEntry->SetState(kTRUE);
    PrecisionEntry->SetState(kTRUE);

    //add entries
    PrecisionFrame->AddFrame(PrecisionLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));
    PrecisionFrame->AddFrame(PrecisionEntry, new TGLayoutHints(kLHintsLeft,1,1,1,1));
    SamplingFrame->AddFrame(SamplingLabel, new TGLayoutHints(kLHintsLeft| kLHintsCenterY,1,1,1,1));
    SamplingFrame->AddFrame(SamplingEntry, new TGLayoutHints(kLHintsLeft | kLHintsCenterY,1,1,1,1));
    /*  
    StepXButtonGroup = new TGButtonGroup(SamplingFrame, "Step x [micron]", kHorizontalFrame);
    StepYButtonGroup = new TGButtonGroup(SamplingFrame, "Step y [micron]", kHorizontalFrame);
    StepXButton[0] = new TGRadioButton(StepXButtonGroup, new TGHotString("1"));
    StepXButton[1] = new TGRadioButton(StepXButtonGroup, new TGHotString("0.1"));
    StepYButton[0] = new TGRadioButton(StepYButtonGroup, new TGHotString("1"));
    StepYButton[1] = new TGRadioButton(StepYButtonGroup, new TGHotString("0.1"));


    
    //develop entries
    StepXButton[0]->SetState(kButtonDown);
    StepYButton[1]->SetState(kButtonDown);
    StepXButton[0]->SetOn(kTRUE);
    StepYButton[1]->SetOn(kTRUE);
    StepXButton[0]->Connect("Toggled(Bool_t)", "WFGUI", this, "SetStepx()");
    StepXButton[1]->Connect("Toggled(Bool_t)", "WFGUI", this, "SetStepx()");
    StepYButton[0]->Connect("Toggled(Bool_t)", "WFGUI", this, "SetStepy()");
    StepYButton[1]->Connect("Toggled(Bool_t)", "WFGUI", this, "SetStepy()");

    SetStepx();
    SetStepy();


    //add entries
    StepXButtonGroup->AddFrame(StepXButton[0], new TGLayoutHints(kLHintsCenterX,-50,10,0,0));
    StepXButtonGroup->AddFrame(StepXButton[1], new TGLayoutHints(kLHintsCenterX,5,10,0,0));
    StepYButtonGroup->AddFrame(StepYButton[0], new TGLayoutHints(kLHintsCenterX,-50,10,0,0));
    StepYButtonGroup->AddFrame(StepYButton[1], new TGLayoutHints(kLHintsCenterX,5,10,0,0));
    
    //add frame
    SamplingFrame->AddFrame(StepYButtonGroup,new TGLayoutHints(kLHintsRight| kLHintsExpandX,1,1,1,1));
    SamplingFrame->AddFrame(StepXButtonGroup,new TGLayoutHints(kLHintsRight| kLHintsExpandX,1,1,1,1));

*/    

    
    SamplingFrame->AddFrame(StepyEntry, new TGLayoutHints(kLHintsRight | kLHintsCenterY,1,1,1,1));    
    SamplingFrame->AddFrame(StepxEntry, new TGLayoutHints(kLHintsRight | kLHintsCenterY,1,1,1,1));
    SamplingFrame->AddFrame(StepLabel, new TGLayoutHints(kLHintsRight | kLHintsCenterY,1,1,1,1));
    
    //add frames
    PrecisionGroupFrame->AddFrame(PrecisionFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 0,0,0,0));
    PrecisionGroupFrame->AddFrame(SamplingFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 0,0,0,0));
    ControlFrame->AddFrame(PrecisionGroupFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 1,1,1,1));
    ////////////////////////////////////////////

    
    ////////////////////////////////////////////
    /////////OUTPUT FILE FRAME/////////////
    //create frames
    NameFrame = new TGGroupFrame(ControlFrame,"Output files for signals",kHorizontalFrame);
    
    //create entries
    
    FileNameOnButton = new TGCheckButton(NameFrame,"ON",0);
    FileNameLabel = new TGLabel(NameFrame, "Name:");
    FileNameEntry = new TGTextEntry(NameFrame, new TGTextBuffer(5),kSunkenFrame | kDoubleBorder | kOwnBackground);
    PrintLabel = new TGLabel(NameFrame, "GS/s");
    PrintEntry = new TGNumberEntry(NameFrame, valueMap["PRINT"],4,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0.1,10000);
    //develop entries

    FileNameOnButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetFileNameOn(Bool_t)");
    FileNameEntry->SetText("wf");
    FileNameEntry->SetMaxLength(4096);
    FileNameEntry->SetAlignment(kTextLeft);
    FileNameEntry->Resize(104,FileNameEntry->GetDefaultHeight());
    FileNameEntry->MoveResize(56,80,150,22);
    FileNameEntry->SetState(kFALSE);
    FileNameLabel->Disable(kTRUE);
    PrintEntry->SetState(kFALSE);
    PrintLabel->Disable(kTRUE);
    
    //add entries
    NameFrame->AddFrame(FileNameOnButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));
    NameFrame->AddFrame(PrintLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));
    NameFrame->AddFrame(PrintEntry, new TGLayoutHints(kLHintsLeft| kLHintsTop,1,1,1,1));
    NameFrame->AddFrame(FileNameLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));
    NameFrame->AddFrame(FileNameEntry, new TGLayoutHints(kLHintsRight| kLHintsTop | kLHintsExpandX,1,1,1,1));
	
    //add frames
    ControlFrame->AddFrame(NameFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX ,1,1,1,1));
    ////////////////////////////////////////////

    
    ////////////////////////////////////////////
    /////////BATCH MODE FRAME/////////////
    //create frames
	BatchFrame = new TGGroupFrame(ControlFrame,"Batch Mode",kHorizontalFrame);
    BatchOnButtonFrame = new TGHorizontalFrame(BatchFrame);
    BatchEventNumberFrame = new TGHorizontalFrame(BatchFrame);
    
    //create entries
    BatchOnButton = new TGCheckButton(BatchFrame, "ON",0);
    EventsLabel = new TGLabel(BatchFrame,"Starting at event:");
    EventsLabel2 = new TGLabel(BatchFrame," for events:");
    EventsEntry = new TGNumberEntry(BatchFrame, (Int_t) 1,3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    EventsEntryStart = new TGNumberEntry(BatchFrame, (Int_t) 1,3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);

    //develop entries
    BatchOnButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetBatchOn(Bool_t)");
    EventsEntry->SetState(kFALSE);
    EventsEntryStart->SetState(kFALSE);
    
    EventsLabel->Disable(kTRUE);
    EventsLabel2->Disable(kTRUE);

    //add entries
    BatchFrame->AddFrame(BatchOnButton, new TGLayoutHints(kLHintsLeft,1,1,1,1));
    BatchFrame->AddFrame(EventsLabel, new TGLayoutHints(kLHintsLeft,1,1,1,1));
    BatchFrame->AddFrame(EventsEntryStart, new TGLayoutHints(kLHintsLeft ,1,1,1,1));
    BatchFrame->AddFrame(EventsLabel2, new TGLayoutHints(kLHintsLeft,1,1,1,1));
    BatchFrame->AddFrame(EventsEntry, new TGLayoutHints(kLHintsLeft ,1,1,1,1));


    

    //add frames
    BatchFrame ->AddFrame(BatchOnButtonFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX ,0,0,0,0));
    BatchFrame ->AddFrame(BatchEventNumberFrame, new TGLayoutHints(kLHintsCenterX ,0,0,0,0));
    ControlFrame->AddFrame(BatchFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX ,1,1,1,1));
    ////////////////////////////////////////////
    
 
    
    ////////////////////////////////////////////
    /////////SELECT PARTICLES FRAME/////////////
    //create frames
    ParticlesPropertiesFrame = new TGGroupFrame(ControlFrame,"Select Particles",kVerticalFrame);

    ParticlesKindFrame = new TGVerticalFrame(ParticlesPropertiesFrame);
    ParticlesSpecificsFrame = new TGHorizontalFrame(ParticlesPropertiesFrame, kHorizontalFrame);
    NumberFrame = new TGHorizontalFrame(ParticlesPropertiesFrame);
    ParticlesSpecificsLabelFrame = new TGHorizontalFrame(ParticlesSpecificsFrame);
    ParticlesSpecificsSetFrame = new TGVerticalFrame(ParticlesSpecificsFrame);
    
    //create entries
    ParticleKind = new TGComboBox(ParticlesKindFrame);
    ParticleKind->AddEntry(new TGString("MIP: uniform Q, Qtot = q*[#eh/um]*Height"), 1);
    ParticleKind->AddEntry(new TGString("MIP: NON uniform Q, Qtot = q*[#eh/um]*Height"), 2);
    ParticleKind->AddEntry(new TGString("MIP Landau"), 3);
    ParticleKind->AddEntry(new TGString("Laser (1064 nm): Top-TCT, Q = q*[#eh/um]*Height"), 4);
    ParticleKind->AddEntry(new TGString("Laser (1064 nm): Edge-TCT, Q = q*[#eh/um]*Height"), 8);
    ParticleKind->AddEntry(new TGString("Edge MIP Landau"), 10);
    ParticleKind->AddEntry(new TGString("Laser (4-600 nm)/Alpha from top: E = 5MeV"), 5);
    ParticleKind->AddEntry(new TGString("Laser (4-600 nm)/Alpha from bottom: E = 5MeV"), 6);
    ParticleKind->AddEntry(new TGString("X-Ray"), 9);
    ParticleKind->AddEntry(new TGString("Current pulse"), 7);

    
    ParticleSpecificsEntry = new TGNumberEntry(ParticlesSpecificsSetFrame,  0,3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    ParticleSpecificsLabel1 = new TGLabel(ParticlesSpecificsLabelFrame, new TGString("#eh/um"));
    ParticleSpecificsLabel2 = new TGLabel(ParticlesSpecificsLabelFrame, new TGString("Range [um]"));
    ParticleSpecificsLabel3 = new TGLabel(ParticlesSpecificsLabelFrame, new TGString("Duration [ns]"));
    ParticleSpecificsLabel4 = new TGLabel(ParticlesSpecificsLabelFrame, new TGString(" E [keV]"));

    NumberLabel = new TGLabel(NumberFrame,"Number of Particles: ");
    NumberEntry = new TGNumberEntry(NumberFrame, valueMap["NUMBERP"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,1,100);
    
    //develop entries
    ParticleKind->Select(1);
    ParticleKind->Connect("Selected(Int_t)","WFGUI", this, "CallSetPart(Int_t)");
    ParticleKind->Resize(ParticleKind->GetWidth()+240, ParticleKind->GetHeight()+10);
    ParticleSpecificsEntry->SetState(kFALSE);
    ParticleSpecificsLabel1->SetMargins(0,0,2,2);
    ParticleSpecificsLabel1->Disable(kTRUE);
    ParticleSpecificsLabel2->SetMargins(0,0,2,2);
    ParticleSpecificsLabel2->Disable(kTRUE);
    ParticleSpecificsLabel3->SetMargins(0,0,2,2);
    ParticleSpecificsLabel3->Disable(kTRUE);
    ParticleSpecificsLabel3->SetMargins(0,0,2,2);
    ParticleSpecificsLabel3->Disable(kTRUE);
    ParticleSpecificsLabel4->Disable(kTRUE);
    ParticleSpecificsLabel4->SetMargins(0,0,2,2);
    ParticleSpecificsLabel4->Disable(kTRUE);
    NumberEntry->SetState(kTRUE);
    //CallSetPart(1)
    
    //add entries
    ParticlesKindFrame->AddFrame(ParticleKind, new TGLayoutHints(kLHintsLeft| kLHintsTop| kLHintsExpandX, 1,1,1,1));
    ParticlesSpecificsSetFrame->AddFrame(ParticleSpecificsEntry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));
    ParticlesSpecificsLabelFrame->AddFrame(ParticleSpecificsLabel1, new TGLayoutHints(kLHintsLeft |kLHintsTop,1,6,2,1));
    ParticlesSpecificsLabelFrame->AddFrame(ParticleSpecificsLabel2, new TGLayoutHints(kLHintsLeft |kLHintsTop,1,6,2,1));
    ParticlesSpecificsLabelFrame->AddFrame(ParticleSpecificsLabel3, new TGLayoutHints(kLHintsLeft |kLHintsTop,1,1,2,1));
    ParticlesSpecificsLabelFrame->AddFrame(ParticleSpecificsLabel4, new TGLayoutHints(kLHintsLeft |kLHintsTop,1,1,2,1));
    NumberFrame->AddFrame(NumberLabel, new TGLayoutHints(kLHintsLeft,1,1,1,1));
    NumberFrame->AddFrame(NumberEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));

    //add frames
    ParticlesPropertiesFrame -> AddFrame(ParticlesKindFrame, new TGLayoutHints( kLHintsExpandX, 0,0,0,0));
    ParticlesPropertiesFrame -> AddFrame(ParticlesSpecificsFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 0,0,0,0));
    ParticlesSpecificsFrame->AddFrame(ParticlesSpecificsLabelFrame,new TGLayoutHints(kLHintsLeft | kLHintsTop| kLHintsExpandX,0,0,0,0));
    ParticlesSpecificsFrame->AddFrame(ParticlesSpecificsSetFrame,new TGLayoutHints(kLHintsRight,0,0,0,0));
    ControlFrame -> AddFrame(ParticlesPropertiesFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX , 1,1,1,1));

    ///////////////// top frame /////
    //        NumberFrame = new TGHorizontalFrame(ParticlesPropertiesFrame);
    //CarriersInFrame = new TGGroupFrame(ParticlesPropertiesFrame,"Top",kHorizontalFrame);
     CarriersInFrame = new TGHorizontalFrame(ParticlesPropertiesFrame);
    CarriersInLabelFrame = new TGHorizontalFrame (CarriersInFrame);
    CarriersInSetFrame = new TGHorizontalFrame (CarriersInFrame);
    
    CarriersInLabel =  new TGLabel(CarriersInLabelFrame, new TGString("X[um],Angle[D]"));
    CarriersInNumberentry = new TGNumberEntry(CarriersInLabelFrame, (Double_t) 0,3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELLimitMinMax,0.,1000.);
    CarriersAngleNumberentry = new TGNumberEntry(CarriersInLabelFrame, (Double_t) 0,3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELLimitMinMax,-45,45);
    //    BatchRandomButton = new TGCheckButton(CarriersInLabelFrame, "Random Impact",0);

    EdgeNumberentry = new TGNumberEntry(CarriersInSetFrame, valueMap["YPOSITION"] ,3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELLimitMinMax,0.,1000.);
    BatchRandomButton = new TGCheckButton(CarriersInSetFrame, "Rnd",0);
    EdgeLabel =  new TGLabel(CarriersInLabelFrame, new TGString("Y[um]:"));
	
    //develop entries
    BatchRandomButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetBatchRandomOn(Bool_t)");
    EdgeNumberentry -> SetState(kFALSE);
    EdgeLabel ->Disable(kTRUE);
    
    // add entries
    CarriersInLabelFrame->AddFrame(CarriersInLabel, new TGLayoutHints(kLHintsLeft ,1,4,4,1));
    CarriersInLabelFrame->AddFrame(EdgeLabel, new TGLayoutHints(kLHintsLeft ,1,4,4,1));
    CarriersInLabelFrame->AddFrame(CarriersAngleNumberentry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    CarriersInLabelFrame->AddFrame(CarriersInNumberentry, new TGLayoutHints(kLHintsRight,1,1,1,1));


    ////    CarriersInSetFrame->AddFrame(EdgeLabel, new TGLayoutHints(kLHintsRight,1,4,4,1));
    CarriersInSetFrame->AddFrame(BatchRandomButton, new TGLayoutHints(kLHintsRight,1,1,2,0));
    CarriersInSetFrame->AddFrame(EdgeNumberentry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    // CarriersInLabelFrame->AddFrame(BatchRandomButton, new TGLayoutHints(kLHintsLeft,1,1,2,0));
    

    
    ParticlesPropertiesFrame->AddFrame(CarriersInFrame, new TGLayoutHints(kLHintsLeft| kLHintsExpandX,1,1,1,1));
    CarriersInFrame->AddFrame(CarriersInLabelFrame, new TGLayoutHints(kLHintsLeft| kLHintsExpandX,1,1,1,1));
    CarriersInFrame->AddFrame(CarriersInSetFrame, new TGLayoutHints(kLHintsRight,1,1,1,1));


    ParticlesPropertiesFrame->AddFrame(NumberFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 0,0,0,0));
    
    
    ////////////////////////////////////////////
    /////////IRRADIATION FRAME/////////////
    //create frames
    IrradiationFrame  = new TGGroupFrame(ControlFrame,"Irradiation",kVerticalFrame);
    
    //    IrradiationOnFrame = new TGVerticalFrame (IrradiationFrame);
 
    IrrParticleFrame = new TGHorizontalFrame(IrradiationFrame);

    IrradiationValuesFrame = new TGHorizontalFrame (IrradiationFrame); // 3 frames: on | label | set 
    IrradiationOnFrame = new TGVerticalFrame (IrradiationValuesFrame); //nc
    IrradiationLabelFrame = new TGVerticalFrame(IrradiationValuesFrame);
    IrradiationSetFrame = new TGVerticalFrame(IrradiationValuesFrame);
    
    //create entries
    //    IrradiationLabel3 = new TGLabel(IrradiationOnFrame,new TGString("Fluence [10^14 neq /cm^2]:"));
    IrradiationLabel3 = new TGLabel(IrrParticleFrame,new TGString("Fluence [10^14 neq /cm^2]:"));
    IrradiationEntry = new TGNumberEntry(IrrParticleFrame, valueMap["IRRADIATION"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,10000);
    
    DopingGLButtonGroup = new TGButtonGroup(IrrParticleFrame, "Gain layer doping", kHorizontalFrame);
    ParticleIrrButtonGroup = new TGButtonGroup(IrrParticleFrame, "Irradiation with: ", kHorizontalFrame);

    // IrradiationEntry = new TGNumberEntry(IrradiationSetFrame, valueMap["IRRADIATION"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,10000);

    IrradiationOnButton = new TGCheckButton(IrradiationOnFrame, "CCE beta e/h:",0);
    AcceptorCreationButton = new TGCheckButton(IrradiationOnFrame, "Acceptor creation",0);
    LnAcceptorCreationButton = new TGCheckButton(IrradiationLabelFrame, "ln(phi>5E15)",0);
    

    InitialDopRemovalButton = new TGCheckButton(IrradiationSetFrame, "Doping rem.",0);
    AllOnButton = new TGCheckButton(IrradiationSetFrame, "All On/Off",0);
    GainQuenchingButton = new TGCheckButton(IrradiationOnFrame, "Gain quenching due to fluence",0);
    QuenchEntry = new TGNumberEntry(IrradiationLabelFrame, valueMap["QUENCH"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,10000);
    
    IrradiationEntry2 = new TGNumberEntry(IrradiationLabelFrame, valueMap["BETA_ELECTRONS"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,10000);

    
    

    IrradiationEntry3 = new TGNumberEntry(IrradiationSetFrame, valueMap["BETA_HOLES"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,10000);
    // IrradiationLabel = new TGLabel(IrradiationLabelFrame,new TGString(":"));
    

    //    DopingGLButton[0] = new TGRadioButton( DopingGLButtonGroup, new TGHotString("B"));
    // DopingGLButton[1] = new TGRadioButton( DopingGLButtonGroup, new TGHotString("B+C"));
    //DopingGLButton[0]->SetState(kButtonDown);
    // DopingGLButton[0]->SetOn(kTRUE);  
    // DopingGLButton[0]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDopingGL()");
    // DopingGLButton[1]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDopingGL()");
    // CallSetDopingGL();

    
    ParticleIrrButton[0] = new TGRadioButton( ParticleIrrButtonGroup, new TGHotString("neutrons "));
    ParticleIrrButton[1] = new TGRadioButton( ParticleIrrButtonGroup, new TGHotString("protons/pions"));
    //  IrradiationLabel2 = new TGLabel(IrradiationLabelFrame,new TGString("beta_electrons:"));

    ParticleIrrButton[0]->SetState(kButtonDown);
    ParticleIrrButton[0]->SetOn(kTRUE);  
    ParticleIrrButton[0]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetParticleIrr()");
    ParticleIrrButton[1]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetParticleIrr()");
    CallSetParticleIrr();


    DJFrame = new TGHorizontalFrame(IrradiationFrame);
    DJDetailsFrame = new TGVerticalFrame(DJFrame);
    DJDetailsTopFrame= new TGHorizontalFrame(DJDetailsFrame);
    DJDetailsBottomFrame= new TGHorizontalFrame(DJDetailsFrame);
    DJOnButton = new TGCheckButton(DJFrame, "DJ ON ",0);

    DJButtonGroup = new TGButtonGroup(DJFrame, " ", kHorizontalFrame);
    DJButton[0] = new TGRadioButton(DJButtonGroup, new TGHotString("Linear"));
    DJButton[1] = new TGRadioButton(DJButtonGroup, new TGHotString("Step"));
    DJEntry = new TGNumberEntry(DJDetailsTopFrame, valueMap["DOUBLEJUNCTION"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,100);
    DJehEntry = new TGNumberEntry(DJDetailsBottomFrame, valueMap["NA_OVER_ND"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,100);
    DJLabel = new TGLabel(DJDetailsTopFrame,new TGString("   N_{Eff}[0-100]:"));
    DJehLabel = new TGLabel(DJDetailsBottomFrame,new TGString("  N_A/N_D:"));

    //develop entries
    IrradiationOnButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetCCEOn(Bool_t)");
    AcceptorCreationButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetAcceptorCreation(Bool_t)");
    LnAcceptorCreationButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetLnAcceptorCreation(Bool_t)");
    InitialDopRemovalButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetInitialDopRemoval(Bool_t)");
    AllOnButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetAllOn(Bool_t)");
    
    GainQuenchingButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetGainQuenching(Bool_t)");
    IrradiationEntry -> SetState(kFALSE);
    IrradiationEntry2 -> SetState(kFALSE);
    IrradiationEntry3 -> SetState(kFALSE);
    QuenchEntry -> SetState(kFALSE);
    
    //IrradiationLabel ->Disable(kTRUE);
    //IrradiationLabel2 ->Disable(kTRUE);
    //IrradiationLabel3 ->Disable(kTRUE);
    
    DJOnButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetDJOn(Bool_t)");
    DJEntry -> SetState(kFALSE);
    DJehEntry -> SetState(kFALSE);
    DJLabel ->Disable(kTRUE);
    DJehLabel ->Disable(kTRUE);
    DJButton[0]->SetState(kButtonDown);
    DJButton[0]->SetOn(kTRUE);
    DJButton[0]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDJType()");
    DJButton[1]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDJType()");
    CallSetDJType();

    //add entries
    IrrParticleFrame->AddFrame(IrradiationLabel3, new TGLayoutHints(kLHintsLeft | kLHintsCenterY,1,5,5,1));
    IrrParticleFrame->AddFrame(IrradiationEntry, new TGLayoutHints(kLHintsLeft | kLHintsCenterY,1,5,5,1));
    //    IrrParticleFrame->AddFrame(DopingGLButtonGroup, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 1,5,5,1));
    IrrParticleFrame->AddFrame(ParticleIrrButtonGroup, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,5,5,1));

    // IrradiationSetFrame->AddFrame(IrradiationEntry, new TGLayoutHints(kLHintsLeft,1,5,5,1));

    IrradiationOnFrame->AddFrame(IrradiationOnButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,5,5,1));
    IrradiationOnFrame->AddFrame(AcceptorCreationButton, new TGLayoutHints(kLHintsLeft,1,5,5,1));
    IrradiationOnFrame->AddFrame(GainQuenchingButton, new TGLayoutHints(kLHintsLeft,1,5,5,1));
    //    IrradiationSetFrame->AddFrame(IrradiationEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    IrradiationSetFrame->AddFrame(IrradiationEntry3, new TGLayoutHints(kLHintsRight,1,1,1,1));

    //    IrradiationLabelFrame->AddFrame(IrradiationLabel, new TGLayoutHints(kLHintsRight,1,5,5,1));
    //    ParticleIrrButtonGroup
    //    IrradiationLabelFrame->AddFrame(ParticleIrrButtonGroup, new TGLayoutHints(kLHintsRight,1,5,5,1));
    IrradiationLabelFrame->AddFrame(IrradiationEntry2, new TGLayoutHints(kLHintsRight,1,1,1,1));
    //    IrradiationLabelFrame->AddFrame(IrradiationLabel2, new TGLayoutHints(kLHintsRight,1,5,5,1));
    IrradiationLabelFrame->AddFrame(LnAcceptorCreationButton, new TGLayoutHints(kLHintsLeft,1,5,5,1));
    IrradiationLabelFrame->AddFrame(QuenchEntry, new TGLayoutHints(kLHintsLeft,1,5,5,1));
    // IrradiationSetFrame->AddFrame(IrradiationEntry, new TGLayoutHints(kLHintsLeft,1,5,5,1));
    IrradiationSetFrame->AddFrame(InitialDopRemovalButton, new TGLayoutHints(kLHintsLeft,1,5,5,1));
    IrradiationSetFrame->AddFrame(AllOnButton, new TGLayoutHints(kLHintsLeft,1,5,5,1));
    
    DJDetailsTopFrame->AddFrame(DJEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    DJDetailsTopFrame->AddFrame(DJLabel, new TGLayoutHints(kLHintsLeft | kLHintsExpandX,1,1,5,1));
    DJDetailsBottomFrame->AddFrame(DJehEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    DJDetailsBottomFrame->AddFrame(DJehLabel, new TGLayoutHints(kLHintsLeft | kLHintsExpandX,1,1,1,1));
    DJDetailsFrame->AddFrame(DJDetailsTopFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandX,1,1,1,1));
    DJDetailsFrame->AddFrame(DJDetailsBottomFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandX,1,1,1,1));
    
    //add frames
    IrradiationFrame -> AddFrame(IrrParticleFrame, new TGLayoutHints(kLHintsLeft, 0,0,0,0));
    IrradiationValuesFrame -> AddFrame(IrradiationOnFrame, new TGLayoutHints(kLHintsLeft, 0,0,0,0));
    IrradiationValuesFrame->AddFrame(IrradiationLabelFrame,new TGLayoutHints(kLHintsLeft| kLHintsExpandX,0,0,0,0));
    IrradiationValuesFrame->AddFrame(IrradiationSetFrame,new TGLayoutHints(kLHintsRight,0,0,0,0));
    IrradiationFrame -> AddFrame(IrradiationValuesFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 0,0,0,0));

    ControlFrame->AddFrame(IrradiationFrame ,new TGLayoutHints(kLHintsCenterY| kLHintsExpandX,1,1,1,1));
    IrradiationFrame -> AddFrame(DJFrame, new TGLayoutHints(kLHintsLeft, 0,0,0,0));
    DJFrame -> AddFrame(DJOnButton, new TGLayoutHints(kLHintsLeft, 0,0,15,0));
    DJFrame -> AddFrame(DJButtonGroup, new TGLayoutHints(kLHintsLeft, 0,0,0,0));
    DJFrame -> AddFrame(DJDetailsFrame, new TGLayoutHints(kLHintsLeft, 0,0,0,0));
    
    ////////////////////////////////
    
    ////////////////////////////////////////////
    /////////PLOT SETTINGS FRAME/////////////
    //create frames
    //  PlotGroupFrame = new TGGroupFrame(ControlFrame,"Plot Settings",kVerticalFrame); // GroupFrame: plot settings
    PlotGroupFrame = new TGGroupFrame(ControlFrame,"Drawing",kHorizontalFrame); // GroupFrame: plot settings
    //UpdateFrame = new TGHorizontalFrame(PlotGroupFrame);
    
    //create entries
    Eyes = new TGCheckButton(PlotGroupFrame, "Equipotential Lines",0);
    AbsCur = new TGCheckButton(PlotGroupFrame, "Current Absolute Value",0);
    ShowCur = new TGCheckButton(PlotGroupFrame, "e-h Motion",0);
    //LessPlotCheck = new TGCheckButton(UpdateFrame, "No 1D Plots",0);
    //Less2DPlotCheck = new TGCheckButton(UpdateFrame, "No 1D && 2D",0);

    //develop entries
	Eyes->Connect("Toggled(Bool_t)","WFGUI",this,"SetPlotFieldHist(Bool_t)");
	AbsCur->Connect("Toggled(Bool_t)","WFGUI",this,"SetAbsCur(Bool_t)");
	ShowCur->Connect("Toggled(Bool_t)","WFGUI",this,"SetShowCur(Bool_t)");
	//LessPlotCheck->Connect("Toggled(Bool_t)","WFGUI",this,"SetLessPlot(Bool_t)");
	//Less2DPlotCheck->Connect("Toggled(Bool_t)","WFGUI",this,"SetLess2DPlot(Bool_t)");

    //add entries
	PlotGroupFrame->AddFrame(Eyes, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,1,5));
	PlotGroupFrame->AddFrame(AbsCur, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,1,5));
	PlotGroupFrame->AddFrame(ShowCur, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,1,5));
    //UpdateFrame->AddFrame(LessPlotCheck, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,1,5));
    //UpdateFrame->AddFrame(Less2DPlotCheck, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,1,5));
    
    //add frames
	//PlotGroupFrame->AddFrame(UpdateFrame,new TGLayoutHints(kLHintsLeft,1,1,1,1));
	ControlFrame->AddFrame(PlotGroupFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////


	////////////////////////////////////////////
    /////////CURRENT SETTINGS FRAME/////////////
    //create frames
    CurrentsFrame = new TGGroupFrame(ControlFrame,"Current Settings",kVerticalFrame);
	BFieldFrame = new TGHorizontalFrame(CurrentsFrame);
    DiffusionAndDispersionFrame = new TGVerticalFrame(CurrentsFrame);
    TempSetFrame = new TGHorizontalFrame(CurrentsFrame);
    
    //create entries
    CurrentsButton[0] = new TGCheckButton(BFieldFrame,new TGHotString("B-Field on. Tesla (Positive = entering the plot) ="),0);
    BfieldEntry = new TGNumberEntry(BFieldFrame, (Double_t) 0,3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEAAnyNumber);
    CurrentsButton[1] = new TGCheckButton(DiffusionAndDispersionFrame,new TGHotString("Diffusion"),0);
    CurrentsButton[2] = new TGCheckButton(DiffusionAndDispersionFrame,new TGHotString("Charge Cloud Dispersion (no Alpha)"),0);
    TempLabel = new TGLabel(TempSetFrame,"Temperature [K]:");
    TempEntry = new TGNumberEntry(TempSetFrame, valueMap["TEMPERATURE"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
	
    //develop entries
    BfieldEntry->SetState(kFALSE);
    TempEntry->SetState(kTRUE);
    CurrentsButton[1]->Connect("Toggled(Bool_t)","WFGUI",this,"SetDiffusion(Bool_t)");
    CurrentsButton[0]->Connect("Toggled(Bool_t)","WFGUI",this,"SetBField(Bool_t)");
    CurrentsButton[2]->Connect("Toggled(Bool_t)","WFGUI",this,"SetChargeCloud(Bool_t)");
    
    //add entries
    BFieldFrame->AddFrame(CurrentsButton[0], new TGLayoutHints(kLHintsLeft,1,1,3,2));
    BFieldFrame->AddFrame(BfieldEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    DiffusionAndDispersionFrame->AddFrame(CurrentsButton[1], new TGLayoutHints(kLHintsLeft,1,1,2,1));
    DiffusionAndDispersionFrame->AddFrame(CurrentsButton[2], new TGLayoutHints(kLHintsLeft,1,1,1,2));
    TempSetFrame->AddFrame(TempLabel, new TGLayoutHints(kLHintsLeft,1,1,3,2));
    TempSetFrame->AddFrame(TempEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    
    //add frames
    CurrentsFrame->AddFrame(BFieldFrame, new TGLayoutHints(kLHintsTop| kLHintsExpandX,0,0,0,0));
    CurrentsFrame->AddFrame(DiffusionAndDispersionFrame, new TGLayoutHints(kLHintsTop| kLHintsExpandX,0,0,0,0));
	CurrentsFrame->AddFrame(TempSetFrame,new TGLayoutHints(kLHintsTop| kLHintsExpandX,0,0,0,0));
    ControlFrame->AddFrame(CurrentsFrame,new TGLayoutHints(kLHintsCenterY | kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////

	SettingsFrame2->AddFrame(ControlFrame, new TGLayoutHints(kLHintsLeft | kLHintsCenterY | kLHintsExpandY,1,1,1,1));

    //////END OF CONTROL FRAME/////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    
    
	///////////////////////////////////////////////////////////////////////////////////////////////
	//////START OF Right frame //////////////////////////////////////////////////////
    RightFrame = new TGVerticalFrame(SettingsFrame2);

        ///////////////////////////////////////////////////////////
    // Saving interface
    
    SaveFrameV = new TGGroupFrame(RightFrame,"Files in sensors/data && sensors/graph",kVerticalFrame);   
    //    SaveFrame = new TGGroupFrame(RightFrame,"Files in sensors/data && sensors/graph",kHorizontalFrame);
    //    SaveFrame = new TGGroupFrame(SaveFrameV,"",kHorizontalFrame);
    SaveFrame = new TGHorizontalFrame(SaveFrameV);   
    SaveFrameWF = new TGHorizontalFrame(SaveFrameV);
    WFButton = new TGCheckButton(SaveFrameWF, "Special WF file?",0);
    WFButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetWF(Bool_t)");
    SaveButton = new TGTextButton(SaveFrame, "Save");
    LoadButton = new TGTextButton(SaveFrame, "Load");
    
    SaveFileName = new TGTextEntry(SaveFrame, new TGTextBuffer(5), kSunkenFrame | kDoubleBorder | kOwnBackground);
    SaveFileNameWF = new TGTextEntry(SaveFrameWF, new TGTextBuffer(5), kSunkenFrame | kDoubleBorder | kOwnBackground);
    //    SaveFileName->SetText("sampleout.dat");
    SaveFileName->SetText(fname1.c_str());
    SaveFileName->SetMaxLength(2000);
    SaveFileName->SetAlignment(kTextLeft);
    SaveFileName->MoveResize(56, 80, 160, 22);
    SaveFileName->SetState(kTRUE);
    SaveButton->Associate(this);
    SaveButton->SetTextColor(1, kFALSE);
    SaveButton->Connect("Clicked()", "WFGUI", this, "SaveData()");
    SaveButton->SetEnabled(kTRUE);
    SaveButton->SetBackgroundColor(0x00ff00);    
    

    SaveFileNameWF->SetText(fname1WF.c_str());
    SaveFileNameWF->SetMaxLength(2000);
    SaveFileNameWF->SetAlignment(kTextLeft);
    SaveFileNameWF->MoveResize(56, 80, 160, 22);
    SaveFileNameWF->SetState(kTRUE);
    
    LoadButton->Associate(this);
    LoadButton->SetTextColor(1, kFALSE);
    LoadButton->Connect("Clicked()", "WFGUI", this, "LoadData()");
    //    LoadButton->Connect("Clicked()", "WFGUI", this, "GetParameters()");
    LoadButton->SetEnabled(kTRUE);
    LoadButton->SetBackgroundColor(0x00ff00);    
    SaveFrame->AddFrame(SaveButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));
    SaveFrame->AddFrame(LoadButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));
    SaveFrame->AddFrame(SaveFileName, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));
    SaveFrameWF->AddFrame(WFButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));
    SaveFrameWF->AddFrame(SaveFileNameWF, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));
    SaveButton1 = new TGTextButton(SaveFrame, "SaveGraph");
    SaveFileName1 = new TGTextEntry(SaveFrame, new TGTextBuffer(5), kSunkenFrame | kDoubleBorder | kOwnBackground);
    //    SaveFileName->SetText("sampleout.dat");
    SaveFileName1->SetText("graph");
    SaveFileName1->SetMaxLength(2000);
    SaveFileName1->SetAlignment(kTextLeft);
    SaveFileName1->MoveResize(56, 80, 100, 22);
    SaveFileName1->SetState(kTRUE);
    SaveButton1->Associate(this);
    SaveButton1->SetTextColor(1, kFALSE);
    SaveButton1->Connect("Clicked()", "WFGUI", this, "SaveGraph()");
    SaveButton1->SetEnabled(kTRUE);
    SaveButton1->SetBackgroundColor(0x00ff00);    

    SaveFrame->AddFrame(SaveButton1, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));
    //    SaveFrame->AddFrame(SaveFileName1, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));

   
    
    DetectorPropertiesFrame = new TGGroupFrame(RightFrame,"Detector Properties",kVerticalFrame);
	
    ////////////////////////////////////////////
    /////////TYPE FRAME/////////////
    //create frames
	TypeFrameGroup  = new TGButtonGroup(DetectorPropertiesFrame,"Type",kHorizontalFrame);
    
    //create entries
	TypeButton[0] = new TGRadioButton(TypeFrameGroup, new TGHotString("Si"));
	TypeButton[1] = new TGRadioButton(TypeFrameGroup, new TGHotString("Diamond"));
	TypeButton[2] = new TGRadioButton(TypeFrameGroup, new TGHotString("SiC"));
	TypeButton[0]->SetState(kButtonDown);

    //develop entries
	TypeButton[0]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDetType()");	
	TypeButton[1]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDetType()");	
	TypeButton[2]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDetType()");
	CallSetDetType();


    
    //add entries
    TypeFrameGroup->AddFrame(TypeButton[0], new TGLayoutHints(kLHintsCenterX,-160,10,0,0));
    TypeFrameGroup->AddFrame(TypeButton[1], new TGLayoutHints(kLHintsCenterX,-40,10,0,0));
    TypeFrameGroup->AddFrame(TypeButton[2], new TGLayoutHints(kLHintsRight,5,10,0,0));
    
    //add frames
    DetectorPropertiesFrame->AddFrame(TypeFrameGroup ,new TGLayoutHints(kLHintsCenterY| kLHintsExpandX,1,1,1,1));
    ////////////////////////////////////////////

    
    ////////////////////////////////////////////
    /////////DOPING TYPE FRAME/////////////
    //create frames
    //    DopingFrame = new TGGroupFrame(DetectorPropertiesFrame, "", kHorizontalFrame);
    DopingFrame = new TGHorizontalFrame(DetectorPropertiesFrame);

    DopStripButtonGroup = new TGButtonGroup(DopingFrame, "Strips", kHorizontalFrame);
    DopBulkButtonGroup = new TGButtonGroup(DopingFrame, "Bulk", kHorizontalFrame);
    //   DopStripButtonGroup = new TGButtonGroup(TypeFrameGroup, "Strips", kHorizontalFrame);
    //  DopBulkButtonGroup = new TGButtonGroup(TypeFrameGroup, "Bulk", kHorizontalFrame);
    
    //create entries
    StripButton[0] = new TGRadioButton(DopStripButtonGroup, new TGHotString("n"));
    StripButton[1] = new TGRadioButton(DopStripButtonGroup, new TGHotString("p"));
    
    BulkButton[0] = new TGRadioButton(DopBulkButtonGroup, new TGHotString("n"));
    BulkButton[1] = new TGRadioButton(DopBulkButtonGroup, new TGHotString("p"));
    
    //develop entries
    StripButton[0]->SetState(kButtonDown);
    BulkButton[1]->SetState(kButtonDown);
    StripButton[0]->SetOn(kTRUE);
    BulkButton[1]->SetOn(kTRUE);
    StripButton[0]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDopingStrip()");
    StripButton[1]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDopingStrip()");
    BulkButton[0]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDopingBulk()");
    BulkButton[1]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetDopingBulk()");
    
    CallSetDopingBulk();
    CallSetDopingStrip();
    
    //add entries
    DopStripButtonGroup->AddFrame(StripButton[0], new TGLayoutHints(kLHintsCenterX,-50,10,0,0));
    DopStripButtonGroup->AddFrame(StripButton[1], new TGLayoutHints(kLHintsCenterX,5,10,0,0));
    DopBulkButtonGroup->AddFrame(BulkButton[0], new TGLayoutHints(kLHintsCenterX,-50,10,0,0));
    DopBulkButtonGroup->AddFrame(BulkButton[1], new TGLayoutHints(kLHintsCenterX,5,10,0,0));
    
    //add frames
    //TypeFrameGroup->AddFrame(DopStripButtonGroup,new TGLayoutHints(kLHintsLeft| kLHintsExpandX,1,1,1,1));
    //TypeFrameGroup->AddFrame(DopBulkButtonGroup,new TGLayoutHints(kLHintsLeft| kLHintsExpandX,1,1,1,1));
    DopingFrame->AddFrame(DopStripButtonGroup,new TGLayoutHints(kLHintsLeft| kLHintsExpandX,1,1,1,1));
    DopingFrame->AddFrame(DopBulkButtonGroup,new TGLayoutHints(kLHintsLeft| kLHintsExpandX,1,1,1,1));
    DetectorPropertiesFrame->AddFrame(DopingFrame,new TGLayoutHints(kLHintsCenterY| kLHintsExpandX ,1,1,1,1));
    ////////////////////////////////////////////
    
    
    ////////////////////////////////////////////
    /////////DIMENSIONS FRAME/////////////
    //create frames
    DimensionsFrame  = new TGHorizontalFrame(DetectorPropertiesFrame);
    DimLabelFrame = new TGVerticalFrame(DimensionsFrame);//200 60
    DimSetFrame = new TGVerticalFrame(DimensionsFrame);
    WPFrame = new TGHorizontalFrame(DimSetFrame);//50 90
    ThickFrame = new TGHorizontalFrame(DimSetFrame);//50 90


   DopStripButtonGroup = new TGButtonGroup(DopingFrame, "Strips", kHorizontalFrame);
   DopBulkButtonGroup = new TGButtonGroup(DopingFrame, "Bulk", kHorizontalFrame);


    
    //create entries
    XMAXentry = new TGNumberEntry(DimSetFrame, valueMap["STRIP_NUMB"],3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative);
    XMAXLabel = new TGLabel(DimLabelFrame,new TGString("# of strips (1,3,5..):"));
    YMAXentry = new TGNumberEntry(ThickFrame, valueMap["DETECT_HEIGHT"],3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative);
    AdaptiveThicknessButton = new TGCheckButton(ThickFrame, "Adaptive",0);
    YMAXLabel = new TGLabel(DimLabelFrame,new TGString("Thickness [um]:"));
    PitchLabel = new TGLabel(DimLabelFrame,new TGString("Width [um], Pitch [um]:"));
    Pitchentry = new TGNumberEntry(WPFrame, valueMap["STR_PITCH"],4,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative);
    Widthentry = new TGNumberEntry(WPFrame, valueMap["STR_WIDTH"],4,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative);


    //   AdaptiveThicknessButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetAdaptive(Bool_t)");
    //add entries
    DimSetFrame->AddFrame(XMAXentry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));
    DimLabelFrame->AddFrame(XMAXLabel, new TGLayoutHints(kLHintsTop,1,1,1,1));
    //   DimSetFrame->AddFrame(YMAXentry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));
    DimLabelFrame->AddFrame(YMAXLabel, new TGLayoutHints(kLHintsTop,1,1,1,1));
    //    DimLabelFrame->AddFrame(AdaptiveThicknessButton, new TGLayoutHints(kLHintsTop,1,1,1,1));
    
    DimLabelFrame->AddFrame(PitchLabel, new TGLayoutHints(kLHintsTop,1,1,3,1));
    WPFrame->AddFrame(Pitchentry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));
    WPFrame->AddFrame(Widthentry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));

    ThickFrame->AddFrame(YMAXentry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));
    ThickFrame->AddFrame(AdaptiveThicknessButton, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));

    //develop entries
    PitchLabel->SetMargins(0,0,3,0);
    YMAXLabel->SetMargins(0,0,3,2);
    XMAXLabel->SetMargins(0,0,3,2);

    //add frames
    DimSetFrame->AddFrame(ThickFrame, new TGLayoutHints(kLHintsRight | kLHintsTop,0,0,0,0));
    DimSetFrame->AddFrame(WPFrame, new TGLayoutHints(kLHintsRight | kLHintsTop,0,0,0,0));
    DimensionsFrame->AddFrame(DimLabelFrame,new TGLayoutHints(kLHintsLeft,0,0,0,0));
    DimensionsFrame->AddFrame(DimSetFrame,new TGLayoutHints(kLHintsRight,0,0,0,0));
    DetectorPropertiesFrame->AddFrame(DimensionsFrame ,new TGLayoutHints(kLHintsCenterY| kLHintsExpandX ,1,1,1,1));
    ////////////////////////////////////////////
    /////////VOLTAGE FRAME/////////////
    //create frames
    VoltageFrame  = new TGHorizontalFrame(DetectorPropertiesFrame);
    VolLabelFrame = new TGVerticalFrame(VoltageFrame);
    VolSetFrame = new TGHorizontalFrame(VoltageFrame);
    
    //create entries
	BiasLabel = new TGLabel(VolLabelFrame,new TGString("Bias [V], Bulk Depletion (new) [V]:"));
    Biasentry = new TGNumberEntry(VolSetFrame, valueMap["BIAS_VOLTAGE"],4,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative);
    Depletionentry = new TGNumberEntry(VolSetFrame, valueMap["DEPL_VOLTAGE"],4,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative);

    //add entries
	VolLabelFrame->AddFrame(BiasLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,1,1,1,1));
	VolSetFrame->AddFrame(Biasentry, new TGLayoutHints(kLHintsCenterX | kLHintsTop,1,1,1,1));
	VolSetFrame->AddFrame(Depletionentry, new TGLayoutHints(kLHintsCenterX | kLHintsTop,1,1,1,1));											//
	
    //develop entries
    BiasLabel->SetMargins(0,0,3,0);
    
    //add frames
    VoltageFrame->AddFrame(VolLabelFrame,new TGLayoutHints(kLHintsLeft,0,0,0,0));
    VoltageFrame->AddFrame(VolSetFrame,new TGLayoutHints(kLHintsRight,0,0,0,0));
    DetectorPropertiesFrame->AddFrame(VoltageFrame ,new TGLayoutHints(kLHintsCenterY| kLHintsExpandX ,0,0,0,0));
    ////////////////////////////////////////////

    ////////////////////////////////////////////
    /////////GAIN FRAME/////////////
    //create frames
	GainBigFrame  = new TGGroupFrame(DetectorPropertiesFrame,"Gain",kVerticalFrame);
	GainDopingFrame = new TGVerticalFrame(GainBigFrame);
	GainKindFrame = new TGVerticalFrame(GainBigFrame);
	GainShapeFrame = new TGVerticalFrame(GainBigFrame);
	GainFrame  = new TGHorizontalFrame(GainBigFrame);
	GainSetFrame = new TGVerticalFrame(GainFrame);//200 60
	GainLabelFrame = new TGVerticalFrame(GainFrame);
	ForceGainFrame = new TGHorizontalFrame(GainLabelFrame);//50 90
	ScreenedGainFrame = new TGHorizontalFrame(GainLabelFrame);//50 90
	
    //create entries
	GainDoping = new TGComboBox(GainDopingFrame);
	GainDoping->AddEntry(new TGString("No gain layer implant"), 0);
	GainDoping->AddEntry(new TGString("Boron"), 1);
	GainDoping->AddEntry(new TGString("Boron + Carbon"), 2);
	GainDoping->AddEntry(new TGString("Gallium"), 3);
	GainDoping->AddEntry(new TGString("Gallium + Carbon"), 4);
	//	GainDoping->AddEntry(new TGString("Boron Low Diffusion"), 5);
	
	GainKind = new TGComboBox(GainKindFrame);
	GainKind->AddEntry(new TGString("Gain Mechanism = OFF"), 0);
	GainKind->AddEntry(new TGString("Massey  - WF2 LGAD model"), 2);
	GainKind->AddEntry(new TGString("Massey optimized"), 5);
	GainKind->AddEntry(new TGString("van Overstraeten optimized"), 6);
	GainKind->AddEntry(new TGString("Okuto - Crowell model"), 3);
	GainKind->AddEntry(new TGString("Bologna model"), 4);
	GainKind->AddEntry(new TGString("van Overstraeten – de Man model"), 1);

	GainShape = new TGComboBox(GainShapeFrame);
	GainShape->AddEntry(new TGString(" - "), 0);
	GainShape->AddEntry(new TGString("I) Implant decreasing linearly from Junction"), 1);
	GainShape->AddEntry(new TGString("II) Implant at 0.5 - 1.0 micron"), 2);
	GainShape->AddEntry(new TGString("III) Implant at 0.6 - 1.0 micron"), 4);
	GainShape->AddEntry(new TGString("IV) Implant at 1.1 - 1.5 micron"), 7);
	GainShape->AddEntry(new TGString("V) Implant at 1.3 - 1.9 micron"), 5);
	GainShape->AddEntry(new TGString("VI) Implant at 1.6 - 2.1 micron"), 8);
	GainShape->AddEntry(new TGString("VII) Implant at 1.8 - 2.2 micron"), 6);		
	GainShape->AddEntry(new TGString("VIII) Implant at 0.0 - 3.0 micron"), 3);
	GainShape->AddEntry(new TGString("IX) Implant at 3.0 - 3.5 micron"), 9);
	GainShape->AddEntry(new TGString("X) Implant pn near ohmic contact"), 10);
	
	
	Dopingentry = new TGNumberEntry(GainSetFrame, valueMap["DOP_LEV"],4,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0.,500.);
	DopingLabel = new TGLabel(GainLabelFrame,new TGString("Gain implant peak doping [10^16/cm^3]"));

	//	ForceGainButton = new TGCheckButton(GainLabelFrame, "Force Gain",0);
	//	PrecisionGainButton = new TGCheckButton(GainLabelFrame, "High precision?",0);	
	ForceGainButton = new TGCheckButton(ForceGainFrame, "Force Gain",0);
	PrecisionGainButton = new TGCheckButton(ForceGainFrame, "High precision",0);
	ScreenedGainButton = new TGCheckButton(ScreenedGainFrame, "Screened Gain",0);
	///  

	//	ForceGainLabel = new TGLabel(GainLabelFrame,new TGString("Force Gain"));
	Gainentry = new TGNumberEntry(GainSetFrame, valueMap["GAIN_SCL"],4,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,1.,500.);
	ScreenedGainEntry = new TGNumberEntry(ScreenedGainFrame, valueMap["SGAIN_SCL"],4,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0.,500.);
	//	GainLabel = new TGLabel(GainLabelFrame,new TGString("    Gain:"));
	//	GainRatioentry = new TGNumberEntry(GainSetFrame, valueMap["HE_GAIN_RAT"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0.,1.);
	//	GainRatioLabel = new TGLabel(GainLabelFrame,new TGString("h/e Gain ratio:"));
	GainIndententry = new TGNumberEntry(GainSetFrame, valueMap["GAIN_LYR_RSS"],4,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0.,100.);
	//	GainIndentLabel = new TGLabel(GainLabelFrame,new TGString("Gain recess (um):"));
	GainIndentLabel = new TGLabel(ScreenedGainFrame,new TGString(" Recess [um]:"));
	
    //develop entries

	//	GainDoping->Select(0);
	GainDoping->Connect("Selected(Int_t)","WFGUI", this, "SetGainDoping(Int_t)");
	GainDoping->Resize(GainDoping->GetWidth()+240, GainDoping->GetHeight()+10);
	
	//	GainKind->Select(2);
	GainKind->Connect("Selected(Int_t)","WFGUI", this, "SetGainKind(Int_t)");
	GainKind->Resize(GainKind->GetWidth()+240, GainKind->GetHeight()+10);

	//	GainShape->Select(0);
	GainShape->Connect("Selected(Int_t)","WFGUI", this, "SetGainShape(Int_t)");
	GainShape->Resize(GainShape->GetWidth()+240, GainShape->GetHeight()+10);
	
	ForceGainButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetForceGain(Bool_t)");
	ScreenedGainButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetScreenedGain(Bool_t)");
	PrecisionGainButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetPrecisionGain(Bool_t)");
	ScreenedGainEntry ->SetState(kFALSE);
	Gainentry ->SetState(kFALSE);
	//GainLabel ->Disable(kTRUE);
	
	//GainIndententry ->SetState(kFALSE);
	//GainIndentLabel ->Disable(kTRUE);
	
    //add entries
	GainDopingFrame->AddFrame(GainDoping, new TGLayoutHints(kLHintsLeft| kLHintsTop| kLHintsExpandX, 1,1,1,1));
	GainKindFrame->AddFrame(GainKind, new TGLayoutHints(kLHintsLeft| kLHintsTop| kLHintsExpandX, 1,1,1,1));
	GainShapeFrame->AddFrame(GainShape, new TGLayoutHints(kLHintsLeft| kLHintsTop| kLHintsExpandX, 1,1,1,1));
    	GainSetFrame->AddFrame(Dopingentry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));
	GainLabelFrame->AddFrame(DopingLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,1,1,3,2));
	//	GainSetFrame->AddFrame(ForceGainButton, new TGLayoutHints(kLHintsRight,1,1,1,1));

	//GainLabelFrame->AddFrame(ForceGainButton, new TGLayoutHints(kLHintsRight,1,1,1,1));
	//GainLabelFrame->AddFrame(PrecisionGainButton, new TGLayoutHints(kLHintsRight,1,1,1,1));


       ForceGainFrame->AddFrame(PrecisionGainButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));
       ForceGainFrame->AddFrame(ForceGainButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));
       GainLabelFrame->AddFrame(ForceGainFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));
       GainLabelFrame->AddFrame(ScreenedGainFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop,1,1,1,1));	

	//	GainLabelFrame->AddFrame(ForceGainLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,1,1,3,2));
	GainSetFrame->AddFrame(Gainentry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));
	//GainLabelFrame->AddFrame(GainLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,1,1,3,2));
	//	GainSetFrame->AddFrame(GainRatioentry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));
	//	GainLabelFrame->AddFrame(GainRatioLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,1,1,3,2));
	GainSetFrame->AddFrame(GainIndententry, new TGLayoutHints(kLHintsRight | kLHintsTop,1,1,1,1));
	//	GainLabelFrame->AddFrame(GainIndentLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,1,1,3,1));
	ScreenedGainFrame->AddFrame(ScreenedGainButton, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,1,1,1,1));
	ScreenedGainFrame->AddFrame(ScreenedGainEntry, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,1,1,1,1));
	ScreenedGainFrame->AddFrame(GainIndentLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,1,1,1,1));


    //add frames
	GainBigFrame -> AddFrame(GainDopingFrame, new TGLayoutHints( kLHintsExpandX, 0,0,0,0));
	GainBigFrame -> AddFrame(GainShapeFrame, new TGLayoutHints( kLHintsExpandX, 0,0,0,0));
	GainBigFrame -> AddFrame(GainKindFrame, new TGLayoutHints( kLHintsExpandX, 0,0,0,0));
	GainBigFrame -> AddFrame(GainFrame, new TGLayoutHints( kLHintsExpandX, 0,0,0,0));
	GainFrame->AddFrame(GainSetFrame, new TGLayoutHints(kLHintsRight | kLHintsTop,0,0,0,0));
	GainFrame->AddFrame(GainLabelFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,0,0));
	DetectorPropertiesFrame->AddFrame(GainBigFrame ,new TGLayoutHints(kLHintsCenterY| kLHintsExpandX ,1,1,1,1));

    ////////////////////////////////////////////


    ////////////////////////////////////////////

    //    RightFrame->AddFrame(SaveFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX ,1,1,1,1));
    SaveFrameV->AddFrame(SaveFrame, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX ,1,1,1,1));
    SaveFrameV->AddFrame(SaveFrameWF, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX ,1,1,1,1));
    RightFrame->AddFrame(SaveFrameV, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX ,1,1,1,1));
    RightFrame->AddFrame(DetectorPropertiesFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 1,1,1,1));	
    
    //////END OF DETECTOR PROPERTIES FRAME/////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
 
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //////START OF OSCILLOSCOPE FRAME /////////////////////////////////////////////////////////////
	//create frames
    ElectronicsFrame = new TGGroupFrame(RightFrame," Electronics",kVerticalFrame);


   //////START OF READ OUT FRAME /////////////////////////////////////////////////////////////////
    //create frames
    //    ReadoutFrame = new TGGroupFrame(RightFrame,"Readout",kHorizontalFrame);
    // ReadOutButtonGroup = new TGButtonGroup(ReadoutFrame, "Side", kHorizontalFrame);

    ReadOutButtonGroup = new TGButtonGroup(ElectronicsFrame, "Readout", kHorizontalFrame);

    //create entries
    ReadOutButton[0] = new TGRadioButton( ReadOutButtonGroup, new TGHotString("Top - DC "));
    ReadOutButton[1] = new TGRadioButton( ReadOutButtonGroup, new TGHotString("Top - AC "));
    ReadOutButton[2] = new TGRadioButton( ReadOutButtonGroup, new TGHotString("Back "));
    
    ReadOutButton[0]->SetState(kButtonDown);
    
    //    ACButton = new TGCheckButton(ReadOutButtonGroup, "AC",0);

    //    ACButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetAC(Bool_t)");
    //    ReadOutButtonGroup->AddFrame(ACButton, new TGLayoutHints(kLHintsLeft | kLHintsTop ,5,5,1,5));

    // ACButton = new TGCheckButton(ReadoutFrame, "AC",0);
    //   ACEntry = new TGNumberEntry(ReadoutFrame, valueMap["AC"],4,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0.,100.);
    
    //develop entries
    ReadOutButton[0]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetReadOut()");
    ReadOutButton[1]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetReadOut()");
    ReadOutButton[2]->Connect("Toggled(Bool_t)", "WFGUI", this, "CallSetReadOut()");
    

    CallSetReadOut();//why is this called even after it's connected?
    
    //    ACButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetAC(Bool_t)");
    // SetAC(0);
    //    ReadoutFrame-> AddFrame(ReadOutButtonGroup, new TGLayoutHints(kLHintsLeft, 0,0,15,0));
    ElectronicsFrame-> AddFrame(ReadOutButtonGroup, new TGLayoutHints(kLHintsCenterY| kLHintsExpandX ,1,1,1,1));
    // ReadoutFrame -> AddFrame(ACButton, new TGLayoutHints(kLHintsLeft, 0,0,15,0));
   //   ReadoutFrame -> AddFrame(ACEntry, new TGLayoutHints(kLHintsLeft, 0,0,15,0));
    
    //add frames
    //RightFrame->AddFrame(ReadoutFrame,new TGLayoutHints(kLHintsLeft| kLHintsExpandX,1,1,1,1));
    //////END OF READ OUT FRAME////////////////////////////////////////////////////////////////////


    
    OscilloscopeFrame = new TGHorizontalFrame(ElectronicsFrame);	
    OscilloscopeLeftFrame = new TGVerticalFrame(OscilloscopeFrame);	
    OscilloscopeRightFrame = new TGVerticalFrame(OscilloscopeFrame);	
    TRiseSetFrame = new TGHorizontalFrame(OscilloscopeRightFrame,200,90);
    ShImpFrame = new TGHorizontalFrame(OscilloscopeRightFrame,200,90);
    LCFrame = new TGHorizontalFrame(OscilloscopeRightFrame,200,90);
    ShNoiseFrame = new TGHorizontalFrame(OscilloscopeRightFrame,200,90);
    BBBWFrame = new TGHorizontalFrame(OscilloscopeRightFrame,200,90);
    BBNoiseFrame = new TGHorizontalFrame(OscilloscopeRightFrame,200,90);

    //create entries
    //    OscOnButton = new TGCheckButton(ElectronicsFrame, "ON",0);
    OscOnButton = new TGCheckButton(OscilloscopeLeftFrame, "ON",0);
 
    CSAKind = new TGComboBox(OscilloscopeRightFrame);
    CSAKind->AddEntry(new TGString("Select Cdet first"), 20);
    CSAKind->AddEntry(new TGString("Generic CSA"), 0);
    CSAKind->AddEntry(new TGString("CSA: TOFFEE"), 1);
    CSAKind->AddEntry(new TGString("CSA: NA62"), 2);
    CSAKind->AddEntry(new TGString("CSA: SC"), 3);
    CSAKind->AddEntry(new TGString("TI: FAST low gain"), 4);
    CSAKind->AddEntry(new TGString("TI: FAST mid gain"), 5);
    CSAKind->AddEntry(new TGString("TI: FAST high gain"), 6);
    CSAKind->AddEntry(new TGString("TI: SC"), 7);

    CSAKind->Select(20);
    CSAKind->Connect("Selected(Int_t)","WFGUI", this, "CallSetCSA(Int_t)");
    CSAKind->Resize(CSAKind->GetWidth()+120, CSAKind->GetHeight()+10);
    OscOnButton->Resize(CSAKind->GetWidth(), OscOnButton->GetHeight()+10);



    
    //    NA62OnButton = new TGCheckButton(OscilloscopeRightFrame, "CSA NA62",0);
    
    CDLabel = new TGLabel(OscilloscopeLeftFrame,new TGString("Detector Cdet [pF] RC [ns]:"));
    CDEntry = new TGNumberEntry(LCFrame, valueMap["CAPACITANCE"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,.1,40.);
    LDEntry = new TGNumberEntry(LCFrame, valueMap["INDUCTANCE"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,1.,40.);

    OscBWLabel = new TGLabel(OscilloscopeLeftFrame,new TGString("Scope (50 [Ohm]) BW[GHz]:"));
    OscBWEntry = new TGNumberEntry(OscilloscopeRightFrame, valueMap["OSCOPE_BW"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    ImpLabel = new TGLabel(OscilloscopeLeftFrame,new TGString("CSA:Imp[Ohm],Tr.Imp[mV/fC]:"));
    //    ImpEntry = new TGNumberEntry(OscilloscopeRightFrame, valueMap["IMPEDANCE"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    TRiseLabel = new TGLabel(OscilloscopeLeftFrame, new TGString("CSA(Cdet=0)T_r,f(10-90%)[ns]:"));
    TFallEntry = new TGNumberEntry(TRiseSetFrame, valueMap["SHPR_DCY_TIME"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0.1,40.);
    TRiseEntry = new TGNumberEntry(TRiseSetFrame, valueMap["SHPR_INT_TIME"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0.1,60.);
    //    ShTransLabel = new TGLabel(OscilloscopeLeftFrame,new TGString("CSA Trans Imp.[mV/fC]:"));
    CSAImpEntry = new TGNumberEntry(ShImpFrame, valueMap["IMPEDANCE"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    ShTransEntry = new TGNumberEntry(ShImpFrame, valueMap["SHPR_TRANS"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    ShNoiseLabel = new TGLabel(OscilloscopeLeftFrame,new TGString("CSA:Noise,Vth[mV,CFD if<1]:"));
    CSAVthEntry = new TGNumberEntry(ShNoiseFrame, valueMap["VTH"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    ShNoiseEntry = new TGNumberEntry(ShNoiseFrame, valueMap["SHPR_NOISE"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    BBBWLabel = new TGLabel(OscilloscopeLeftFrame,new TGString("TI:Imp[Ohm],BW[GHz],Gain [kOhm]:"));
    BBImpEntry = new TGNumberEntry(BBBWFrame, valueMap["BB_IMP"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    BBGainEntry = new TGNumberEntry(BBBWFrame, valueMap["BBGAIN"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    BBBWEntry = new TGNumberEntry(BBBWFrame, valueMap["BBBW"],4,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    BBNoiseLabel = new TGLabel(OscilloscopeLeftFrame,new TGString("TI:Noise,Vth[mV,CFD if<1]:"));
    BBVthEntry = new TGNumberEntry(BBNoiseFrame, valueMap["BBVTH"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);
    BBNoiseEntry = new TGNumberEntry(BBNoiseFrame, valueMap["BB_NOISE"],3,-1,TGNumberFormat::kNESReal,TGNumberFormat::kNEANonNegative);

    //develop entries
    OscOnButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetOscOn(Bool_t)");
    //    NA62OnButton->Connect("Toggled(Bool_t)","WFGUI",this,"SetNA62On(Bool_t)");
    // NA62OnButton ->SetEnabled(kFALSE);
   
    OscBWLabel->SetMargins(0,0,4,5);
    ImpLabel->SetMargins(0,0,4,5);
    TRiseLabel->SetMargins(0,0,4,5);
    CDLabel->SetMargins(0,0,4,5);
    //    ShTransLabel->SetMargins(0,0,4,5);
    ShNoiseLabel->SetMargins(0,0,4,5);
    BBBWLabel->SetMargins(0,0,4,5);
				 BBNoiseLabel->SetMargins(0,0,4,5);

    //add entries
    //    ElectronicsFrame->AddFrame(OscOnButton, new TGLayoutHints(kLHintsLeft | kLHintsTop ,5,5,1,5));
    // ElectronicsFrame->AddFrame(NA62OnButton, new TGLayoutHints(kLHintsLeft | kLHintsTop ,5,5,1,5));

 OscilloscopeLeftFrame->AddFrame(OscOnButton, new TGLayoutHints(kLHintsLeft | kLHintsTop ,5,5,1,5));
 
    //    OscilloscopeRightFrame->AddFrame(NA62OnButton, new TGLayoutHints(kLHintsLeft | kLHintsTop ,5,5,1,5));
    OscilloscopeRightFrame->AddFrame(CSAKind, new TGLayoutHints(kLHintsLeft | kLHintsTop ,5,5,1,5));
    OscilloscopeLeftFrame->AddFrame(CDLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,1,1));
    OscilloscopeLeftFrame->AddFrame(OscBWLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,1,1));
    OscilloscopeLeftFrame->AddFrame(ImpLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,1,1));
    OscilloscopeLeftFrame->AddFrame(TRiseLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,1,1));
    //    OscilloscopeLeftFrame->AddFrame(ShTransLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,1,1));
    OscilloscopeLeftFrame->AddFrame(ShNoiseLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,1,1));
    OscilloscopeLeftFrame->AddFrame(BBBWLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,1,1));
    OscilloscopeLeftFrame->AddFrame(BBNoiseLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,0,1,1));

    //    OscilloscopeRightFrame->AddFrame(CDEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    //OscilloscopeRightFrame->AddFrame(LDEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    LCFrame->AddFrame(LDEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    LCFrame->AddFrame(CDEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));

    //    OscilloscopeRightFrame->AddFrame(ImpEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    TRiseSetFrame->AddFrame(TFallEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    TRiseSetFrame->AddFrame(TRiseEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    ShImpFrame->AddFrame(ShTransEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    ShImpFrame->AddFrame(CSAImpEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    ShNoiseFrame->AddFrame(CSAVthEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    ShNoiseFrame->AddFrame(ShNoiseEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));

    BBBWFrame->AddFrame(BBGainEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    BBBWFrame->AddFrame(BBBWEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    BBBWFrame->AddFrame(BBImpEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    BBNoiseFrame->AddFrame(BBVthEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));
    BBNoiseFrame->AddFrame(BBNoiseEntry, new TGLayoutHints(kLHintsRight,1,1,1,1));

    //add frames
    OscilloscopeRightFrame->AddFrame(LCFrame, new TGLayoutHints(kLHintsRight,1,1,1,1));    
    OscilloscopeRightFrame->AddFrame(OscBWEntry, new TGLayoutHints(kLHintsRight ,1,1,1,1));    
    OscilloscopeRightFrame->AddFrame(ShImpFrame, new TGLayoutHints(kLHintsRight,0,0,1,1));
    OscilloscopeRightFrame->AddFrame(TRiseSetFrame, new TGLayoutHints(kLHintsRight,0,0,1,1));
    OscilloscopeRightFrame->AddFrame(ShNoiseFrame, new TGLayoutHints(kLHintsRight,0,0,1,1));
    OscilloscopeRightFrame->AddFrame(BBBWFrame, new TGLayoutHints(kLHintsRight,0,0,1,1));
    OscilloscopeRightFrame->AddFrame(BBNoiseFrame, new TGLayoutHints(kLHintsRight,0,0,1,1));
    OscilloscopeFrame->AddFrame(OscilloscopeLeftFrame, new TGLayoutHints(kLHintsLeft |  kLHintsExpandX, 1,1,1,1));
    OscilloscopeFrame->AddFrame(OscilloscopeRightFrame, new TGLayoutHints(kLHintsRight, 1,1,1,1));
    ElectronicsFrame->AddFrame(OscilloscopeFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX , 1,1,1,1));
    RightFrame->AddFrame(ElectronicsFrame, new TGLayoutHints(kLHintsExpandX| kLHintsTop , 0,0,0,0));
    // NC 1
    SettingsFrame2->AddFrame(RightFrame, new TGLayoutHints(kLHintsCenterY , 1,1,1,1));
    //////END OF OSCILLOSOPE FRAME/////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // NC 2
    SettingsGlobalFrame->AddFrame(SettingsFrame2,new TGLayoutHints( kLHintsCenterY | kLHintsExpandY,1,1,1,1));
    // NC 3
    AddFrame(SettingsGlobalFrame,new TGLayoutHints(kLHintsCenterX,2,2,2,2));
    SetOscOn(0);

    
    ///END OF SETTINGS FRAME///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    
    ///*********************************************** END OF SETTINGS ****************************************************///

    
    
    ///******************************************** CANVAS AND HISTOGRAMS *************************************************///
	
    //set canvas specifics
    canvasp = driftcanvas->GetCanvas();

    canvasp->SetBottomMargin(0.15);
    canvasp->SetTopMargin(0.1);
    canvasp->SetLeftMargin(0.15);
    canvasp->SetRightMargin(0.15);
    
    canvaspc = driftpcanvas->GetCanvas();
    canvaspc->Divide(2,2,0.004,0.004);
    canvaspc->SetBottomMargin(0.15);
    canvaspc->SetTopMargin(0.1);
    canvaspc->SetLeftMargin(0.15);
    canvaspc->SetRightMargin(0.15);
    canvaspc->cd(1);

    canvasw = weightcanvas->GetCanvas();
    canvasw->SetBottomMargin(0.15);
    canvasw->SetTopMargin(0.1);
    canvasw->SetLeftMargin(0.15);
    canvasw->SetRightMargin(0.15);
    
    canvaswc = weightpcanvas->GetCanvas();
    canvaswc->Divide(2,1,0.002,0.002);
    canvaswc->SetBottomMargin(0.15);
    canvaswc->SetTopMargin(0.1);
    canvaswc->SetLeftMargin(0.15);
    canvaswc->SetRightMargin(0.15);


    
    curcanvas=currentscanvas->GetCanvas();
    curcanvas->SetBottomMargin(0.15);
    curcanvas->SetTopMargin(0.1);
    curcanvas->SetLeftMargin(0.15);
    curcanvas->SetRightMargin(0.15);

    osccanvas=oscilloscopecanvas->GetCanvas();
    osccanvas->SetBottomMargin(0.15);
    osccanvas->SetTopMargin(0.1);
    osccanvas->SetLeftMargin(0.2);
    osccanvas->SetRightMargin(0.2);

    eleIIcanvas=electronicsIIcanvas->GetCanvas();
    eleIIcanvas->SetBottomMargin(0.15);
    eleIIcanvas->SetTopMargin(0.1);
    eleIIcanvas->SetLeftMargin(0.2);
    eleIIcanvas->SetRightMargin(0.2);

    
    gPad->SetRightMargin(0.2);
    canvaspc->cd(2);
    gPad->SetRightMargin(0.2);
    
    
    // Create histograms for potentials and fields
    ellipse = new TEllipse();

	
	//	dhist = new TH2F("dhist"," ; x [um]; y [um]", dwpot.GetXMAX()*dwpot.GetBinSizex(), 0, dwpot.GetXMAX() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );  
    //  whist = new TH2F("whist"," ; x [um]; y [um]",dwpot.GetXMAX()*dwpot.GetBinSizex(), 0, dwpot.GetXMAX() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );  
    // wfhist = new TH2F("wfhist"," ",dwpot.GetXMAX()*dwpot.GetBinSizex(), 0, dwpot.GetXMAX() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );  
    // dfhist = new TH2F("dfhist"," ",dwpot.GetXMAX()*dwpot.GetBinSizex(), 0, dwpot.GetXMAX() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );  
    // chist = new TH2F("chist"," ; x [um]; y [um]",dwpot.GetXMAX()*dwpot.GetBinSizex(), 0, dwpot.GetXMAX() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );  
    // chhist = new TH2F("chhist"," ; x [um]; y [um]",dwpot.GetXMAX()*dwpot.GetBinSizex(), 0, dwpot.GetXMAX() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );  
    // ctothist = new TH2F("ctothist"," ; x [um]; y [um]",dwpot.GetXMAX()*dwpot.GetBinSizex(), 0, dwpot.GetXMAX() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );  
    // Exhist = new TH2F("Exthist"," ; x [um]; y [um]",dwpot.GetXMAX()*dwpot.GetBinSizex(), 0, dwpot.GetXMAX() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );  
    
    dhist = new TH2F("dhist"," ; x [um]; y [um]",1, 0, 1 , 1,0, 1 );  
    whist = new TH2F("whist"," ; x [um]; y [um]",1, 0, 1 , 1,0, 1 );  
    wfhist = new TH2F("wfhist"," ",1, 0, 1 , 1,0, 1 );  
    dfhist = new TH2F("dfhist"," ",1, 0, 1 , 1,0, 1 );  
    chist = new TH2F("chist"," ; x [um]; y [um]",1, 0, 1 , 1,0, 1 );  
    chhist = new TH2F("chhist"," ; x [um]; y [um]",1, 0, 1 , 1,0, 1 );  
    ctothist = new TH2F("ctothist"," ; x [um]; y [um]",1, 0, 1 , 1,0, 1 );  
    Exhist = new TH2F("Exthist"," ; x [um]; y [um]",1, 0, 1 , 1,0, 1 );  
    // set offset of axistitles

     
    //create other histograms
    jhist = new TH1F("jhist","CSA Jitter; Time [ns]; ",250,0,0.5);
    Enhist = new TH1F("Enhist","Entries; # of e-h pairs;",100,0,1000);
    BBjhist = new TH1F("BBjhist","BB Jitter; Time [ns]; ",250,0,0.5);  
    tvthhist = new TH1F("tvthhist"," CSA Time Resolution; Time [ns]; Entries  ",800,0,8.);  
    BBtvthhist = new TH1F("BBtvthhist"," BB Time Resolution; Time [ns]; Entries ",600,0,6.);  
    twtothist = new TH2F("twtothist","CSA Time Over Threshold; Time over Threshold [ns];  T_arrival;  ",250,0,20., 250,0,5.);  
    BBtwtothist = new TH2F("BBtwtothist","BB Time Over Threshold; Time over Threshold [ns]; T_arrival;  ",250,0,10., 250,0,3.);  
    //    MeasEnhist = new TH1F("MeasEnhist","Entries; Measured <#> of e-h pairs per micron;",(int) GetYMax()*75*2,0,GetYMax()*75*20);
    MeasEnhist = new TH1F("MeasEnhist","Entries; Measured <#> of e-h pairs per micron;",100,0,5000);      
    //develop histograms
    twtothist->SetMarkerColor(2);
    twtothist->SetMarkerSize(0.4);
    twtothist->SetMarkerStyle(2);
    BBtwtothist->SetMarkerColor(4);
    BBtwtothist->SetMarkerSize(0.4);
    BBtwtothist->SetMarkerStyle(2);
    
    // Initialize variables
    weightcut = new TGraph();	
    driftcut = new TGraph();
    dopdens = new TGraph();	
    wherecut = (dwpot.GetXMAX()*dwpot.GetBinSizex())/2+1;
    wherecut2 = (dwpot.GetXMAX()*dwpot.GetBinSizex())/2+1;	
    WhereCut->SetNumber(wherecut);
    WhereCut2->SetNumber(wherecut2);
    stopped=false;
    PotentialThread=0;
    CurrentsThread=0;
    plotupdate=false;
    LessPlot=false;
    Less2DPlot=false;
    bfieldon=false;
    diffusionon=false;
    gainon = false;
    OscOn = false;
    NA62On = false;
    SC_CSAOn = false;   
    FileNameOn = false;
    BatchOn = false;
    BatchRandomOn = false;
    ForceGain = false;
    ScreenedGain = false;
    PrecisionGain = false;    
    AC = false;
    CCEOn = false;
    AcceptorCreation = false;
    LnAcceptorCreation = false;
    GainQuenching = false;
    InitialDopRemoval = false;
    AllOn = false;
    DJOn = false;
    currentson=false;
    fieldyes=false;
    abscuryes=false;
    showcuryes=false;
    chargecloudon = false; 
    stripdoping=1;
    bulkdoping=0;
    Galpha = 330000;
    Gbeta = 1200000;
    BField = 0;

    //    SetGainDoping(valueMap["DOPING_GL"]); //call callsetdopingGL
    
    // SetGainKind(valueMap["KIND_GL"]); //call noone

    GainKind->Select(valueMap["KIND_GL"]); 
    GainDoping->Select(valueMap["DOPING_GL"]);
    GainShape->Select(valueMap["SHAPE_GL"]);
    //   SetGainShape(valueMap["SHAPE_GL"]);  //call setgaindoping and setgainkind
    /*
    Dopingentry->SetNumber(valueMap["DOP_LEV"]);
    Biasentry->SetNumber(valueMap["BIAS_VOLTAGE"]);
    Depletionentry->SetNumber(valueMap["DEPL_VOLTAGE"]);
    YMAXentry->SetNumber(valueMap["DETECT_HEIGHT"]);
    XMAXentry->SetNumber(valueMap["STRIP_NUMB"]); 
    Pitchentry->SetNumber(valueMap["STR_PITCH"]); 
    Widthentry->SetNumber(valueMap["STR_WIDTH"]);
    TempEntry->SetNumber(valueMap["TEMPERATURE"]); 
    */

    // initialize readout
    
    CallSetCSA(3);
    CallSetCSA(7);
    
    //changes to window
    MapSubwindows();
    SetWindowName("Weightfield2  Build 6.0");
    Resize(GetDefaultSize());
    MapWindow();
    CallSetPart(1);
    SetWF(0);

    if(app->Argc() == 2)
      {
	LoadData();
      }
    
    
    ///***************************************** END OF CANVAS AND HISTOGRAMS **********************************************///

}

///----------------------------------------------------------------------------------------------------------------------------------------------///


///----------------------------------------------------------\\ ACTION METHODS //------------------------------------------------------------------///

void WFGUI::CallCalculatePotentials() {

  //     	CalculatingLabel->SetBackgroundColor(0xff0000); // set progress label color to red
  //	CalculatingLabel->SetTitle("Calculating Potentials ..."); // update progess label title

	dhist->Reset();
	whist->Reset();
	twtothist->Reset();
	BBtwtothist->Reset();
	twtothist->SetStats(0);
	BBtwtothist->SetStats(0);
	tvthhist->Reset();
	BBtvthhist->Reset();
	jhist->Reset();
	BBjhist->Reset();
	SetDJValue(DJEntry->GetNumber());
	SetDJehValue(DJehEntry->GetNumber());
	SetFluence(IrradiationEntry->GetNumber());
	SetQuenchCoef(QuenchEntry->GetNumber());
	SetBetaElectrons(IrradiationEntry2->GetNumber());
	SetBetaHoles(IrradiationEntry3->GetNumber());

	//	cout << "lock7 " << __LINE__<< endl;
	
	//	cout << __LINE__<< endl;	
	dwpot.SetDoping(stripdoping,bulkdoping);
	//	CalculatingLabel->SetBackgroundColor(0xff0000); // set progress label color to red
	//	CalculatingLabel->SetTitle("Calculating Potentials ..."); // update progess label title
	//CalculatingLabel2->SetBackgroundColor(0xff0000); // set progress label color to red
	//CalculatingLabel2->SetTitle("Calculating Potentials ..."); // update progess label title


	//	cout << __LINE__<< endl;	
	if ( GainIndententry->GetNumber() > 0 &&( (Pitchentry->GetNumber()- Widthentry->GetNumber()-1) < GainIndententry->GetNumber() ))
	  {  GainIndententry->SetNumber( Pitchentry->GetNumber()- Widthentry->GetNumber()-2);
	    cout << "Gain recess too close to interstrip distance, moved to a shorter recess: " <<  GainIndententry->GetNumber() << " micron" << endl;
	  }

	  
	SetGainRecess(GainIndententry->GetNumber());
	//	cout << __LINE__<< endl;
	//disable buttons
	
	
	WhereCut->SetLimitValues(2, (dwpot.GetXMAX()*dwpot.GetBinSizex())-2.);	
	WhereCut2->SetLimitValues(2,(dwpot.GetXMAX()*dwpot.GetBinSizex())-2);	
	CarriersInNumberentry->SetLimitValues(0,(dwpot.GetXMAX()*dwpot.GetBinSizex()));	

	//	cout << __LINE__<< endl;
	cout << "VBias = " << GetVBias() << " VDepl = " << GetVDepl() << endl;

	dwpot.SetV(GetVBias(),GetVDepl());  				// set depeletion and bias voltage

	for(int i=0; i<dwpot.GetXMAX(); i++) {			// reset potentials to zero as a precaution
	  for(int j=0; j<dwpot.GetYMAX(); j++) {
	    dwpot.SetwPotential(j,i,0.0);
	    dwpot.SetdPotential(j,i,0.0);
	  }
	}		
	
	//	cout << __LINE__<< endl;
	dwpot.SetBoundaryConditions(ReadOutTopFlag);		// set boundary conditions
	dhist->GetXaxis()->SetLabelColor(0);	// set label color to white. because of multigrid approach, the tick labels of the axis are going to vary during calculation
	dhist->GetYaxis()->SetLabelColor(0);	

	//	dwpot.SetDopingProfile(this);

	CalculatingLabel->SetBackgroundColor(0xff0000); // set progress label color to red
	CalculatingLabel->SetTitle("Calculating potentials ..."); // update progess label title
	//	cout << __LINE__<< endl;
	dwpot.Multigrid(this, ReadOutTopFlag); // calculate potential

	// put here the check on the potential and SetVbias, Setgainregionvdepl
	
	//	cout << "Vmax = " << dwpot.GetdPotential(dwpot.GetYMAX()-1 ,dwpot.GetXMAX()-1)<< " Vbias = " << GetVBias() << endl;
	//	cout << " Gain layer region V depl = " << dpot[YMAX-1][XMAX/2] - GetVBias() << endl;
	
	//	cout << __LINE__<< endl;

	
	/////////////////////////////////////////

	//this works
	//	CalculatingLabel->SetBackgroundColor(0xff0000); // set progress label color to red
	// CalculatingLabel->SetTitle("Calculating E Field ..."); // update progess label title
	cout << "Now calculating the Efield " << endl;

	CallCalculateFields(); 			// calculate electric field

	//

	if(stopped==true) {
		CalculatingLabel->SetTitle("Calculation has been stopped.");// set progress label in case of "stop"
		//	CalculatingLabel2->SetTitle("Calculation has been stopped.");// set progress label in case of "stop"
	}
	//	else {
	//	CalculatingLabel->SetTitle("Done with potential");
		//	CalculatingLabel2->SetBackgroundColor(0x00ff00);	// when calculation completed, set progress label color to green
		// CalculatingLabel2->SetTitle("Done.");
	//	}



}	
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::CallCalculateCurrents() {	
	
  CalculatingLabel->SetBackgroundColor(0xff0000);				
  CalculatingLabel->SetTitle("Calculating Currents ...");	// update progress label

  B=0.0;
  Temp=0.0;
  float hity=0;
  
  jhist->Reset();
  BBjhist->Reset();
  tvthhist->Reset();
  BBtvthhist->Reset();

 
  
  //Setting variables from Input panels
  SetAlphaRange(min((int) RANGE_ALPHA, (int) ParticleSpecificsEntry->GetNumber()));//MODIFIEDRangeEntry->GetNumber());
  SetXRayRange(XRayRangeDef*1.);// To be fixed with the appropriate energy dependance
  SetPrecision(PrecisionEntry->GetNumber());
  //SetSampling(max(int(SamplingEntry->GetNumber()),300)); //below 100 the gain mechanism does not work
  //  SetSampling(SamplingEntry->GetNumber());
    SetSampling(SamplingEntry->GetNumber());
   if (GetFileNameOn()) SetPrintSampling(PrintEntry->GetNumber()); 
  SetFluence(IrradiationEntry->GetNumber());
  SetQuenchCoef(QuenchEntry->GetNumber());
  if (GetGainon())  SetYGain( (double) Gainentry->GetNumber()) ;
  SetBetaElectrons(IrradiationEntry2->GetNumber());
  SetBetaHoles(IrradiationEntry3->GetNumber());
  SetNumberP(NumberEntry->GetNumber());
  SetShNoise(ShNoiseEntry->GetNumber());
	
  SetBBNoise(BBNoiseEntry->GetNumber());
  SetBBVth(BBVthEntry->GetNumber());
  SetBBBW(BBBWEntry->GetNumber());
  SetBBGain(BBGainEntry->GetNumber()*1e3);//from kOhm to Ohm
  SetBBImp(BBImpEntry->GetNumber()); 
  SetCSAImp(CSAImpEntry->GetNumber());
  
  SetOscBW(OscBWEntry->GetNumber());
  SetCDet(CDEntry->GetNumber());
  SetLDet(LDEntry->GetNumber());
  SetCSAVth(CSAVthEntry->GetNumber());
  SetShTrans(ShTransEntry->GetNumber());

  //  SetAC(GetAC()); // force reading the AC value
  
	if(bfieldon==true) 
	  {
	    B = BfieldEntry->GetNumber();
	    SetBField(B);
	  }
	
	cout << " ========= Inputs to Simulation =================" << endl;	
	cout << "Time step in simulation = " << GetSampling()*1e12 << " ps " <<  endl;		
	cout << "Space mesh in simulation x = " << dwpot.GetBinSizex() << " micron " <<  endl;
	cout << "Space mesh in simulation y = " << dwpot.GetBinSizey() << " micron " <<  endl;		
	Temp = TempEntry->GetNumber();
	cout << "Temperature set to = "<< Temp << " Kelvin" << endl;
	cout << "Detector Type: "<< DetType << endl;
	//cout<<"Flag gain ON = "<<GetGainon()<<endl;
	//	cout << "Upper detector dimension = "<<dwpot.GetYMAX()<< " micron" << endl;
	int NBatch = 1.;
	int NBatchStart = 1.;

	dimMaxCarriers= 2*dwpot.Getmipcharge()+2000000;
	
	
	int xRandomHit; 
	float MaxTWCSA = 0;
	float MaxTWBB = 0;
	//float NpairsMax = 0;
	// double MaxPairs = 0;
	float MinTWCSA = 1000;
	float MinTWBB = 1000;
	//	float Npairs  = 0;

	osccanvas->Divide(1,2,0.002,0.002);
	eleIIcanvas->Divide(1,2,0.002,0.002);

	
	if (GetBatchOn() == true)
	{ 
	  //	      CurrentsProgressBar->SetMax(EventsEntry->GetNumber());
	  //   CurrentsProgressBar->Percent(0);
	      
	      NBatchStart = EventsEntryStart->GetNumber();
	      NBatch = EventsEntry->GetNumber()+EventsEntryStart->GetNumber();
	      GetEnhist()->Reset();	     	     
	      GetMeasEnhist()->Reset();
	}
	//	cout << "Batch = " << NBatch << endl;
	for (int e=NBatchStart; e<= NBatch; e++)
	  {
	    
	    carriers =new Carriers[dimMaxCarriers];
	    
	    cout << "Processing event " << e << endl;
	    //	    xRandomHit =  (( CarriersInNumberentry->GetNumber() -(int) (dwpot.Getpitch()/2) ) + (int) (dwpot.Getpitch()*gRandom->Rndm()));
	    xRandomHit =   GetXMax()*gRandom->Rndm();
	    //*dwpot.GetBinSizex(); //random hit x value for batch mode

	    for (int i=0;i<dimMaxCarriers;i++) carriers[i].Setinside(-1);	//initialize carriers array
	
	    if (GetBatchRandomOn() == true)
	      {
		carriersin = xRandomHit;
		hity = (dwpot.GetYMAX()*dwpot.GetBinSizey()-GetXRayRange()-ELECTRODE_DEPTH)*gRandom->Rndm()+GetXRayRange();
	      }
	    else 
	      {
		carriersin = CarriersInNumberentry->GetNumber();
		if (GetYMax() >EdgeNumberentry->GetNumber()) hity = EdgeNumberentry->GetNumber();
		else
		  {
		    //		      cout << "Y coordinate out of range, setting it to YMAX/2" << endl; 
		    hity = (dwpot.GetYMAX()/2)*dwpot.GetBinSizey();
		    EdgeNumberentry->SetNumber(hity);
		  }
	      }
	    
	    SetXEntry(carriersin); // x-position of hits
	    //cout << "Status " << radiobuttonstatus<< endl; 
	    switch (radiobuttonstatus){

	    case MIPunif:
	      SetParticleType(1);
	      SetCalibFlag(0);
	      SetConstQFlag(1);
	      SetUniformQFlag(1);
	      SetUserUniformQFlag(0);
	      dwpot.Setmipcharge(ParticleSpecificsEntry->GetNumber()*dwpot.GetYMAX()*dwpot.GetBinSizex()/cos(TMath::Pi()/180*GetAngle()));//MODIFIED
	      //	      dwpot.Setmipcharge(dwpot.GetYMAX()*75*dwpot.GetBinSizex()/cos(TMath::Pi()/180*GetAngle()));
	      CreateCharges(dwpot,carriers,carriersin,this);
	      break;
	      
	    case MIPnonunif:
	      SetParticleType(2);
	      SetCalibFlag(0);
	      SetConstQFlag(1);
	      SetUniformQFlag(0);
	      SetUserUniformQFlag(0);
	      dwpot.Setmipcharge(ParticleSpecificsEntry->GetNumber()*dwpot.GetYMAX()*dwpot.GetBinSizex()/cos(TMath::Pi()/180*GetAngle()));//MODIFIED
	      CreateCharges(dwpot,carriers,carriersin,this);
	      break; 
	      
	    case MIPlandau:
	      SetParticleType(3);
	      SetCalibFlag(0);
	      SetConstQFlag(0);
	      SetUniformQFlag(0);
	      SetUserUniformQFlag(0);
	      CreateCharges(dwpot,carriers,carriersin,this);
	      break;
	       
	    case ALPHA_TOP:
	      SetParticleType(4);
	      SetCalibFlag(0);
	      dwpot.Setmipcharge(ALPHAENERGY/GetEion()*GetAlphaRange()/RANGE_ALPHA);
	      CreateChargesAlphaTop(dwpot,carriers,carriersin,this);

	      break;
	      
	    case ALPHA_BOTTOM:
	      SetParticleType(5);
	      SetCalibFlag(0);
	      dwpot.Setmipcharge(ALPHAENERGY/GetEion()*GetAlphaRange()/RANGE_ALPHA);
	      CreateChargesAlphaBottom(dwpot,carriers,carriersin,this);
	      break;
	      
	    case USR_CHARGE: // laser Top
	      SetParticleType(6);
	      SetConstQFlag(1);
	      SetUniformQFlag(1);
	      SetUserUniformQFlag(1);
	      SetCalibFlag(0);
	      dwpot.Setmipcharge(ParticleSpecificsEntry->GetNumber()*dwpot.GetYMAX()*dwpot.GetBinSizex()/cos(TMath::Pi()/180*GetAngle()));//MODIFIED
	      CreateCharges(dwpot,carriers,carriersin,this);

	      break;

	    case CALIB:
	      SetParticleType(7);
	      SetCalibFlag(1);
	      SetConstQFlag(0);
	      SetUniformQFlag(0);
	      SetUserUniformQFlag(0);
	      break;
	      
	    case EDGE:
	      SetParticleType(8);
	      SetCalibFlag(0);
	      SetConstQFlag(0);
	      SetUniformQFlag(0);
	      SetUserUniformQFlag(0);
	      dwpot.Setmipcharge(dwpot.GetXMAX()*LaserPairsPerMicron*dwpot.GetBinSizex());
	      if (GetBatchRandomOn() == true)
		{
		hity = ELECTRODE_DEPTH+(dwpot.GetYMAX()*dwpot.GetBinSizey()-2*ELECTRODE_DEPTH)*gRandom->Rndm();
		cout << "Hit y position = " << hity << endl;
		}
	      CreateChargesLaserSide(dwpot,carriers,hity,this);
	      
	      break;

	    case XRAY:
	      SetParticleType(9);
	      SetCalibFlag(0);
	      SetConstQFlag(0);
	      SetUniformQFlag(0);
	      SetUserUniformQFlag(0);
	      // dwpot.Setmipcharge(ParticleSpecificsEntry->GetNumber()*1e3*dwpot.GetBinSizey()/IONENERGY);//MODIFIED
	      dwpot.Setmipcharge(ParticleSpecificsEntry->GetNumber()*1e3/GetEion());//MODIFIED
	      CreateChargesXRay(dwpot,carriers, carriersin,hity,this);
	      break;

	    case TRENCH:
	      SetParticleType(3);
	      SetCalibFlag(0);
	      SetConstQFlag(0);
	      SetUniformQFlag(0);
	      SetUserUniformQFlag(0);
	      ParticleSpecificsEntry->SetNumber(70);
	      if (GetBatchRandomOn() == true)
		{
		  hity = ELECTRODE_DEPTH+(dwpot.GetYMAX()*dwpot.GetBinSizey()-2*ELECTRODE_DEPTH)*gRandom->Rndm();
		  cout << "Hit y position = " << hity << endl;
		}
	      CreateChargesMipSide(dwpot,carriers,hity,this);
	      break;

	      
	      

	      
	    default: break;
	    }

	    if (GetParticleType() != 7) cout << "Particle hits at " << carriersin << " micron on the x axis" << endl;
	    
	    
	    SetLorentz(dwpot,carriers,B, Temp, CallGetDetType(), GetFluence());
	    
	    if (e%100 == 0)
	      {
		cout<<"Event Number: "<<e <<endl;
	      }
	    //	    cout << " Wfield x at y = 3000 x = 200, " << wf[3000][2000].GetFieldx() << endl;
	    CalculateCurrents(dwpot,df,wf,carriers,this,e);
	    delete[] carriers;
	    
	    /*	    if(GetFileNameOn())
		    {
		    std::stringstream ss;
		    ss << " Event  #  "<< e << " " ;
		    CalculatingLabel->SetTitle(ss.str().c_str()); // update progess label title	
		    continue; //continue  loop if writing out 
		    } */
	    if(Getstopped()) 
	      {
		Setstopped(false);
		break;
	      }
	    if (GetBatchOn() == true) 
	      {

		std::stringstream ss;
		ss << " Event  #  "<< e << " " ;
		CalculatingLabel->SetTitle(ss.str().c_str()); // update progess label title	
		if (GetOscOn())
		  {

		    if (e == 1)
		      {
			osccanvas->Clear();
			osccanvas->Divide(1,2,0.002,0.002);
			eleIIcanvas->Clear();
			eleIIcanvas->Divide(1,2,0.002,0.002);
		      }
		    
		    jhist->Fill(GetJitter());
		    tvthhist->Fill(GetCSATVth());
		    BBtvthhist->Fill(GetBBTVth());	     
		    twtothist->Fill(GetCSAFTVth()-GetCSATVth(),GetCSATVth());
		    if ( fabs(GetCSAFTVth()-GetCSATVth()) > MaxTWCSA)  MaxTWCSA = fabs(GetCSAFTVth()-GetCSATVth());
		    if ( fabs(GetCSAFTVth()-GetCSATVth()) < MinTWCSA)  MinTWCSA = fabs(GetCSAFTVth()-GetCSATVth());
		    // twtothist->GetXaxis()->SetRangeUser(MinTWCSA*0.9, MaxTWCSA*1.1);
		    
		    
		    BBtwtothist->Fill(GetBBFTVth()-GetBBTVth(),GetBBTVth());
		    if (fabs(GetBBFTVth()-GetBBTVth()) > MaxTWBB)  MaxTWBB = fabs(GetCSAFTVth()-GetCSATVth());
		    if (fabs(GetBBFTVth()-GetBBTVth()) < MinTWBB)  MinTWBB = fabs(GetCSAFTVth()-GetCSATVth());
		    // BBtwtothist->GetXaxis()->SetRangeUser(0.9*MinTWBB, MaxTWBB*1.1);
		    // cout << "histo limits = "<<MinTWBB << " " << MaxTWBB << endl;
		    //		osccanvas->Clear();
		    
		    osccanvas->cd(1);
		    SetHStyle(tvthhist, 0.05);
		    tvthhist->Draw();
		    if (e%10 == 0)  	tvthhist->Fit("gaus");
		    gStyle->SetOptFit(1);
		    osccanvas->cd(2);
		    //SetHStyle(BBtvthhist, 0.05);
		    //		    BBtvthhist->Draw();
		    twtothist->Draw();
		    //		    if (e%10 == 0) BBtvthhist->Fit("gaus");
		    gStyle->SetOptFit(1);
		    osccanvas->Update();


		    eleIIcanvas->cd(1);
		    SetHStyle(BBtvthhist, 0.05);
		    BBtvthhist->Draw();
		    if (e%10 == 0)  	BBtvthhist->Fit("gaus");
		    gStyle->SetOptFit(1);
		    eleIIcanvas->cd(2);
		    //  SetHStyle(BBtwtothist, 0.05);
		    //		    BBtvthhist->Draw();
		    BBtwtothist->Draw();
		    //		    if (e%10 == 0) BBtvthhist->Fit("gaus");
		    //		    gStyle->SetOptFit(1);
		    eleIIcanvas->Update();

		    
		  }
		//		    if (!GetShowCur())
		//		cout << __LINE__<< endl;	
		Getcanvaspc()->Clear();
		if (GetParticleType() <4 || GetParticleType() == 6 ||  GetParticleType() == 8)
		  {
		    Getcanvaspc()->Divide(1,2,0.004,0.004);   
		    Getcanvaspc()->cd(1);
		    // Npairs = GetNumPairs();
		    
		    // cout << "Npairs = " << GetNumPairs() << " " << NpairsMax << endl;
		    if (e == 1)
		      {
			//	MaxPairs = 0;
			//			    GetEnhist()->Reset();			
			if (NBatch >1)    SetHStyle(GetEnhist(), 0.08);
			else    SetHStyle(GetEnhist(), 0.04);
			GetEnhist()->GetXaxis()->SetRangeUser(0.,  500.);			
			GetEnhist()->SetTitle("Generated average # of e-h per micron");
			GetEnhist()->GetXaxis()->SetTitle("<#> of e-h per micron");
			GetEnhist()->GetYaxis()->SetTitle("Events");
			
			
			
			
			SetHStyle(GetMeasEnhist(), 0.08);
			GetMeasEnhist()->SetTitle("Measured average  # of e-h per micron");
			GetMeasEnhist()->GetXaxis()->SetTitle("<#> of e-h per micron");
			GetMeasEnhist()->GetYaxis()->SetTitle("Events");
			GetMeasEnhist()->GetXaxis()->SetRangeUser(0.,  5000.);			
			if (!GetGainon()) 	GetMeasEnhist()->GetXaxis()->SetRangeUser(0.,  500.);	
			gStyle->SetOptFit(1);
			FitLandau = new TF1("FitLandau","landau",20.,1000.);
			FitLandau1 = new TF1("FitLandau1","landau",20.,5000.);
		      }	
			
		    Getcanvaspc()->cd(1);
		    Getcanvaspc()->cd(1)->SetTopMargin(.1);
		    Getcanvaspc()->cd(1)->SetBottomMargin(.2);
		    GetEnhist()->Draw();		    
		    if (e>20 && e%2 == 0 && GetParticleType()==3)  //	Enhist->Fit("landau","","SAME",0,400);
		      {
			gStyle->SetOptFit(1); 
			FitLandau->SetParameter(1,65.);
			FitLandau->SetParameter(2,50.);
			FitLandau->SetParLimits(1,30.,200.);
			FitLandau->SetParLimits(2,10.,100.);			    
			Enhist->Fit(FitLandau,"","SAME");
		      }
		    Getcanvaspc()->cd(2);
		    Getcanvaspc()->cd(2)->SetTopMargin(.1);
		    Getcanvaspc()->cd(2)->SetBottomMargin(.2);	       
		    GetMeasEnhist()->Draw();
		    if (e>20 && e%2 == 0 && GetParticleType()==3)  //	Enhist->Fit("landau","","SAME",0,400);
		      {
			gStyle->SetOptFit(1); 
			FitLandau1->SetParameter(1,1000.);
			FitLandau1->SetParameter(2,500.);
			FitLandau1->SetParLimits(1,100.,2000.);
			FitLandau1->SetParLimits(2,100.,1000.);			    
			MeasEnhist->Fit(FitLandau1,"","SAME");
		      }		    

		    
		  }
	      
		Getcanvaspc()->Update();
		Getcurcanvas()->Update();
		
	      }
	    
	    if (GetBatchOn() == false) break; // exit from the batch loop if you de-select the check box
	  } // end of batch loop

	if (GetBatchOn() == true)
	  {
	    cout<<"Run finished: "<< EventsEntry->GetNumber() <<" events"<<endl;
	

	    CalculatingLabel->SetBackgroundColor(0x00ff00);
	    CalculatingLabel->SetTitle("Done running in batch.");
	  }
	    //CalculatingLabel2->SetBackgroundColor(0x00ff00);
	//CalculatingLabel2->SetTitle("Done.");
	if (GetBatchOn() == false)
	  {
	    curcanvas->Update();
	    osccanvas->Update();
	  }
	CurrentsInfoFrame->Layout();
	QELabel->Layout();
}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::CallCalculateFields() {

  df = new Field*[(dwpot.GetYMAX())];
  for (int i = 0; i < (dwpot.GetYMAX()); i++) df[i] = new Field[(dwpot.GetXMAX())];
  
  wf = new Field*[(dwpot.GetYMAX())];
  for (int i = 0; i < (dwpot.GetYMAX()); i++) wf[i] = new Field[(dwpot.GetXMAX())];	
  CalculateFields(dwpot,wf,df);
  CalculateAbsFields(dwpot,wf,df);
  
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetPlotFieldHist(Bool_t onoff) {
	fieldyes=onoff;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetAbsCur(Bool_t onoff) {
	abscuryes=onoff;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetShowCur(Bool_t onoff) {
	showcuryes=onoff;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetShowCur() {
	return showcuryes;
}
/////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetB() {
	return B;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetQLabel(const char* qetext,const char* qhtext,const char* qehtext,const char* qegtext,const char* qhgtext,const char* qehgtext,const char* qetottext,const char* qhtottext,const char* qtottext) {
	QELabel->SetTitle(qetext);
	QHLabel->SetTitle(qhtext);
	QEHLabel->SetTitle(qehtext);
	QEGLabel->SetTitle(qegtext);
	QHGLabel->SetTitle(qhgtext);
	QEHGLabel->SetTitle(qehgtext);
	QETotLabel->SetTitle(qetottext);
	QHTotLabel->SetTitle(qhtottext);
	QTotLabel->SetTitle(qtottext);
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetAngleLabel(const char* ethetatext, const char* hthetatext) {
	LorentzeLabel->SetTitle(ethetatext);
	LorentzhLabel->SetTitle(hthetatext);
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetLTLabel(const char* etext, const char* htext) {
	LTeLabel->SetTitle(etext);
	LThLabel->SetTitle(htext);
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::DrawFieldsAbs(int LCol = 1) {

	double* q1;
	float Tot_gain = 0;
	float Tot_gain_h = 0;
	wherecut = WhereCut->GetNumber();


	int Npos = 0;
	int NBins = dwpot.GetYMAX();
	q1 = new double[NBins];

	
	double *bdfield; // Absolute value of breakdown field
	double *dabs; // Absolute value of drift field
	double *wabs; // absolute value of weighting field
	double *wxabs; // absolute value of weighting field
	double *gain_h; // gain for holes

	bdfield = new double[ NBins];
	dabs = new double[ NBins];
	wabs = new double[ NBins];		 				
	gain = new double[ NBins];
	gain_h = new double[ NBins];		
	for(int i=0; i< NBins-1; i++)
	  {
	    wabs[i]=0.0;
	    dabs[i]=0.0;
	    gain[i]=1.0;
	    gain_h[i]=1.0;
	    bdfield[i]=0.;
	  }		
	
	double Maxdabs = 0;
	double Mindabs = 100;
	double Maxgain = 0;
	double Maxwabs = 0;
	double Maxwxabs = 0;
	double MaxNoGainE = 0;
	double LocalGain = 0;
	double LocalGain_h = 0;
	double WGain = 0;
	int wherecutbin = wherecut/dwpot.GetBinSizex();
	

	Npos = 0;
	for(int k=0;k< NBins-1;k++) 
	  {
	    Npos = k;
	    // breakdown field
	    //	    if (k==0)     bdfield[k]= 1e-3*4e5/(1.-1./3.*log10( (fabs(dwpot.Getdopyx(1,wherecutbin))*(EPSILON*EPSILONR)/ECHARGE*1e-6)/1e16));//breakdown voltage kV/cm
	    // else bdfield[k] = 1e-3*4e5/(1.-1./3.*log10( (fabs(dwpot.Getdopyx(Npos,wherecutbin))*(EPSILON*EPSILONR)/ECHARGE*1e-6)/1e16));//breakdown voltage kV/cm

	    if (k==0)     bdfield[k]= 1e-3*4e5/(1.-1./3.*log10(fabs(dwpot.Getdopyx(1,wherecutbin))*1e-6/1e16));//breakdown voltage kV/cm
	    else bdfield[k] = 1e-3*4e5/(1.-1./3.*log10( fabs(dwpot.Getdopyx(Npos,wherecutbin))*1e-6/1e16));//breakdown voltage kV/cm
									    
	    if ( bdfield[k] <0 )  bdfield[k] = 400; // for high doping, not relevant
	    
	    q1[k]=k*dwpot.GetBinSizey(); //(int)(pow(2,pot.Getref()));
	    wabs[k] = 1e-3*wf[k][wherecutbin].Getabs();
	    dabs[k] = df[k][wherecutbin].Getabs()/1e5; // V/m ==> kV/cm


	    LocalGain = 1.;
	    LocalGain_h = 1.;
	   
	    WGain = (GetGainvalue(df[Npos][wherecutbin].Getabs(), -1, GetGainKind())+GetGainvalue(df[Npos+1][wherecutbin].Getabs(), -1, GetGainKind()))/2.;
	    LocalGain   =  exp(WGain*dwpot.GetBinSizey()*1e-6);
	    WGain = (GetGainvalue(df[Npos][wherecutbin].Getabs(), 1, GetGainKind())+GetGainvalue(df[Npos+1][wherecutbin].Getabs(), 1, GetGainKind()))/2.;
	    LocalGain_h =  exp(WGain*dwpot.GetBinSizey()*1e-6);
	    //	    std::cout << "posiz2=" << Npos*dwpot.GetBinSizey() << " " << df[Npos+1][wherecutbin].Getabs() << std::endl;
	    

	    if (LocalGain> 100)
	      {
		cout << " Problem in computing Gain for electrons at  position = " << Npos*dwpot.GetBinSizey() << " micron, LocalGain set to 1 " << endl;
		LocalGain = 1.;
	      }
	    
	    if (LocalGain_h> 100)
	      {
		cout << " Problem in computing Gain for holes at  position = " << Npos*dwpot.GetBinSizey() << " micron, LocalGain_h set to 1 " << endl;
		LocalGain_h = 1.;
	      }
	    
	    if (k == 0)
	      {
		gain[k] = 1;
		gain_h[k] = 1;
	      }
	    else
	      {
		gain[k] = (LocalGain)*gain[k-1];
		gain_h[k] = (LocalGain_h)*gain_h[k-1];
		//     	cout << "Npos = " << Npos << setprecision(3)<< " pos = " <<Npos*dwpot.GetBinSizey()*1e-6 << " Localgain = " << LocalGain << " " << gain[k] << endl;
	      }
	    if( gain[k] >1.01 && Getygainlow() == 0)
	      {
		Setygainlow((Npos-1)*dwpot.GetBinSizey());
		// cout << " Set gain low " << (Npos-1) << " " << dwpot.GetBinSizey() << endl;;
	      }
	    
	    if( gain[k] > 1.01 && k > Getygainhigh() && gain[k]>Tot_gain)
	      {
		Setygainhigh((Npos+1)*dwpot.GetBinSizey());
		//		cout << " Set gain high " << (Npos-1) << " " << dwpot.GetBinSizey() << endl;;
	      }
	    Tot_gain = gain[k];
	    
	    Tot_gain_h = gain_h[k];
	    
	    if(q1[k]<Getygainlayerlow()) MaxNoGainE = dabs[k];
	    if (Maxwabs < wabs[k]) Maxwabs = wabs[k];
	    if (Maxdabs < dabs[k]) Maxdabs = dabs[k];
	    if (Maxgain < gain[k]) Maxgain = gain[k];
	    if (dabs[k]<Mindabs) Mindabs = dabs[k];

	    //	    Npos +=VSteps[k];
	    
	    //  cout << k << endl;
	  }
	//	cout << "Outoftheloops" << endl;

	if (Mindabs == 0) cout << " Drift Field Amplitude = 0 !! Decrease DJ Current or Increase Bias" << endl;
	cout << "Total field = "<< Maxdabs << " Fraction due to Bias = " << MaxNoGainE/Maxdabs << endl; 
	cout << "Field due to Bias: "<< MaxNoGainE << " kV/cm. Field due to gain layer: " << Maxdabs - MaxNoGainE << " kV/cm. Fraction of field due to doping: " << 1.-MaxNoGainE/Maxdabs << endl;
	bdfieldcut = new TGraph( NBins-1,q1,bdfield);
	bdfieldcut->SetLineStyle(2);
	bdfieldcut->SetLineColor(2);
	
	dfieldcut = new TGraph( NBins-1,q1,dabs);
   	dfieldcut->GetXaxis()->SetTitle("y [um]");
	dfieldcut->GetYaxis()->SetTitle("|E| [kV/cm]");
   	dfieldcut->GetXaxis()->SetNdivisions(5);
	dfieldcut->SetTitle("E Amplitude |E| [kV/cm] - Dashed: breakdown field ");
   	dfieldcut->GetYaxis()->SetRangeUser(0.0,Maxdabs*1.3);
   	dfieldcut->SetLineWidth(3);

 
	canvaspc->cd(2);
	dfieldcut->SetLineColor(LCol); // set line color to black
	if (fabs(GetDopinggainlayerValue())>0) 
	  {
	    gPad->SetLogy(1);
	    dfieldcut->GetYaxis()->SetRangeUser(Mindabs*0.5,Maxdabs*2);
	  }
	dfieldcut->Draw();
	bdfieldcut->Draw("SAME");
	//	canvaspc->Update();

	std::stringstream ssg;
	ssg << " Max E =  "<< setprecision(3) << Maxdabs  << " [kV/cm]" ;
	TLatex lng;
	lng.DrawLatex( 0.5,Maxdabs,ssg.str().c_str());

	

	std::stringstream ssg1;
	ssg1 << "Bulk E =  " << setprecision(3) << MaxNoGainE << " [kV/cm]" ;
	TLatex lng1;
	lng1.DrawLatex( 0.5,Maxdabs*0.2,ssg1.str().c_str());



	double gexp =  -0.382*log(Maxdabs*1000)+5.58 ; // this parameter makes the growth less violent with E: higher at low field, lower at high field
	double ga = -0.577*log(Maxdabs*1000)+7.8; // this parameter makes the growth less violent with E: higher at low field, lower at high field
	//	double ga = 1;
	// double gexp = 1;
	double GC = 966000; // V*cm^-1 default: 966000 
	double GD = 499; // V*cm^1K^1 
	double Galpha = 443000; //cm^-1
	double CoefFluence = 0; 
	if (GetGainQuenching()) CoefFluence = Quench;
	double Gbeta = GC+GD*TempEntry->GetNumber()+CoefFluence*GetFluence(); // V/cm critical field
	double gai = ga*Galpha*exp(-(gexp*Gbeta)/(Maxdabs*1000))*0.0001; // gain micron
	cout << "Electron gain length = " << setprecision(3) << 1./gai << " [um] at E field: " << Maxdabs << " [kV/cm]" <<  endl;
	//	cout << "Electron gain length = " << setprecision(3) << ga << " [um] at E field: " << gexp << " [kV/cm]" <<  endl;
	

	
	std::stringstream ssg2;
	ssg2 << "El. gain length =  "<< setprecision(3) << 1./gai  << " [um]" ;
	TLatex lng2;
	lng2.DrawLatex( 0.5,Maxdabs*0.5,ssg2.str().c_str());
	
	canvaspc->cd(4);

	dgaincut_h = new TGraph( NBins-1,q1,gain_h);
	dgaincut = new TGraph( NBins-1,q1,gain);
   	dgaincut->GetXaxis()->SetTitle("y [um]");
	dgaincut->GetYaxis()->SetTitle("Gain");
   	dgaincut->GetXaxis()->SetNdivisions(5);
	dgaincut->SetTitle("Incremental gain  as a function of y");
   	dgaincut->GetYaxis()->SetRangeUser(0.0,Maxgain*1.3);
   	dgaincut->SetLineWidth(3);
	//

	
	//	cout << "Gain = " << Tot_gain << endl;
	if (Tot_gain >1.05) SetGainon(true);
	if(GetBatchOn()==false)
	  {
	    
	    if (GetGainon()==true )
	      {
		cout<<"Detector with internal gain. Gain region extends from y = "<< Getygainlow() << " to "<<  Getygainhigh() << " micron" <<endl;
		//   cout << "Ratio h/e gain = " <<  gui->GetGainRatio() << endl;
		
	      }
	    else
	      {
		cout<<"Detector without internal gain."<<endl;

	      }
	    
	  }
    	
	dgaincut->SetLineColor(LCol); // set line color to black
	dgaincut->Draw();

	dgaincut_h->SetLineWidth(2);
	dgaincut_h->SetLineStyle(2);
	dgaincut_h->Draw("SAME");
	
	std::stringstream ssp;
	ssp << " Gain_electron =  " << Tot_gain ;
	TLatex ln;
	ln.DrawLatex( 0.5,Maxgain,ssp.str().c_str());

	std::stringstream ssh;
	ssh << " Gain_holes =  " << Tot_gain_h ;
	TLatex lnh;
	lnh.DrawLatex( 0.5,0.5*Maxgain,ssh.str().c_str());


	double* qx1;
	int NxBins = dwpot.GetXMAX();
	NBins = dwpot.GetYMAX();
	qx1 = new double[NxBins];
	wxabs = new double[ NxBins];
	for(int k=0; k< NxBins-1; k++)
	  {
	    qx1[k]=k*dwpot.GetBinSizex(); //(int)(pow(2,pot.Getref()));
	    //	    wxabs[k] = 1e-3*wf[(int) 0.8*NBins][k].Getabs();
	    //	    wxabs[k] =   wf[(int) 0.99*NBins][k].GetFieldx();
	    //	    cout << "biny = " << 0.99*NBins <<  " binx = " << k << " Field = " <<  wf[(int) 0.99*NBins][k].GetFieldx() << endl;
	    wxabs[k] =    1e-3*wf[int (0.8*NBins)][k].GetFieldx();
	    //	    cout << "biny = " <<  pippo <<  " binx = " << k << " Field = " <<  wf[pippo][k].GetFieldx() << " 0.9bins " <<   int (0.85*NBins) << " Field: " << wf[int (0.85*NBins)][k].GetFieldx() <<endl;
	    if (Maxwxabs < wxabs[k]) Maxwxabs = wxabs[k];
	  }
	
	//	wfieldcut = new TGraph( NBins-1,q1,wabs);
       	wfieldcut = new TGraph( NxBins-1,qx1,wxabs);
   	wfieldcut->GetXaxis()->SetTitle("x [um]");
	wfieldcut->GetYaxis()->SetTitle("[10^3/m]");
   	wfieldcut->GetXaxis()->SetNdivisions(4);
	wfieldcut->SetTitle("WF along x @ 0.8 ymax");
	wfieldcut->GetYaxis()->SetTitleOffset(1.5);
	//   	wfieldcut->GetYaxis()->SetRangeUser(0.0,Maxwabs*1.3);
	wfieldcut->GetYaxis()->SetRangeUser(-Maxwxabs*1.3,Maxwxabs*1.3);
   	wfieldcut->SetLineWidth(3);
	canvaswc->cd(2);
	canvaswc->cd(2)->SetLeftMargin(.15);
	canvaswc->cd(2)->SetRightMargin(.15);	
	wfieldcut->SetLineColor(LCol); // set line color to black
	wfieldcut->Draw("AL");
	//	wfieldcut->GetYaxis()->SetLimits(0., Maxwabs*1.2);
	wfieldcut->GetYaxis()->SetLimits(0., Maxwxabs*1.2);
	//	canvaswc->Update();

	canvasw->cd();
	//	wfieldcut->Draw("SAME");
	canvasw->Update();				
	delete[] q1;
	delete[] dabs;
	delete[] wabs;
	delete[] qx1;
	delete[] wxabs;
	
	
}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::DrawFieldsAbsX() {
  // return;
	wherecut = WhereCut->GetNumber();
	wherecut2 = WhereCut2->GetNumber();
	//	int Step = pow(1./GetStepy(), 0.5);

	
	//	int NBins = 0;

	int NBins = dwpot.GetYMAX();

	double* q2;
	q2 = new double[NBins];

	
	double *dxabs; // Absolute value of drift field
	dxabs = new double[NBins];	
	double *wxabs; // Absolute value of weighting field
	wxabs = new double[NBins];	

	for(int i=0; i<NBins; i++) {
		wxabs[i]=0.0;
		dxabs[i]=0.0;
	}			

	int wherecutbin = wherecut/dwpot.GetBinSizex();
	//	Npos = 0;
	for(int k=0;k<NBins;k++)
	  {
	    q2[k]=k*dwpot.GetBinSizey(); //(int)(pow(2,pot.Getref()));
	    dxabs[k] = df[k][wherecutbin].GetFieldx()/1e5 ;
	    wxabs[k] = ( fabs(wf[k][wherecutbin].GetFieldx())  > 0.1) ?  1e-3*fabs(wf[k][wherecutbin].GetFieldx()) : 0;
	    //   Npos +=VSteps[k];
	  }

	

	dfieldxcut = new TGraph(NBins,q2,dxabs);
   	dfieldxcut->GetXaxis()->SetTitle("y [um]");
   	dfieldxcut->GetXaxis()->SetNdivisions(5);
	dfieldxcut->SetTitle("Drift Field Ex [kV/cm] ");

   	dfieldxcut->GetYaxis()->SetLabelSize(0.05);
   	dfieldxcut->GetYaxis()->SetLabelOffset(.01);
   	dfieldxcut->GetXaxis()->SetLabelSize(0.05);
   	dfieldxcut->GetXaxis()->SetLabelOffset(.01);
   	dfieldxcut->GetXaxis()->SetTitleSize(0.05);
   	dfieldxcut->SetLineWidth(3);

	//	canvaspf->Divide(1,1,0.002,0.002);
	//	canvaspf->cd();
	//canvaspf->Clear()
	// canvaspf->Update();

	canvaspc->cd(2);
	gPad->SetLogy(0);
	dfieldxcut->Draw("AL");
	canvaspc->Update();

	wfieldxcut = new TGraph(NBins,q2,wxabs);
   	wfieldxcut->GetXaxis()->SetTitle("y [um]");
   	wfieldxcut->GetXaxis()->SetNdivisions(5);
	wfieldxcut->SetTitle("Weighting Field Ex [10^3/m] ");
   	wfieldxcut->GetYaxis()->SetLabelSize(0.05);
   	wfieldxcut->GetYaxis()->SetLabelOffset(.01);
   	wfieldxcut->GetXaxis()->SetLabelSize(0.05);
   	wfieldxcut->GetXaxis()->SetLabelOffset(.01);
   	wfieldxcut->GetXaxis()->SetTitleSize(0.05);
   	wfieldxcut->SetLineWidth(3);

	canvaswc->cd(2);

	wfieldxcut->Draw("AL");
	canvaswc->Update();
	delete[] q2;
	delete[] dxabs;
	delete[] wxabs;
	
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::DrawFieldsAbsY() {
  // return;
	wherecut = WhereCut->GetNumber();
	wherecut2 = WhereCut2->GetNumber();

	double MaxEy = 0;
	double MinEy = 0;



	
	int NBins = dwpot.GetYMAX();
	double* q3;
       	q3 = new double[NBins];

	double *dyabs; // Absolute value of drift field in y direction
	dyabs = new double[NBins];					
	double *wyabs; // Absolute value of weighting field in x direction
	wyabs = new double[NBins];	

	for(int i=0; i<NBins; i++) { // initialize
		wyabs[i]=0.0;
		dyabs[i]=0.0;
	}
	int wherecutbin = wherecut/dwpot.GetBinSizex();

	for(int k=0;k<NBins;k++)
	  {
	    q3[k]=k*dwpot.GetBinSizey(); //(int)(pow(2,pot.Getref()));
	    dyabs[k] = df[k][wherecutbin].GetFieldy()/1e5 ;
	    if (dyabs[k] >MaxEy) MaxEy = dyabs[k];
	    if (dyabs[k] <MinEy) MinEy = dyabs[k];
	    wyabs[k] = ( fabs(wf[k][wherecutbin].GetFieldy())  > 0.1) ?  1e-3*fabs(wf[k][wherecutbin].GetFieldy()) : 0;

	  }

	

	dfieldycut = new TGraph(NBins,q3,dyabs); 
   	dfieldycut->GetXaxis()->SetTitle("y [um]");
   	dfieldycut->GetXaxis()->SetNdivisions(5);
	dfieldycut->SetTitle("Drift Field Ey [kV/cm] ");

   	dfieldycut->GetYaxis()->SetLabelSize(0.05);
   	dfieldycut->GetYaxis()->SetLabelOffset(.01);
   	dfieldycut->GetXaxis()->SetLabelSize(0.05);
   	dfieldycut->GetXaxis()->SetLabelOffset(.01);
   	dfieldycut->GetXaxis()->SetTitleSize(0.05);
   	dfieldycut->SetLineWidth(3);

	canvaspc->cd(2);
	gPad->SetLogy(0);
	dfieldycut->Draw("AL");
	canvaspc->Update();

       
	wfieldycut = new TGraph(NBins,q3,wyabs);
   	wfieldycut->GetXaxis()->SetTitle("y [um]");
   	wfieldycut->GetYaxis()->SetTitle("[10^3/m]");
   	wfieldycut->GetXaxis()->SetNdivisions(5);
	wfieldycut->SetTitle("Weighting Field Ey");
   	wfieldycut->GetYaxis()->SetLabelSize(0.05);
   	wfieldycut->GetYaxis()->SetLabelOffset(.01);
   	wfieldycut->GetXaxis()->SetLabelSize(0.05);
   	wfieldycut->GetXaxis()->SetLabelOffset(.01);
   	wfieldycut->GetXaxis()->SetTitleSize(0.05);
   	wfieldycut->SetLineWidth(3);
	//	canvaswf->Divide(1,1);
	//	canvaswf->cd();
	// canvaswf->Clear();  //Nicolo1
	//	wfieldycut->Draw("ACP");
	canvaswc->cd(2);
	wfieldycut->Draw("AL");
	canvaswc->Update();

	if (MaxEy*MinEy <0) cout << " Drift Field positive and negative!! Decrease Current or Increase Bias" << endl;

	delete[] q3;
	delete[] dyabs;
	delete[] wyabs;
	return;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::DrawFieldHist() {
  if (GetLess2DPlot()) return;
  
	int p=1;
	int a=0;
	int h=0;
	int binsx=dwpot.GetXMAX(); // binsx = number of bins on x-axis
	int binsy=dwpot.GetYMAX(); // binsy = number of bins on y-axis
	dfhist->Reset();	 //dfhist = histogram of drift field 

	
	for(int i=0;i<(int) binsx;i++) {
	  h=  i;
	  // cout << h << " : " ;
	  for(int j=0;j< (int) binsy;j++) 
	    {
	      a= j;
	      dfhist->SetBinContent(i,j,df[a][h].Getabs()/1e5);	    	
	    }
	  
	}
	
	dfhist->SetStats(0);	
	canvasp->cd();
	dwpot.DriftPal();
	dfhist->Draw("SAMES" "CONT2"); //draw on same canvas as dhist and use arrows
	//	dfhist->Draw("SAMES" "ARR"); //draw on same canvas as dhist and use arrows	
	canvasp->Update();
	return;

	//whist->Draw("COLZ");
	wfhist->Reset();	//wfhist = histogram of weighting field
	wfhist->TH2F::SetBins((binsx/p+1), 0,(binsx/p+1) ,(binsy/p+1),0.,(binsy/p+1) );
	//	canvasw->Divide(1,1,0.002,0.002);
	canvasw->cd();
					
	for(int i=0;i<binsx/p+1;i++) {
		for(int j=0;j<binsy/p+1;j++) {
			h=i*p;
			a=j*p;
			wfhist->SetBinContent(i+1,j+1,dwpot.Getwpot(a,h));
			//	wfhist->SetBinContent(i+1,j+1,df[i][j].GetFieldx());
		}
	}
	wfhist->SetStats(0);
	dwpot.WeightPal();
	wfhist->Draw("SAMES" "ARR");	
	canvasw->Update();				
	// return;


}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::Getdiffusionon() {
	return diffusionon;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::Getabscuron() {
	return abscuryes;
}

/////////////////////////////////////////////////////////////////////////////////////////////
int WFGUI::SetStopOn(){
  ThreadstopCurrents();
  ThreadstopPotential();
  
  if (stopped==false) {  
    stopped=true;
    return 0;
  }
      
     else return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::Getstopped() {
	return stopped;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::Setstopped(Bool_t x) {
	stopped=x;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TGLabel * WFGUI::GetCalculatingLabel() {
	return CalculatingLabel;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TGLabel * WFGUI::GetCalculatingLabel2() {
	return CalculatingLabel2;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetPlotUpdate(Bool_t onoff) {
	plotupdate=onoff;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::Getplotupdate() {
	return plotupdate;
}	
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetLess2DPlot(Bool_t onoff) {
	Less2DPlot=onoff;
	SetLessPlot(onoff);
	//	cout << GetLess2DPlot() << " get less 2D " << endl;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetLess2DPlot() {
	return Less2DPlot;
}	

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetLessPlot(Bool_t onoff) {
	LessPlot=onoff;
	if (onoff)
	  {
	    OnStripsButton2->SetEnabled(kFALSE);
	    BetweenStripsButton2->SetEnabled(kFALSE);		
	    ExButton2->SetEnabled(kFALSE);				
	    EyButton2->SetEnabled(kFALSE);				
	    DrawCutsUserEntry2->SetEnabled(kFALSE);
	    OnStripsButton->SetEnabled(kFALSE);
	    BetweenStripsButton->SetEnabled(kFALSE);		
	    ExButton->SetEnabled(kFALSE);				
	    EyButton->SetEnabled(kFALSE);		
	    DrawCutsUserEntry->SetEnabled(kFALSE);		
	  }
	else if(!GetLess2DPlot())
	  {
	    OnStripsButton2->SetEnabled(kTRUE);
	    BetweenStripsButton2->SetEnabled(kTRUE);		
	    ExButton2->SetEnabled(kTRUE);				
	    EyButton2->SetEnabled(kTRUE);				
	    DrawCutsUserEntry2->SetEnabled(kTRUE);
	    OnStripsButton->SetEnabled(kTRUE);
	    BetweenStripsButton->SetEnabled(kTRUE);		
	    ExButton->SetEnabled(kTRUE);				
	    EyButton->SetEnabled(kTRUE);	
	    DrawCutsUserEntry->SetEnabled(kTRUE);			
	  }

}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetLessPlot() {
	return LessPlot;
}	
/////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetYMax(){
  return  YMAXentry->GetNumber();
}
/////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetXMax(){
  return  Pitchentry->GetNumber()*XMAXentry->GetNumber();
}
/////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetNStrips(){
  int TempXMAX = XMAXentry->GetNumber();
  if (TempXMAX%2==0) 
    {
       TempXMAX++;
       //cout << "Number of strips increased to be an  odd number " << endl;
    }
  return TempXMAX;
}

double WFGUI::GetScreenedGainEntry(){
    return ScreenedGainEntry->GetNumber();
}
////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetChargeEntry(){
    return ParticleSpecificsEntry->GetNumber();//MODIFIEDChargeentry->GetNumber();
}

////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetCalibEntry(){
    return ParticleSpecificsEntry->GetNumber();//MODIFIEDCalibentry->GetNumber();
}

////////////////////////////////////////////////////////////////////////////////////////////
int WFGUI::CallBoundaryConditions() {

  
  SetVDepl(Depletionentry->GetNumber());
  SetGainon(false);
  SetYGain(Gainentry->GetNumber());

  SetStepy(0.1);
  
  Setygainlow(0.);
  Setygainhigh(0.);
  SetGainRecess(GainIndententry->GetNumber());
  SetFluence(IrradiationEntry->GetNumber());      
  SetQuenchCoef(QuenchEntry->GetNumber());
  
  if (!GetInitialDopRemoval() && !GetAcceptorCreation() && !GetGainQuenching() && !GetCCEOn()  )  SetFluence(0);
  if (!GetGainQuenching() )  SetQuenchCoef(0);
      
  SetDJValue(DJEntry->GetNumber());
  SetDJehValue(DJehEntry->GetNumber());
  SetStepx(StepxEntry->GetNumber());
  SetStepy(StepyEntry->GetNumber());
  SetBiasEntry(Biasentry->GetNumber());

  CallSetDopingBulk();
  CallSetDopingStrip();
  SetDopinggainlayerValue(Dopingentry->GetNumber());
 
  double Ndoping = 0;
  SetGainLayerVdepletion(Ndoping);
  if (GetGainExp() == 0)  Ndoping = fabs(GetDopinggainlayerValue()); // Total doping, assuming a step doping density (base*height)
  else Ndoping = fabs(GetDopinggainlayerValue())/2.; // Total doping, assuming a step doping density (base*height/2.)

  if (GetGainDoping()) SetGainLayerVdepletion(Ndoping);
  SetVBias(Biasentry->GetNumber());
   SetGainShape(NGainShape);

  if ( GetVBias() <0)
    {
      cout << "Not enough Vbias to deplete the gain layer" << endl;
      cout << "The program stops" << endl;
      char * gridlabel3;
  
      gridlabel3 = new char[50];
      sprintf(gridlabel3, "Not enough Bias, program stops");
      CalculatingLabel->SetBackgroundColor(0xff0000); // set progress label color to red
      CalculatingLabel->SetTitle(gridlabel3);
      return 1;
    } 
  
  //  XMAXentry->GetNumber();
  float YYMAX=  YMAXentry->GetNumber();
  float Ratio[5];
  float BestRatio = 2;
  double dd;
  int Besti = 0;
  Pitchentry->GetNumber();
  Widthentry->GetNumber();
  TempEntry->GetNumber();
  SetSWidth(Widthentry->GetNumber());
  if(PotentialThread==0)
    {
      //      cout << " In WFGUI Boundary  stepy " << GetStepy() << endl;
      //     cout << " CallA" << endl;

      //   cout << " Adaptive thickness = " <<  AdaptiveThicknessButton->GetState() << endl;
      if ( GetGainon() &&  AdaptiveThicknessButton->GetState())
	{
	  for(int ii=-2; ii<3;ii++)
	    {
	      Ratio[ii+2] = 0;
	      //	      cout << "Evaluating computational grid for thickness " << YYMAX+ii << " microns" << endl;
	      YMAXentry->SetNumber(YYMAX+ii);
	      dwpot.SetPitchWidthXY(this, GetYMax(),GetNStrips(),Pitchentry->GetNumber(),Widthentry->GetNumber(), GetStepx(), GetStepy() );     
	      
	      dd = 0;
	      
	      //    cout << "Ratio = " <<  Getygainlayerlow() << " pippo " <<  Getygainlayerhigh() << endl;
		
	      for(int y=0;y<YMAXentry->GetNumber()/dwpot.GetBinSizey();y++)	 
		if  (y*dwpot.GetBinSizey() >= Getygainlayerlow())
		  if (y*dwpot.GetBinSizey()   < Getygainlayerhigh())
		    {
		      dd += dwpot.GetBinSizey() ;
		    }
	      if (dd !=0)  Ratio[ii+2] = dd/(Getygainlayerhigh() -Getygainlayerlow());
	      //   cout << "Ratio = " << Ratio[ii+2] << endl;
	      if ( fabs(Ratio[ii+2]-1) < fabs(BestRatio-1))
		{
		  BestRatio = Ratio[ii+2];
		  Besti = ii;
		  //	  cout << "Besti = " << Besti << endl;
		  //  cout << "Bestratio = " << BestRatio << endl;
		}	
	    }
	  cout << "Best computational grid has thickness " << YYMAX+Besti << " microns with ratio = " << BestRatio <<  endl;
	}
      YMAXentry->SetNumber(YYMAX+Besti);
      dwpot.SetPitchWidthXY(this, GetYMax(),GetNStrips(),Pitchentry->GetNumber(),Widthentry->GetNumber(), GetStepx(), GetStepy() );     
      
      dwpot.SetV(Biasentry->GetNumber(),GetVDepl());  				// set depeletion and bias voltage
      dwpot.SetDoping(stripdoping,bulkdoping);
      
      for(int i=0; i<dwpot.GetXMAX(); i++) {			// reset potentials to zero as a precaution
	for(int j=0; j<dwpot.GetYMAX(); j++) {
	  dwpot.SetwPotential(j,i,0.0);
	  dwpot.SetdPotential(j,i,0.0);
	}
      }
      
      
      dhist->Reset(); // Reset histograms before filling them again
      whist->Reset();
      
      cout << "Sensor thickness = " <<  dwpot.GetYMAX()*dwpot.GetBinSizey()  << " micron. Number of bins = " <<  dwpot.GetYMAX() << " Bin size on y = " << dwpot.GetBinSizey() << " micron" << endl;
      cout << "Sensor total width = " <<   dwpot.GetXMAX()*dwpot.GetBinSizex() << " micron. Number of bins = " <<  dwpot.GetXMAX() << " Bin size on x = " << dwpot.GetBinSizex() << " micron" << endl;
      //      StepxEntry->SetNumber( 0.01*((int) (100*dwpot.GetBinSizex())));
      //  StepyEntry->SetNumber(0.01*((int) (100*dwpot.GetBinSizey())));
      //      SetStepx(StepxEntry->GetNumber());
      // SetStepy(StepyEntry->GetNumber());
      
      
      dhist->TH2F::SetBins(dwpot.GetXMAX(),0,dwpot.GetXMAX()*dwpot.GetBinSizex() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );
      whist->TH2F::SetBins(dwpot.GetXMAX(),0,dwpot.GetXMAX()*dwpot.GetBinSizex() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );
      dfhist->TH2F::SetBins(dwpot.GetXMAX(),0,dwpot.GetXMAX()*dwpot.GetBinSizex() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );
      wfhist->TH2F::SetBins(dwpot.GetXMAX(),0,dwpot.GetXMAX()*dwpot.GetBinSizex() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );
      chist->TH2F::SetBins(dwpot.GetXMAX(),0,dwpot.GetXMAX()*dwpot.GetBinSizex() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );
      chhist->TH2F::SetBins(dwpot.GetXMAX(),0,dwpot.GetXMAX()*dwpot.GetBinSizex() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );
      ctothist->TH2F::SetBins(dwpot.GetXMAX(),0,dwpot.GetXMAX()*dwpot.GetBinSizex() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );
      Exhist->TH2F::SetBins(dwpot.GetXMAX(),0,dwpot.GetXMAX()*dwpot.GetBinSizex() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );
      //cout <<   dwpot.GetXMAX() << " " << dwpot.GetYMAX()	<< endl;	      
      WhereCut->SetNumber(dwpot.GetXMAX()*dwpot.GetBinSizex()/2);
      WhereCut2->SetNumber(dwpot.GetXMAX()*dwpot.GetBinSizex()/2);
      
      dwpot.SetBoundaryConditions(ReadOutTopFlag); // set boundary conditions	
      
      for(int i=0;i<dwpot.GetXMAX();i++) { // fill histogram to show the boundary conditions
	for(int j=0;j<dwpot.GetYMAX();j++)
	  {
	    dhist->SetBinContent(i+1,j+1,dwpot.Getdpot(j,i));
	    whist->SetBinContent(i+1,j+1,dwpot.Getwpot(j,i));
	  }
      }
      
      
      CarriersInNumberentry->SetNumber((dwpot.GetXMAX()*dwpot.GetBinSizex())/2);
      
      OnStripsButton2->SetEnabled(kFALSE);
      BetweenStripsButton2->SetEnabled(kFALSE);				
      ExButton2->SetEnabled(kFALSE);				
      EyButton2->SetEnabled(kFALSE);				
      DrawCutsUserEntry2->SetEnabled(kFALSE);
      OnStripsButton->SetEnabled(kFALSE);
      BetweenStripsButton->SetEnabled(kFALSE);		
      ExButton->SetEnabled(kFALSE);				
      EyButton->SetEnabled(kFALSE);		
      DrawCutsUserEntry->SetEnabled(kFALSE);		
      DrawAllGraph(-1);
      
    }
  
  //  CalculatingLabel->SetBackgroundColor(0xff0000); // set progress label color to red
  // CalculatingLabel->SetTitle("Calculating potentials ..."); // update progess label title
  //  cout << "Lock2 " <<  __LINE__<< endl;
  return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::CallSetDetType() {

  if (TypeButton[0]->IsOn())
    {
	DetType = 0;
	cout << "Bulk material set to Silicon" << endl;	
	Eion=IONENERGYSI;
	
    }
  else if (TypeButton[1]->IsOn()) 
    {
	DetType= 1;
	cout << "Bulk material set to Diamond" << endl;	
	Eion = 5.;
    }
  else if (TypeButton[2]->IsOn()) 
    {
      	DetType =  2;
	cout << "Bulk material set to Silicon Carbide" << endl;	
	Eion = IONENERGYSIC ;
    }

  return;
}
/////////////////////////////////////////////////////////////////////////////////////////////
int WFGUI::CallGetDetType() {
  return DetType;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::CallSetDopingBulk() {	
  //set bulk doping
	if(BulkButton[0]->IsOn())
	  {
	    bulkdoping = NTYPE;
	    cout << "Bulk doping  = n" << endl;
	  }
	
	else
	  {
	    bulkdoping = PTYPE;
	    cout << "Bulk doping  = p" << endl;
	    
	  }
	CalculateButton->SetEnabled(kFALSE);	
	//	cout << "CallSetDoping0 bulkdoping " << bulkdoping << endl;
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::CallSetDopingGL(Int_t id) {	
  //This is the fraction of c_boron that has beem measured for the other dopings

  // c_coefficient from R-V:
  // float B = 5.4 ; // 5.5;
  //float BC = 1.9; // 2.5 ; // 2.7; // it was 2.1;
  //float Ga = 7.5;// 6.5; //  it was 8.5
  //float GAC = 4.1;// 3.3; // it was 2.7
  //float BLD = 4.7 ;//4.1 // OK

  

  float B = 0.85; //1.1 ; // 0.96 ; // <<= UFSD3 W1 needs a larger exponent (has better rad hard)~ 1.3
  float BC = B*1.9/0.9; // ratio between k_cap coeff 2.6;//1.8; //2.6; // 2.5 ; // 2.7; // it was 2.1 on UFSD3 W5;
  float Ga = 5.4/7.5*B;// 6.5; //  it was 8.5
  float GAC = 2.1*Ga;// 3.3; // it was 2.7
  // float BLD = 4.7 ;//4.1 // OK

  //  cout << "GetGainShape() " << GetGainShape() << " " << BC << endl;
  if (GetGainShape() == 8) BC *= 1.5; //deep carbonated is more resistive
  // cout << "GetGainShape() " << GetGainShape() << " " << BC << endl;
  
  //  CalculateButton->SetEnabled(kFALSE);
  if (id == 1 )  DopingGLType = B; // Boron	
  else if (id == 2) DopingGLType = BC; //Boron+Carbon
  else if (id == 3) DopingGLType = Ga; //Gallium
  else if (id == 4) DopingGLType = GAC; //Gallium+Carbon
  else if (id == 5) DopingGLType = B; //Boron Low Diffusion

  // cout << "DopingGLType = " << DopingGLType << endl;
}
////////////////////////////////////////////////
void WFGUI::CallSetParticleIrr() {	
  //set particle type
  CalculateButton->SetEnabled(kFALSE);
  if(ParticleIrrButton[0]->IsOn())  ParticleIrrType = 0; // neutrons	
  else ParticleIrrType = 1; //pions
  
  //  cout << "CallSetParticleIrr " << ParticleIrrType << endl;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::CallSetDJType() {	
  if (DJButton[0]->IsOn()) DJvalue = 0;
  if (DJButton[1]->IsOn()) DJvalue = 1;

  if (DJvalue == 0 ) cout << "Linear doping density for the Double Junction computation  " << endl;
  else  cout << "Step doping density for the Double Junction computation  " << endl;
}
/////////////////////////////////////////////////////////////////////////////////////////////
int WFGUI::CallGetDJType() {	
  return DJvalue;
	//	cout << "CallSetDoping0 bulkdoping " << bulkdoping << endl;
}
/////////////////////////////////////////////////////////////////////////////////////////////
int WFGUI::CallGetDopingBulk() {	
  //set bulk doping
	return bulkdoping;
	
}
int WFGUI::CallGetDopingStrip() {	
  //set bulk doping
	return stripdoping;
	
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::CallSetReadOut() {	
  //set bulk doping
	if(ReadOutButton[0]->IsOn())
	  {
	    ReadOutTopFlag = true;
	    cout << "Top DC readout selected" << endl;	    
	    SetAC(0);
	  }
	else if(ReadOutButton[1]->IsOn())
	{
	    ReadOutTopFlag = true;
	    SetAC(1);
	    cout << "Top AC readout selected" << endl;	    
	  }
	
	else if(ReadOutButton[2]->IsOn())
	  {
	    ReadOutTopFlag = false;
	    cout << "Backplane readout selected" << endl;
	    SetAC(0);
	  }
	CalculateButton->SetEnabled(kFALSE);	


	//cout << "Needs to compute the potential again!! " << endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::CallSetDopingStrip() {			//set strip doping
	if(StripButton[0]->IsOn()) 
	  {
	    stripdoping = NTYPE;
	    cout << "Strip doping  = n" << endl;
	  }
	else 
	  {
	    stripdoping = PTYPE;
	    cout << " Strip doping  = p" << endl;
	  }
	CalculateButton->SetEnabled(kFALSE);	
}
/////////////////////////////////////////////
void WFGUI::SetGainDepth(double dpet)
{
  GainShapeDepth = dpet+ELECTRODE_DEPTH;
  return;
}
void WFGUI::SetGainLength(double dpet)
{
  GainShapeLength = dpet;
  return;
}
void WFGUI::SetGainExp(double dpet)
{
  GainShapeExp = dpet;
  return;
}

double WFGUI::GetGainDepth()
{
  return  GainShapeDepth;
}

double WFGUI::GetGainExp()
{
  return  GainShapeExp;
}

double WFGUI::GetGainLength()
{
  return  GainShapeLength;
}


void WFGUI::SetGainShape(Int_t id)
{


  CalculateButton->SetEnabled(kFALSE);
  NGainShape = id;

  if (NGainShape == 0)
    {
      SetGainExp(0);
      SetGainLength(0);
      SetGainDepth(0);
      Dopingentry->SetNumber(0);
      SetGainDoping(0); //0
      //      SetGainKind(0);
      //      SamplingEntry->SetNumber(3);
      cout << "Sensor without Gain layer has been selected" << endl;
      SetGainon(false);
    }
  
  else if (NGainShape == 1)
    {
      SetGainExp(CNMDexp);
      SetGainLength(CNMGAINLENGTH);
      SetGainDepth(CNMGAINDEPTH);
      SamplingEntry->SetNumber(0.1); //1
      cout << "I) Implant decreasing linearly from Junction " << endl;
    }
    else if (NGainShape == 2)
      {
	SetGainExp(FBKDexp);
	SetGainLength(0.5);
	SetGainDepth(1.0);
       	cout << "II) Implant at 0.5 - 1.0 micron from junction" << endl;
	SamplingEntry->SetNumber(0.1); //2

      }
    else if (NGainShape == 3)
      {
	SetGainExp(FBKDexp);

	SetGainLength(3);
	
	SetGainDepth(0);
	SamplingEntry->SetNumber(0.1); //3
	cout << "VIII) Implant at 0.0 - 3.0 micron from junction" << endl;
      }
      else if (NGainShape == 4)
      {
	SetGainExp(FBKDexp);
	//	SetGainLength(FBKLDGAINLENGTH);
	//	SetGainDepth(FBKLDGAINDEPTH);
	SamplingEntry->SetNumber(0.1); //4
	SetGainLength(0.4);
	SetGainDepth(1.0);
	cout << "III) Implant at 0.6 - 1.0 micron from junction" << endl;
      }
      else if (NGainShape == 5)
      {
	SetGainExp(HPKDexp);
	SamplingEntry->SetNumber(0.1); //5
	SetGainLength(0.6);
	SetGainDepth(1.9);
	cout << "V) Implant at 1.3 - 1.9 micron from junction" << endl;	
      }
      else if (NGainShape == 6)
      {
	SetGainExp(HPKDexp);
	SetGainLength(HPKLDDEEPGAINLENGTH);
	SetGainDepth(HPKLDDEEPGAINDEPTH);
	SamplingEntry->SetNumber(0.1); //6
	SetGainLength(0.4);
	SetGainDepth(2.2);
       	cout << "VII) Implant at 1.8 - 2.2 micron from junction" << endl;

	
      }
        else if (NGainShape == 7)
      {
	SetGainExp(HPKDexp);
	SetGainLength(HPKLDGAINLENGTH);
	SetGainDepth(HPKLDGAINDEPTH);
	SamplingEntry->SetNumber(0.1); //7

	SetGainLength(0.4);
	SetGainDepth(1.5);
	cout << "IV) Implant at 1.1 - 1.5 micron from junction" << endl;
	
      }

	else if (NGainShape == 8)
      {
	SetGainExp(FBKDexp);
	SetGainLength(FBKLDDEEPGAINLENGTH);
	SetGainDepth(FBKLDDEEPGAINDEPTH);
	SamplingEntry->SetNumber(0.1); //7
	cout << "VI) Implant at 1.6 - 2.1 micron from junction" << endl;



	SetGainLength(0.5);
	SetGainDepth(2.1);
	//	cout << "Gain implant 1.6 - 2.1 um from junction" << endl;
	
      }
    else if (NGainShape == 9)
      {
	SetGainExp(FBKDexp);
	SamplingEntry->SetNumber(0.1); //3
	cout << "IX) Implant at 3.0 - 3.5 micron from junction" << endl;
	SetGainDepth(3);
	SetGainLength(0.5);

      }
      else if (NGainShape == 10)
      {
	SetGainExp(FBKDexp);
	SetGainLength(0.5);
	//	SetGainLength(MONODEEPGAINLENGTH);
	//SetGainDepth(MONODEEPGAINDEPTH);
	SamplingEntry->SetNumber(0.1); //3
       	cout << "X) Implant pn near ohmic contact" << endl;

	SetGainDepth(GetYMax()-MONOGAINPOSITION);
	cout << "Gain layer depth = " << GetYMax()-MONOGAINPOSITION << endl;
	
      }
  
  

  //  SetGainShape(NGainShape);

  
  if (GetGainDoping())  cout << "Gain implant position = " << Getygainlayerlow() << " - " << Getygainlayerhigh() << endl;
  //  cout << "Gain layer position: " << Getygainlayerlow() << " - " << Getygainlayerhigh() << endl;
  //  SetGainKind(GetGainKind());
  SetGainDoping(GetGainDoping());
  
  return;
}

int WFGUI::GetGainShape()
{
  return  NGainShape; 
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetGainKind(Int_t id) {
  NGainKind = id;
  CalculateButton->SetEnabled(kFALSE);
  //  SamplingEntry->SetNumber(1000);

  if (id !=0)
    {
      cout << "Detector with gain: forced step y" << endl;
      SetStepy(0.08);
      StepyEntry->SetNumber(0.08);
      StepyEntry-> SetState(kFALSE);
    }
  
  if (id == 0)
    {
      Dopingentry->SetNumber(0);
      cout << "Gain mechanism off" << endl;
      StepyEntry-> SetState(kTRUE);
    }
  else if (id == 1)
    {
      cout << "Impact ionization simulation: van Overstraeten – de Man model" << endl;

    }
  else if (id == 2)
    {
      cout << "Impact ionization simulation: Massey LGAD model" << endl;
    }
  else if (id == 3)
    {
      cout << "Impact ionization simulation: Okuto - Crowell modell" << endl;

    }
  else if (id == 4)
    {
      cout << "Impact ionization simulation: Bologna model" << endl;

    }
    else if (id == 5)
    {
      cout << "Impact ionization simulation: Massey optimized as in paper by Rivera, Moll, Study of impact... " << endl;

    }
      else if (id == 6)
    {
      cout << "Impact ionization simulation: van Overstraeten optimized as in paper by Rivera, Moll, Study of impact... " << endl;

    }
  
  //  cout << " Gain Type = " << GainType << endl;

  
}

////////////////////////////////////////////////

  int WFGUI::GetGainKind() {
  return NGainKind;
    //  cout << " Gain Type = " << GainType << endl;
    }
    
/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetGainDoping(Int_t id) {
  if (id == 0 && NGainDoping !=0)
    {
      GainShape->Select(0);
      GainShape->SetEnabled(kFALSE);
      GainDoping->Select(0);
      SamplingEntry->SetNumber(0.3);
      SamplingEntry->SetState(kTRUE);
    }
  if (id > 0 && NGainDoping ==0)
    {
      CalculateButton->SetEnabled(kFALSE);
      GainShape->SetEnabled(kTRUE);
      GainShape->Select(2);
      SamplingEntry->SetNumber(0.1);
      SamplingEntry->SetState(kFALSE);
      //      SetStepy(0.2);
      // StepyEntry->SetNumber(0.2);      
    }
  CalculateButton->SetEnabled(kFALSE);
  NGainDoping = id;
  //  CalculateButton->SetEnabled(kFALSE);
  //  SamplingEntry->SetNumber(1000);
  if (id == 0 ) cout << "No Gain layer " << endl;
  else if      (id == 1)  cout << "Gain implant doping: Boron" << endl;  
  else if (id == 2)  cout << "Gain implant doping: Boron + Carbon" << endl;  
  else if (id == 3)  cout << "Gain implant doping: Gallium " << endl;
  else if (id == 4)  cout << "Gain implant doping: Gallium + Carbon" << endl;
  else if (id == 5)  cout << "Gain implant doping: Carbon Low Diffusion" << endl;
  
  CallSetDopingGL(NGainDoping);
}

  int WFGUI::GetGainDoping() {
  return NGainDoping;
    //  cout << " Gain Type = " << GainType << endl;
    }
    
    ///////////////////////////////////////
void WFGUI::CallSetCSA(Int_t id) {

  // SetNA62On(0);
  SetSC_CSAOn(0);

  if (id == 0)
    {
      ShTransEntry->SetNumber(10);
      ShNoiseEntry->SetNumber(1.3);
      TRiseEntry->SetNumber(4);
      TFallEntry->SetNumber(6);
      CSAImpEntry->SetNumber(60);
    }
      
  else if (id == 1)
    {
      
      // TOFFEE 
      CSAImpEntry->SetNumber(1./TOFFEE_gm);
      // double Ci = TOFFEE_gain*TOFFEE_Cf; // 70 fF feedback
      //	double Qfrac = 1./(1.+ GetCDet()*1E-12/Ci);
      //	double taurise_CSA_RC = 1.0e-12*GetCDet()*GetCSAImp(); //
      //	double taufall_CSA_RC = 1.0e-12*GetCDet()*GetCSAImp(); //
      
      
      ShTransEntry->SetNumber(7.3);
      ShNoiseEntry->SetNumber(1.6);
      TRiseEntry->SetNumber(3);
      TFallEntry->SetNumber(6);
    }
  
 else  if (id == 2)
    {
      SetNA62On(1);
    }
  else if (id == 3)
    {
      SetSC_CSAOn(1);
    }

  else if (id == 4)
    {
      BBNoiseEntry->SetNumber( 0.96*((int) ((0.17*log(CDEntry->GetNumber())+2.98)*10))/10.);
      cout << "TI selected: FAST EVO low gain" << endl;
      BBImpEntry->SetNumber(90);
      //      if (CDEntry->GetNumber()<2)       BBGainEntry->SetNumber(25);
      BBGainEntry->SetNumber(25);

      //      if  (CDEntry->GetNumber()<1)
	BBBWEntry->SetNumber(0.66);      
	// else BBBWEntry->SetNumber( 1.*((int) (729*pow(CDEntry->GetNumber(), -0.61)))/1000);
      //      BBNoiseEntry->SetNumber(2.7);
    }

    else if (id == 5)
    {
      
      cout << "TI selected: FAST EVO mid gain" << endl;
      BBNoiseEntry->SetNumber( 1.*((int) ((0.17*log(CDEntry->GetNumber())+2.98)*10))/10.);
      BBImpEntry->SetNumber(165);
      BBGainEntry->SetNumber(55);
      //if  (CDEntry->GetNumber()<1)
      BBBWEntry->SetNumber(0.48);
      // else BBBWEntry->SetNumber( 1.*((int) (478*pow(CDEntry->GetNumber(), -0.727)))/1000);
      BBNoiseEntry->SetNumber(2.7);
    }

    else if (id == 6)
    {
      cout << "TI selected: FAST EVO high gain" << endl;
      BBNoiseEntry->SetNumber( 1.06*((int) ((0.17*log(CDEntry->GetNumber())+2.98)*10))/10.);
      BBImpEntry->SetNumber(490);

      BBGainEntry->SetNumber(146);
      //if  (CDEntry->GetNumber()<1)
      BBBWEntry->SetNumber(0.23);
      // else BBBWEntry->SetNumber( 1.*((int) (478*pow(CDEntry->GetNumber(), -0.86)))/1000);
      
      BBNoiseEntry->SetNumber(2.7);
    }
    else if (id == 7)
    {
      cout << "TI selected: Santa Cruz + x10" << endl;
      BBImpEntry->SetNumber(25);
      BBGainEntry->SetNumber(4.7);
      BBBWEntry->SetNumber(0.7);
      BBNoiseEntry->SetNumber(1.5);
    }
  
}

	      
// ///////////////////////////////////////////////////////////////////////////////////////////
int WFGUI::CallSetPart(Int_t id) {


  //Edge always false unless is edge //
  EdgeNumberentry -> SetState(kFALSE);
  NumberEntry -> SetState(kFALSE);
  EdgeLabel ->Disable(kTRUE);
  CarriersInNumberentry->SetState(kTRUE);
  CarriersAngleNumberentry->SetState(kTRUE);
  BatchRandomButton ->SetEnabled(kTRUE);
  ParticleSpecificsLabel1->Disable(kTRUE);
  ParticleSpecificsLabel2->Disable(kTRUE);
  ParticleSpecificsLabel3->Disable(kTRUE);
  ParticleSpecificsLabel4->Disable(kTRUE);
  
     if (id == 1) {
         radiobuttonstatus=MIPunif;
	 //         ParticleSpecificsEntry->SetNumber(0);
         // ParticleSpecificsEntry->SetState(kFALSE);
         // NumberEntry->SetState(kTRUE);
         // NumberLabel->Disable(kFALSE);

	 ParticleSpecificsLabel1->Disable(kFALSE);
	 ParticleSpecificsLabel1->SetTextColor(gROOT->GetColor(1), kFALSE);
	 ParticleSpecificsEntry->SetNumber(GetMPV());
	 ParticleSpecificsEntry->SetState(kTRUE);
	 NumberEntry->SetState(kTRUE);
	 NumberLabel->Disable(kFALSE);
         cout << "Selecting MIP uniform distributed" << endl;
         return 0;
     }
    
     if (id == 2) {
         radiobuttonstatus=MIPnonunif;
	 //         ParticleSpecificsEntry->SetNumber(0);
         //ParticleSpecificsEntry->SetState(kFALSE);
         //NumberEntry->SetState(kTRUE);
         //NumberLabel->Disable(kFALSE);
	 ParticleSpecificsLabel1->Disable(kFALSE);
	 ParticleSpecificsLabel1->SetTextColor(gROOT->GetColor(1), kFALSE);
	 ParticleSpecificsEntry->SetNumber(GetMPV());
	 ParticleSpecificsEntry->SetState(kTRUE);
	 NumberEntry->SetState(kTRUE);
	 NumberLabel->Disable(kFALSE);
         cout << "Selecting MIP non uniform distributed with fixed total amplitude" << endl;
         return 1;
     }
    
     if (id == 3) {
         radiobuttonstatus=MIPlandau;
         ParticleSpecificsEntry->SetNumber(0);
         ParticleSpecificsEntry->SetState(kFALSE);
         NumberEntry->SetState(kTRUE);
         NumberLabel->Disable(kFALSE);
         cout << "Selecting MIP Landau distributed" << endl;
         return 2;
     }
    
    if (id == 4) {
        radiobuttonstatus=USR_CHARGE;
        ParticleSpecificsLabel1->Disable(kFALSE);
        ParticleSpecificsLabel1->SetTextColor(gROOT->GetColor(1), kFALSE);
	ParticleSpecificsEntry->SetNumber(65);
        ParticleSpecificsEntry->SetState(kTRUE);
        NumberEntry->SetState(kTRUE);
        NumberLabel->Disable(kFALSE);
        cout << "Selecting Laser (1064 nm)" << endl;
	cout << "EField screening decreased by a factor of 3 to fit experimental data" << endl;
        return 3;
    }
    
    if (id == 5 || id == 6) {
        int top_or_bottom = 4;

        if (id == 5) {
            radiobuttonstatus=ALPHA_TOP;
            cout << "Selecting Alpha from top" << endl;
            top_or_bottom = 4;
        }
        if (id == 6) {
            radiobuttonstatus=ALPHA_BOTTOM;
            cout << "Selecting Alpha from bottom" << endl;
            top_or_bottom = 5;
        }

	ParticleSpecificsLabel2->Disable(kFALSE);
	ParticleSpecificsLabel2->SetTextColor(gROOT->GetColor(1), kFALSE);

        ParticleSpecificsEntry->SetNumber(10);
	
        ParticleSpecificsEntry->SetState(kTRUE);
        NumberEntry->SetState(kFALSE);
        NumberLabel->Disable(kTRUE);
        return top_or_bottom;
    }
 
    if (id == 7) {
        radiobuttonstatus= CALIB;
	BatchRandomButton ->SetEnabled(kFALSE);
	CarriersInLabel ->Disable(kTRUE);
	CarriersInNumberentry->SetState(kFALSE);
	CarriersAngleNumberentry->SetState(kFALSE);
        ParticleSpecificsLabel3->Disable(kFALSE);
        ParticleSpecificsLabel3->SetTextColor(gROOT->GetColor(1), kFALSE);
        ParticleSpecificsEntry->SetNumber(4);
        ParticleSpecificsEntry->SetState(kTRUE);
        NumberEntry->SetState(kFALSE);
        NumberLabel->Disable(kTRUE);
	//	CalculateButton->SetEnabled(kTRUE); 
		      
        cout << "Selecting Electronic Pulse"<< endl;
        return 6;
    }

         if (id == 8) {
        radiobuttonstatus=EDGE;
	BatchRandomButton ->SetEnabled(kTRUE);
	CarriersInLabel ->Disable(kTRUE);
	CarriersInNumberentry->SetState(kFALSE);
	CarriersAngleNumberentry->SetState(kFALSE);
	ParticleSpecificsEntry->SetNumber(65);
        ParticleSpecificsEntry->SetState(kTRUE);
        ParticleSpecificsLabel1->Disable(kFALSE);
        ParticleSpecificsLabel1->SetTextColor(gROOT->GetColor(1), kFALSE);
	ParticleSpecificsEntry->SetNumber(65);	
	EdgeNumberentry -> SetState(kTRUE);
	EdgeLabel ->Disable(kFALSE);

        cout << "Selecting Edge Laser"<< endl;
        return 8;



	 }
         if (id == 9) {
        radiobuttonstatus=XRAY;
	BatchRandomButton ->SetEnabled(kTRUE);
	CarriersInNumberentry->SetState(kTRUE);
	CarriersAngleNumberentry->SetState(kTRUE);
        ParticleSpecificsLabel4->Disable(kFALSE);
	ParticleSpecificsEntry->SetNumber(10);
        ParticleSpecificsEntry->SetState(kTRUE);
	NumberEntry->SetState(kTRUE);
	NumberLabel->Disable(kFALSE);
	EdgeNumberentry -> SetState(kTRUE);
	EdgeLabel ->Disable(kFALSE);
        cout << "Selecting X-Ray"<< endl;
	cout << " For the absorption probability see: http://web-docs.gsi.de/~stoe_exp/web_programs/x_ray_absorption/index.php" << endl;
        return 9;
    }

	 if (id == 10) {
         radiobuttonstatus=TRENCH;

	 BatchRandomButton ->SetEnabled(kTRUE);
	 CarriersInLabel ->Disable(kTRUE);
	 CarriersInNumberentry->SetState(kFALSE);
	 CarriersAngleNumberentry->SetState(kFALSE);
	 ParticleSpecificsEntry->SetNumber(65);
	 ParticleSpecificsEntry->SetState(kTRUE);
	 ParticleSpecificsLabel1->Disable(kFALSE);
	 ParticleSpecificsLabel1->SetTextColor(gROOT->GetColor(1), kFALSE);
	 ParticleSpecificsEntry->SetNumber(65);	
	 EdgeNumberentry -> SetState(kTRUE);
	 EdgeLabel ->Disable(kFALSE);

         ParticleSpecificsEntry->SetNumber(0);
	 //         ParticleSpecificsEntry->SetState(kFALSE);
	 // NumberEntry->SetState(kTRUE);
	 // NumberLabel->Disable(kFALSE);
         cout << "Selecting Edge MIP Landau" << endl;
         return 10;
     }
 
	
	return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::FillHist()
{
  double *VSteps;
  double *HSteps;
  int VNBins = 0;
  int HNBins = 0;
  float Xpos = 0;
  float Ypos = 0;
  int YStep = 0;
  int XStep = 0;
  XStep =  max( (int) (dwpot.GetXMAX()/600), 1);
  
  VSteps = new double[dwpot.GetYMAX()];
  HSteps = new double[dwpot.GetXMAX()];

  for(int k=0;k<dwpot.GetXMAX();k+=XStep)
    {
      HSteps[HNBins] = 1.*k*dwpot.GetBinSizex();
      //cout << "HSteps = " << HNBins << " " << k*dwpot.GetBinSizex() << " " << HSteps[HNBins] <<endl;
      HNBins++;

    }
      HNBins--;
  
  //	cout << Step << "Step " << endl;
  for(int k=0;k<dwpot.GetYMAX();k+=YStep)
    {
      Ypos = 1.*k*dwpot.GetBinSizey();
      if (Ypos < 5. || Ypos >  GetYMax()-5. )  YStep = 1;
      else  YStep =  max( (int) (dwpot.GetYMAX()/600), 1);

      VSteps[VNBins] = Ypos;

      //cout << "VSteps = " << VNBins << " " << Ypos << " " << VSteps[VNBins] <<endl;
      VNBins++;	    
    }
  VNBins--;	    
  //  for  ( int kk = 0; kk<HNBins;kk++) cout << "HSteps = " <<kk << " " << HSteps[kk] <<endl;

  canvasp->Clear();
  canvasw->Clear();
  dhist->Reset();	//Reset histogram
    ///dhist->TH2F::SetBins( dwpot.GetXMAX(), 0,dwpot.GetXMAX()*dwpot.GetBinSizex() ,dwpot.GetYMAX() ,0,  dwpot.GetYMAX()*dwpot.GetBinSizey() );
    //    dhist->TH2F::SetBins( (int) HNBins, 0,dwpot.GetXMAX()*dwpot.GetBinSizex() ,VNBins ,0,  dwpot.GetYMAX()*dwpot.GetBinSizey() );
    dhist->TH2F::SetBins( HNBins, HSteps ,VNBins ,VSteps );
    dhist->GetXaxis()->SetLabelColor(1);	
    dhist->GetYaxis()->SetLabelColor(1);					
    dhist->SetStats(0);
    
    whist->Reset();	//Reset histogram
    ///whist->TH2F::SetBins( dwpot.GetXMAX(), 0,dwpot.GetXMAX()*dwpot.GetBinSizex() ,dwpot.GetYMAX() ,0,  dwpot.GetYMAX()*dwpot.GetBinSizey() );
    whist->TH2F::SetBins(  HNBins, HSteps ,VNBins , VSteps );
    whist->GetXaxis()->SetLabelColor(1);	
    whist->GetYaxis()->SetLabelColor(1);					
    whist->SetStats(0);
    Xpos = 0;
    for(int i=0;i<HNBins;i++)
      {		// fill histogram
	Xpos =  (int) (HSteps[i]/dwpot.GetBinSizex());
	for(int j=0;j<VNBins;j++)
	  {
	    Ypos =  (int) (VSteps[j]/dwpot.GetBinSizey());
	    dhist->SetBinContent(i+1,j+1,dwpot.Getdpot(Ypos,Xpos));
	    whist->SetBinContent(i+1,j+1,dwpot.Getwpot(Ypos,Xpos));
	  }
      }

    
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::DrawAllGraph(int LCol = 1)
{
  //  cout << " DrawAllGraph 2DPlot " << GetLess2DPlot() << " LCol= " <<LCol << " GetLessPlot " << GetLessPlot()<< endl;

  SetStyle();
  
  //  cout << __LINE__<< endl;	
  
  //  if(GetFileNameOn()) return;
  
  
  
  //  cout << wherecut << " wherecut" <<  " " << "LCol = " << LCol << endl;
  
  
  
  
  canvasp->Clear();
  canvasw->Clear();
  //  canvasp->Update(); 
  // canvasw->Update();     
  
  canvasp->cd();
  dwpot.DriftPal();
  dhist->SetStats(0); // hide statistics box
  whist->SetStats(0);
  dhist->Draw("COLZ");
  
  
  canvasw->cd();
  dwpot.DriftPal();
  whist->Draw("COLZ");
  // canvasw->Update();
  canvasp->cd();

  // memory clicking ON strips below  this line
  
  if (LCol == -1)
    {
      //      delete NLine1;
      // delete NLine2;
      DrawStrips();
      canvasw->Update(); 
      canvasp->Update();
      canvaswc->Clear();
      canvaspc->Clear();
      canvaswc->Update(); 
      canvaspc->Update();
      return;
    }

  
  
  wherecut = WhereCut->GetNumber(); 
  wherecut2 = WhereCut2->GetNumber();   
  NLine1 = new TLine();
  NLine2 = new TLine();
  NLine1->SetLineStyle(2);
  NLine2->SetLineStyle(2);
  
  //  cout << __LINE__<< endl;	
  //  cout << "Line1" << endl;
  
  NLine1->DrawLine(wherecut,0,wherecut,GetYMax());
  if(fieldyes==true) DrawFieldHist(); // Nicolo1

  
  
  canvasw->cd();
  dwpot.DriftPal();
  whist->Draw("COLZ");
  
  // cout << "Line2" << endl;
  NLine2->DrawLine(wherecut2,0,wherecut2,GetYMax());

  DrawStrips();
  canvasw->Update(); 
  canvasp->Update(); 
  


  //  if (GetLessPlot() || LCol == 0 || GetFileNameOn() ) return;
    if ( LCol == 0 ) return;

  ///nicolo The comand "Divide" causes memory leaks..."
  canvaswc->Clear();
  canvaspc->Clear();
  canvaswc->Divide(2,1,0.002,0.002);

  canvaspc->Divide(2,2,0.004,0.004);
  
  DrawCutGraph(LCol); // plot the bottom left plot, the Potentials
  DrawDopDensGraph(LCol);
  DrawFieldsAbs(LCol);
  canvaspc->Update();
  canvaswc->Update();
  
  cout << "===== Finished DrawAllGraph ======" << endl;

  delete NLine1;
  delete NLine2;
  
  return;

}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::DrawStrips()
{
  int thickness = max(1.,GetYMax()/50.);

  int bx3 =  (1 + dwpot.GetXMAX()/2 -  dwpot.Getwidth()/2)*dwpot.GetBinSizex();
  int bx4 = (1 + dwpot.GetXMAX()/2 +  dwpot.Getwidth()/2)*dwpot.GetBinSizex();

  int bx1 = bx3 - dwpot.Getpitch()*dwpot.GetBinSizex();
  int bx2 = bx4 - dwpot.Getpitch()*dwpot.GetBinSizex();

  int bx5 = bx3 + dwpot.Getpitch()*dwpot.GetBinSizex();
  int bx6 = bx4 + dwpot.Getpitch()*dwpot.GetBinSizex();
  
  
  TBox *bb1 = new TBox( bx1 ,dwpot.GetYMAX()*dwpot.GetBinSizey()-1. , bx2,dwpot.GetYMAX()*dwpot.GetBinSizey()-1.+thickness);
  TBox *bb2 = new TBox( bx3 ,dwpot.GetYMAX()*dwpot.GetBinSizey()-1.  , bx4,dwpot.GetYMAX()*dwpot.GetBinSizey()-1. +thickness);
  TBox *bb3 = new TBox( bx5 ,dwpot.GetYMAX()*dwpot.GetBinSizey()-1.  , bx6,dwpot.GetYMAX()*dwpot.GetBinSizey()-1. +thickness);
  
  bb1->SetFillColor(1);
  bb2->SetFillColor(1);
  bb3->SetFillColor(1);
  
  canvasp->cd();

  bb2->Draw();
  
  if (GetNStrips()>=2)
    {
      bb1->Draw();
      bb3->Draw();
    }
  
  return;

}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::DrawCutGraph(int LCol)
{
  // return;
  int wherecutbin=  WhereCut->GetNumber()/dwpot.GetBinSizex();
  //  wherecut2= WhereCut2->GetNumber();
	cout << " " << endl;
	cout << "Plotting the field cutting in x  at " << WhereCut->GetNumber() << endl;
	double* q=0;
	int Step = max( (int) (dwpot.GetYMAX()/600), 1);
	int Nbin = dwpot.GetYMAX()/Step;

	q = new double[Nbin];
	for(int k=0;k<Nbin;k++) q[k]=k*Step*dwpot.GetBinSizey();
	double *dpoty=0;
	float dMax = 0.;
	float dMin = 0.;
	dpoty = new double[Nbin];					
	for(int k=0;k<Nbin;k++) 
	  {
	    dpoty[k] = dwpot.Getdpot(k*Step,wherecutbin);
	    //	    cout << " dpoty = " << k*dwpot.GetBinSizey() << " " <<   dpoty[k] << endl;
	    if (dpoty[k]> dMax) dMax = dpoty[k];
	    if (dpoty[k]< dMin) dMin = dpoty[k];
	  }
	TGraph driftcut2(Nbin,q,dpoty);
	
	driftcut= new TGraph(Nbin,q,dpoty);
	driftcut->GetXaxis()->SetTitle("y [um]");
	driftcut->GetYaxis()->SetTitle("Potential [V]");
   	driftcut->GetXaxis()->SetNdivisions(5);
	driftcut->SetTitle(" Drift Potential V [V]");
	driftcut->GetXaxis()->SetLabelColor(1);
	driftcut->GetYaxis()->SetLabelColor(1);
   	driftcut->GetYaxis()->SetRangeUser(dMin*1.3,dMax*1.3);
	
   	driftcut->SetLineWidth(3);
	driftcut->SetLineColor(LCol); // set line color to black

	canvaspc->cd(1);
	driftcut->Draw("AL");
	//	canvaspc->Update();

	//	cout << " Done Drift" << endl;

	double *wpoty=0;
	float wMax = 0.;
	float wMin = 0.;
	wpoty = new double[Nbin];					
	for(int k=0;k<Nbin;k++) 
	  {
	    //wpoty[k] = dwpot.Getwpot(k*Step,wherecutbin);
	    //wpoty[k] = dwpot.Getwpot(k*Step,wherecutbin);
	   wpoty[k] = 1e-3*wf[k*Step][wherecutbin].Getabs();
	   wpoty[k] = 1e-3*wf[k*Step][wherecutbin].GetFieldy();
	   //	   cout << "Fields along y = " <<  wpoty[k] << " k = " << k << endl;
	  if (wpoty[k] > wMax) wMax = wpoty[k];
	  if (wpoty[k] < wMin) wMin = wpoty[k];
	  }

	weightcut = new TGraph(Nbin,q,wpoty);
	weightcut->GetXaxis()->SetTitle("y [um]");
	weightcut->GetYaxis()->SetTitle("[10^3/m]");
	weightcut->GetYaxis()->SetTitleOffset(1.5);
   	weightcut->GetXaxis()->SetNdivisions(5);
	weightcut->SetTitle("WF along y @ the cut");	
	weightcut->GetXaxis()->SetLabelColor(1);
	weightcut->GetYaxis()->SetLabelColor(1);
   	weightcut->GetYaxis()->SetRangeUser(wMin*1.3,wMax*1.3);
   	weightcut->SetLineWidth(3);
	weightcut->SetLineColor(LCol); // set line color to black
	//	canvaswc->Divide(1,1,0.002,0.002);
	canvaswc->cd(1);
	canvaswc->cd(1)->SetLeftMargin(.15);
	canvaswc->cd(1)->SetRightMargin(.15);	
	weightcut->Draw("AL");
	// canvaswc->Update();

	//	cout << " Done Weighting" << endl;

	delete[] q;
	delete[] dpoty;
	delete[] wpoty;
	//	delete driftcut;
	// delete weightcut;

	
}
////////////////////////////////////////////
void WFGUI::DrawDopDensGraph(int LCol)
{
  int wherecutbin=  WhereCut->GetNumber()/dwpot.GetBinSizex();
	double* q=0;


	
	int Step = max( (int) (dwpot.GetYMAX()/600), 1);

	int NBins = 0;
	float pos = 0;
	int Npos = 0;
	int *VSteps;
	VSteps = new int[dwpot.GetYMAX()];

	// variable step size for the histograms: highest precision in the top and bottom 5 micron
	
	for(int k=0;k<dwpot.GetYMAX();k+=Step)
	  {
	    pos = k*dwpot.GetBinSizey();
	    if (pos < 5. || pos >  GetYMax()-5. )    Step = 1;
	    else  Step =  max( (int) (dwpot.GetYMAX()/600), 1);

	    VSteps[NBins] = Step;
	    
	    NBins++;	    
	  }
	
	q = new double[NBins];
	Npos = 0;
	for(int k=0;k<NBins;k++)
	  {

	    q[k]=Npos*dwpot.GetBinSizey(); //(int)(pow(2,pot.Getref()));
	    Npos +=VSteps[k];
	    //  cout <<  k  << " q1[k] " << q1[k] << " Npos " << Npos << endl;	    
	  }	
	//	for(int k=0;k<Nbin;k++) q[k]=k*Step*dwpot.GetBinSizey();
       	double *dpoty=0;
	float dMax = 0.;
	float dMin = 0.;
	float YMax = 0.;
	dpoty = new double[NBins];
	//dopdensity = eN/epsilon	
	Npos = 0;
	for(int k=0;k<NBins;k++) 
	  {
	    
	    // dpoty[k] = (dwpot.Getdopyx(Npos,wherecutbin)*(EPSILON*EPSILONR)/ECHARGE*1e-6)*1e-12; //DopDensity is N/epsilon [m^3] charge density in unit of N/cm3
	    dpoty[k] = dwpot.Getdopyx(Npos,wherecutbin)*1e-12*1e-6; //DopDensity is N [m^3] charge density,  in unit of N/cm3 * 10^12
	    // cout << " k = " << k << " : " << dpoty[k] << " " << dwpot.Getdopdensity(k) << endl;
	    if (fabs(dpoty[k]) != 0 ) YMax = Npos;	    
	    if (fabs(dpoty[k])> dMax) dMax = fabs(dpoty[k]);
	    if (dpoty[k]< dMin) dMin = dpoty[k];
	    Npos +=VSteps[k];
	  }
	//	TGraph dopdens(Nbin,q,dpoty);
	
	dopdens= new TGraph(NBins,q,dpoty);
	dopdens->GetXaxis()->SetTitle("y [um]");
	dopdens->GetYaxis()->SetTitle("Doping Density [N/cm^3]");	
   	dopdens->GetXaxis()->SetNdivisions(5);
	dopdens->SetTitle(" Eff. Doping e(N_{D}f_{D}-N_{A}f_{A})+rho_{Bias}  [n/cm^{3}*10^{12}]");
	dopdens->GetXaxis()->SetLabelColor(1);
	dopdens->GetYaxis()->SetLabelColor(1);
   	dopdens->GetYaxis()->SetRangeUser(-dMax*1.3,dMax*1.3);
	dopdens->GetXaxis()->SetRangeUser(2.,YMax-1.);
	
   	dopdens->SetLineWidth(3);
	dopdens->SetLineColor(LCol); // set line color to black

	canvaspc->cd(3);
	dopdens->Draw("AL");

	NLine1 = new TLine();
	NLine1->SetLineStyle(2);
	NLine1->DrawLine(0,0, GetYMax(),0);

	std::stringstream ssp;
	std::stringstream ssn;
	ssn << " n-type " ;
	ssp << " p-type " ;
	TLatex ln;
	TLatex lp;
	ln.DrawLatex( 10,1,ssn.str().c_str());

	lp.DrawLatex( 10,-1,ssp.str().c_str());

	delete[] q;
	delete[] dpoty; 
	
}
/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetCutOnStrips() {

  wherecut = (dwpot.GetXMAX()/2+1)*dwpot.GetBinSizex();
  wherecut2 = (dwpot.GetXMAX()/2+1)*dwpot.GetBinSizex();
  WhereCut->SetNumber(wherecut);
  WhereCut2->SetNumber(wherecut2);	
  
  DrawAllGraph(2);
  return;

}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::DrawAll() {

  wherecut = WhereCut->GetNumber(); 
  WhereCut->SetNumber(wherecut);
  WhereCut2->SetNumber(wherecut);	
  DrawAllGraph(1);
  return;

}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::DrawAllw() {

  wherecut2 = WhereCut2->GetNumber(); 
  WhereCut2->SetNumber(wherecut2);
  WhereCut->SetNumber(wherecut2);	
  DrawAllGraph(1);
  return;

}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetCutBetweenStrips() {
  // return;
  wherecut = (dwpot.GetXMAX()/2+1+(int) (dwpot.Getpitch()/2))*dwpot.GetBinSizex();	
  wherecut2 = (dwpot.GetXMAX()/2+1+(int)(dwpot.Getpitch()/2))*dwpot.GetBinSizex();	
	WhereCut->SetNumber(wherecut);
	WhereCut2->SetNumber(wherecut2);

	DrawAllGraph(4);
	return;

}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetBField(Bool_t on) {
	bfieldon=on;
	if(bfieldon) BfieldEntry->SetState(kTRUE);
	else BfieldEntry->SetState(kFALSE);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetDiffusion(Bool_t on) {
	diffusionon=on;
	//	if(diffusionon) TempEntry->SetState(kTRUE);
	// else TempEntry->SetState(kFALSE);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetChargeCloud(Bool_t on) {
	chargecloudon=on;
}

/////////////////////////////////////////////////////////////////////////////////////////////

bool WFGUI::GetChargeCloud() {
	return chargecloudon;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetNA62On(Bool_t State) {
  
  NA62On=State;
  if(NA62On==true) 
    {
      ShTransEntry->SetNumber(75);
      ShNoiseEntry->SetNumber(1.6);
      CSAImpEntry->SetNumber(50);
      //      TRiseEntry->SetState(kFALSE);
      // TFallEntry->SetState(kFALSE);
    }
  //  else
  //  {
  //   TRiseEntry->SetState(kTRUE);
  //    TFallEntry->SetState(kTRUE);
  //  }
}
bool WFGUI::GetNA62On() {

  return NA62On;
}
/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetSC_CSAOn(Bool_t State) {
  
  SC_CSAOn=State;
  if(SC_CSAOn==true) 
    {
      ShTransEntry->SetNumber(6);
      ShNoiseEntry->SetNumber(1.3);
      TRiseEntry->SetNumber(0.4);
      TFallEntry->SetNumber(0.4);
      CSAImpEntry->SetNumber(30);
    }
}
bool WFGUI::GetSC_CSAOn() {

  return SC_CSAOn;
}
/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetOscOn(Bool_t State) {
  OscOn=State;
  if(OscOn==true) 
    {	
      OscBWEntry->SetState(kTRUE);
      CSAImpEntry->SetState(kTRUE);
      CDEntry->SetState(kTRUE);
      LDEntry->SetState(kTRUE);
      //      LDEntry->SetState(kFALSE);
      TRiseEntry->SetState(kTRUE);
      TFallEntry->SetState(kTRUE);
      ShTransEntry->SetState(kTRUE);
      ShNoiseEntry->SetState(kTRUE);
      BBVthEntry->SetState(kTRUE);
      BBNoiseEntry->SetState(kTRUE);
      BBBWEntry->SetState(kTRUE);
      BBGainEntry->SetState(kTRUE);
      BBImpEntry->SetState(kTRUE);
      CSAVthEntry->SetState(kTRUE);
      OscBWLabel->Disable(kFALSE);
      ImpLabel->Disable(kFALSE);
      CDLabel->Disable(kFALSE);
      TRiseLabel->Disable(kFALSE);
      // ShTransLabel->Disable(kFALSE);
      ShNoiseLabel->Disable(kFALSE);
      BBNoiseLabel->Disable(kFALSE);
      BBBWLabel->Disable(kFALSE);
      CSAKind ->SetEnabled(kTRUE);
      
    }
  else 	
    {
      CSAKind ->SetEnabled(kFALSE);
      CDEntry->SetState(kFALSE);
      CDEntry->SetState(kFALSE);
      LDEntry->SetState(kFALSE);
      OscBWEntry->SetState(kFALSE);
      CSAImpEntry->SetState(kFALSE);
      TRiseEntry->SetState(kFALSE);
      TFallEntry->SetState(kFALSE);
      ShTransEntry->SetState(kFALSE);
      ShNoiseEntry->SetState(kFALSE);
      BBVthEntry->SetState(kFALSE);
      BBNoiseEntry->SetState(kFALSE);
      BBBWEntry->SetState(kFALSE);
      BBImpEntry->SetState(kFALSE);
      BBGainEntry->SetState(kFALSE);
      CSAVthEntry->SetState(kFALSE);
      OscBWLabel->Disable(kTRUE);
      ImpLabel->Disable(kTRUE);
      CDLabel->Disable(kTRUE);
      TRiseLabel->Disable(kTRUE);
      // ShTransLabel->Disable(kTRUE);
      ShNoiseLabel->Disable(kTRUE);
      BBNoiseLabel->Disable(kTRUE);
      BBBWLabel->Disable(kTRUE);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetFileNameOn(Bool_t State) {

  SetSampling(SamplingEntry->GetNumber());
  SetPrintSampling(PrintEntry->GetNumber());
  FileNameOn=State;
  // cout << "filename state = " << FileNameOn << endl;
	if(FileNameOn==true) 
	{
	  //	  SetLess2DPlot(kTRUE);
	  FileNameEntry->SetState(kTRUE);
	  FileNameLabel->Disable(kFALSE);
	  PrintEntry->SetState(kTRUE);
	  PrintLabel->Disable(kFALSE);
	}
	else 	
	{
	  //	    SetLess2DPlot(kFALSE);
	    FileNameEntry->SetState(kFALSE);
	    FileNameLabel->Disable(kTRUE);
	    PrintEntry->SetState(kFALSE);
	    PrintLabel->Disable(kTRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetOscBW(double bw) {
  OscBW=bw;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetOscBW() {
  return OscBW;
  //	  OscBWEntry->GetNumber();
//	else OscBWEntry->SetState(kFALSE);
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBBImp(double Imp) {
  BBImp=Imp;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBBImp() {
  return BBImp;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetCSAImp(double Imp) {
  CSAImp=Imp;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetCSAImp() {
  return CSAImp;
  // ImpEntry->GetNumber();
//	else OscBWEntry->SetState(kFALSE);
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetCDet(double CD) {
	CDet=CD;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetCDet() {
  return CDet;

}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetLDet(double LD) {
	LDet=LD;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetLDet() {
  return LDet;

}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetTRise(double tr) {
	TRise=tr;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetTRise() {
	return TRiseEntry->GetNumber();

}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetTFall(double tf) {
	TFall=tf;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetTFall() {
	return TFallEntry->GetNumber();

}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetCSAVth(double vt) {
  //time threshold
  CSAVth=vt;
}

//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetCSAVth() {
  return CSAVth;

}


//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetShTrans() {
  return ShTrans;
}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetShTrans(double vt) {
	ShTrans=vt;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBBVth( double vt) {
	BBVth=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBBVth() {
  return BBVth;

}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBBGain( double vt) {
	BBGain=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBBGain() {
  return BBGain;

}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBBBW(double vt) {
	BBBW=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBBBW() {
  return BBBW;

}



/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetShNoise(double vt) {
	ShNoise=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetShNoise() {
  return ShNoise;

}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBBNoise(double vt) {
	BBNoise=vt;
}

//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBBNoise() {
  return BBNoise;

}

//////////////////////////////////////////////////////////////////////////////////////
// double WFGUI::GetTVth() {
//  return TVth;
// }

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetJitter(double vt) {
	CSAJitter=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetJitter() {
  return fabs(CSAJitter);

}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBBJitter(double vt) {
	BBJitter=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBBJitter() {
  return fabs(BBJitter);

}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetCSATVth(double vt) {
  CSATVth=vt; // Vth Forward time
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetCSATVth() {
  return CSATVth;

}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetCSAFTVth(double vt) {
  CSAFTVth=vt; // Vth Forward time
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetCSAFTVth() {
  return CSAFTVth;

}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBBFTVth(double vt) {
	BBFTVth=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBBFTVth() {
  return BBFTVth;

}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBBTVth(double vt) {
	BBTVth=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBBTVth() {
  return BBTVth;

}


/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetFJitter(double vt) {
	CSAFJitter=vt;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetFJitter() {
  return fabs(CSAFJitter);

}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetSWidth(double tf) {
	SWidth= tf;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetSWidth() {
  return SWidth;

}

/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetOscOn() {
	return OscOn;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetFileNameOn() {
	return FileNameOn;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBatchOn(Bool_t on) {
	BatchOn=on;
    if(BatchOn) {
        EventsEntry->SetState(kTRUE);
	EventsEntryStart->SetState(kTRUE);
        EventsLabel->Disable(kFALSE);
	EventsLabel2->Disable(kFALSE);
    }
    else {
        EventsEntry->SetState(kFALSE);
	EventsEntryStart->SetState(kFALSE);
        EventsLabel->Disable(kTRUE);
	EventsLabel2->Disable(kTRUE);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetBatchOn() {
	return BatchOn;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBatchRandomOn(Bool_t on) {
	BatchRandomOn=on;
    if (BatchRandomOn==true){
        CarriersAngleNumberentry->SetState(kFALSE);
        CarriersInNumberentry->SetState(kFALSE);
        CarriersInLabel->Disable(kTRUE);
    }
    else{
        CarriersAngleNumberentry->SetState(kTRUE);
        CarriersInNumberentry->SetState(kTRUE);
        CarriersInLabel->Disable(kFALSE);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetBatchRandomOn() {
	return BatchRandomOn;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetPrecisionGain(Bool_t on) {
     
  PrecisionGain=on;
  if (on) cout << "High precision gain selected: the computational grid is finer" << endl;
  else cout << "High precision gain deselected" << endl;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetPrecisionGain() {
	return PrecisionGain;
}

void WFGUI::SetWF(Bool_t on) {
  if (on){
    SaveFileNameWF ->SetState(kTRUE);
  }
  else
    SaveFileNameWF ->SetState(kFALSE);
    //GainLabel ->Disable(kFALSE);
  return;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetScreenedGain(Bool_t on) {

  if (on){
    ScreenedGainEntry ->SetState(kTRUE);
    //GainLabel ->Disable(kFALSE);
    std::cout << "Enabling gain screening by charge cloud" << std::endl;
  }
  else
    {
      ScreenedGainEntry ->SetState(kFALSE);
      std::cout << "Disabling gain screening by charge cloud" << std::endl;
    }
  ScreenedGain = on;

}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetScreenedGain() {
    return ScreenedGain;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetForceGain(Bool_t on) {
  if (on){
    Gainentry ->SetState(kTRUE);
    //GainLabel ->Disable(kFALSE);
    IrradiationEntry ->SetState(kFALSE);
    IrradiationEntry2 ->SetState(kFALSE);
    IrradiationEntry3 ->SetState(kFALSE);
    //IrradiationLabel->Disable(kTRUE);
    //IrradiationLabel2->Disable(kTRUE);
    // IrradiationLabel3->Disable(kTRUE);
    IrradiationOnButton ->SetOn(kFALSE, kFALSE);
    IrradiationOnButton ->SetEnabled(kFALSE);
    AcceptorCreationButton ->SetOn(kFALSE, kFALSE);
    AcceptorCreationButton ->SetEnabled(kFALSE);
    InitialDopRemovalButton ->SetOn(kFALSE, kFALSE);
    InitialDopRemovalButton ->SetEnabled(kFALSE);
    AllOnButton ->SetOn(kFALSE, kFALSE);
    AllOnButton ->SetEnabled(kFALSE);
    GainQuenchingButton ->SetOn(kFALSE, kFALSE);
    GainQuenchingButton ->SetEnabled(kFALSE);
    
    
    DJEntry ->SetState(kFALSE);
    DJehEntry ->SetState(kFALSE);
    DJLabel->Disable(kTRUE);
    DJehLabel->Disable(kTRUE);
    DJOnButton ->SetOn(kFALSE, kFALSE);
    DJOnButton ->SetEnabled(kFALSE);
    
    IrradiationOnButton ->SetTextColor(1, kFALSE);
  }
  else
    {
      Gainentry ->SetState(kFALSE);
      //GainLabel ->Disable(kTRUE);
      IrradiationOnButton ->SetOn(kTRUE, kTRUE);
      IrradiationOnButton ->SetEnabled(kTRUE);
      IrradiationOnButton->SetTextColor(1, kFALSE);
      DJOnButton ->SetOn(kTRUE, kTRUE);
      DJOnButton ->SetEnabled(kTRUE);
      AcceptorCreationButton ->SetOn(kTRUE, kTRUE);
      AcceptorCreationButton ->SetEnabled(kTRUE);
      InitialDopRemovalButton ->SetOn(kTRUE, kTRUE);
      InitialDopRemovalButton ->SetEnabled(kTRUE);
      AllOnButton ->SetOn(kTRUE, kTRUE);
      AllOnButton ->SetEnabled(kTRUE);
      GainQuenchingButton ->SetOn(kTRUE, kTRUE);
      GainQuenchingButton ->SetEnabled(kTRUE);

      //      DJEntry ->SetState(kTRUE);
      // DJehEntry ->SetState(kTRUE);
      // DJOnButton->SetTextColor(1, kFALSE);
      // DJLabel->Disable(kFALSE);
      // DJehLabel->Disable(kFALSE);

    }
  ForceGain=on;
}
 /////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetAC(Bool_t on) {
  if (on){
    cout << "AC readout selected"<< endl;
    //ACEntry ->SetState(kTRUE);
    //ACValue = ACEntry->GetNumber();
    //SamplingEntry->SetNumber(1.);
    
  }
  else
    {
      cout << "DC readout selected"<< endl;
      // ACEntry ->SetState(kFALSE);
      ACValue = 0;
      //      SamplingEntry->SetNumber(0.1);
    }
  AC = on;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetForceGain() {
    return ForceGain;
}
 /////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetAC() {
    return AC;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetCCEOn() {
	return CCEOn;
}

/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetAcceptorCreation() {
	return AcceptorCreation;
}

/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetLnAcceptorCreation() {
	return LnAcceptorCreation;
}

/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetGainQuenching() {
	return GainQuenching;
}


/////////////////////////////////5///////////////////////////////////////////////////////////
bool WFGUI::GetInitialDopRemoval() {
	return InitialDopRemoval;
}

/////////////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetDJOn() {
	return DJOn;
}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetCCEOn(Bool_t on) {
  int CalcButtonState = CalculateButton->GetState();
  CCEOn=on;
  //  Bool_t Yes = ;
  if (CCEOn == true)
    {
      //      SetAllOn(true);


        IrradiationEntry ->SetState(kTRUE);
        IrradiationEntry2 ->SetState(kTRUE);
        IrradiationEntry3 ->SetState(kTRUE);
        IrradiationOnButton->SetTextColor(1, kFALSE);
        ForceGain = false;
        ForceGainButton ->SetOn(kFALSE, kFALSE);
        ForceGainButton ->SetEnabled(kFALSE);
        //IrradiationLabel->Disable(kFALSE);
        //IrradiationLabel2->Disable(kFALSE);
	// IrradiationLabel3->Disable(kFALSE);
        SetFluence(IrradiationEntry->GetNumber());
        SetBetaElectrons(IrradiationEntry2->GetNumber());
        SetBetaHoles(IrradiationEntry3->GetNumber());
	// set all on //
  /*			      
	//	AcceptorCreationButton->SetDown(1,0);
	//SetAcceptorCreation(1);
	
	//InitialDopRemovalButton->SetDown(1,0);
	//SetInitialDopRemoval(1);

	//LnAcceptorCreationButton->SetDown(1,0);
	//SetLnAcceptorCreation(1);

	//GainQuenchingButton->SetDown(1,0);
	//SetGainQuenching(1);
	

  */
    }
    else
      {
	if (!GetInitialDopRemoval() && !GetAcceptorCreation() && !GetGainQuenching() )
	  {
	  IrradiationEntry ->SetState(kFALSE);
	  SetFluence(0);
	  //IrradiationLabel->Disable(kTRUE);
	  //IrradiationLabel3->Disable(kTRUE);
	  }

        IrradiationEntry2 ->SetState(kFALSE);
        IrradiationEntry3 ->SetState(kFALSE);
	if (!GetDJOn()) ForceGainButton ->SetEnabled(kTRUE);
	//	if (!GetDJOn()) ForceGainButton->SetTextColor(1, kFALSE);
	//        IrradiationEntry->SetNumber(0);
	//        IrradiationLabel->Disable(kTRUE);
	// IrradiationLabel2->Disable(kTRUE);
        //IrradiationLabel3->Disable(kTRUE);
    }
    if (CalcButtonState ==0)
      {
	CalculateButton->SetEnabled(kTRUE);
	CalcPotButton->SetEnabled(kTRUE);
	CalcPotButton->SetTextColor(1,kFALSE);
      }
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetAllOn(Bool_t on) {
    AllOn=on;
    if (AllOn == true){

      	// set all on //

      IrradiationOnButton->SetDown(1,0);
      SetCCEOn(1);
      
      AcceptorCreationButton->SetDown(1,0);
      SetAcceptorCreation(1);
      
      InitialDopRemovalButton->SetDown(1,0);
      SetInitialDopRemoval(1);
      
      LnAcceptorCreationButton->SetDown(1,0);
      SetLnAcceptorCreation(1);
      
      GainQuenchingButton->SetDown(1,0);
      SetGainQuenching(1);
      
    }
    else
      {
	IrradiationOnButton->SetDown(0,0);
	SetCCEOn(0);
	
	AcceptorCreationButton->SetDown(0,0);
	SetAcceptorCreation(0);
	
	InitialDopRemovalButton->SetDown(0,0);
	SetInitialDopRemoval(0);
	
	LnAcceptorCreationButton->SetDown(0,0);
	SetLnAcceptorCreation(0);
	
	GainQuenchingButton->SetDown(0,0);
	SetGainQuenching(0);
	
    }
}
////////////////////////////////////////

////////////////////////////////////////
void WFGUI::SetGainQuenching(Bool_t on) {
  int CalcButtonState = CalculateButton->GetState();
  //  CalculateButton->SetEnabled(kFALSE);
  GainQuenching=on;
      if (on == true){
        IrradiationEntry ->SetState(kTRUE);
	QuenchEntry ->SetState(kTRUE);
        //IrradiationLabel->Disable(kFALSE);scarb
	//	IrradiationLabel3->Disable(kFALSE);
        SetFluence(IrradiationEntry->GetNumber());
	SetQuenchCoef(QuenchEntry->GetNumber());
    }
    else
      {
	QuenchEntry ->SetState(kFALSE);
	SetQuenchCoef(0);
	if (!GetInitialDopRemoval() && !GetCCEOn() && !GetAcceptorCreation())
	  {
	  IrradiationEntry ->SetState(kFALSE);
	  SetFluence(0);
	  //IrradiationLabel->Disable(kTRUE);
	  //IrradiationLabel3->Disable(kTRUE);
	  
	  }
      }
      
      if (CalcButtonState ==0)
	{
	  CalculateButton->SetEnabled(kTRUE);
	  CalcPotButton->SetEnabled(kTRUE);
	  CalcPotButton->SetTextColor(1,kFALSE);
	}
      //     
      // CalcPotButton->SetEnabled(kTRUE);
      // CalcPotButton->SetTextColor(1,kFALSE);
  
}
////////////////////////////////////////
void WFGUI::SetAcceptorCreation(Bool_t on) {
  CalculateButton->SetEnabled(kFALSE);
  AcceptorCreation=on;
      if (on == true){
        IrradiationEntry ->SetState(kTRUE);
        //IrradiationLabel->Disable(kFALSE);
	//IrradiationLabel3->Disable(kFALSE);
        SetFluence(IrradiationEntry->GetNumber());
    }
    else
      {
	if (!GetInitialDopRemoval() && !GetCCEOn()  && !GetGainQuenching() )
	  {
	  IrradiationEntry ->SetState(kFALSE);
	  //IrradiationLabel->Disable(kTRUE);
	  //IrradiationLabel3->Disable(kTRUE);
	  SetFluence(0);
	  SetLnAcceptorCreation(0);
	  LnAcceptorCreationButton->SetState(kButtonUp);
	  }
    }
  
}

////////////////////////////////////////
void WFGUI::SetLnAcceptorCreation(Bool_t on) {
  CalculateButton->SetEnabled(kFALSE);
  LnAcceptorCreation=on;
      if (on == true){
        IrradiationEntry ->SetState(kTRUE);
        //IrradiationLabel->Disable(kFALSE);
	//IrradiationLabel3->Disable(kFALSE);
        SetFluence(IrradiationEntry->GetNumber());
	AcceptorCreationButton->SetState(kButtonDown);
	SetAcceptorCreation(1);

    }
    else
      {
	if (!GetInitialDopRemoval() && !GetCCEOn()  && !GetGainQuenching() &&  !GetAcceptorCreation() )
	  {
	  IrradiationEntry ->SetState(kFALSE);
	  //IrradiationLabel->Disable(kTRUE);
	  //IrradiationLabel3->Disable(kTRUE);
	  SetFluence(0);
	  }
    }
  
}


////////////////////////////////////////
void WFGUI::SetInitialDopRemoval(Bool_t on) {
  CalculateButton->SetEnabled(kFALSE);
  InitialDopRemoval=on;
      if (on == true){
        IrradiationEntry ->SetState(kTRUE);
        //IrradiationLabel->Disable(kFALSE);
	//IrradiationLabel3->Disable(kFALSE);
        SetFluence(IrradiationEntry->GetNumber());
    }
    else
      {
	if (!GetAcceptorCreation() && !GetCCEOn()  && !GetGainQuenching())
	  {
	  IrradiationEntry ->SetState(kFALSE);
	  SetFluence(0);
	  //IrradiationLabel->Disable(kTRUE);
	  //IrradiationLabel3->Disable(kTRUE);
	  }
    }
}

////////////////////////////////////////
void WFGUI::SetDJOn(Bool_t on) {
  CalculateButton->SetEnabled(kFALSE);
  DJOn=on;
  if (DJOn == true){
 

    
    DJEntry ->SetState(kTRUE);
    DJehEntry ->SetState(kTRUE);
    DJOnButton->SetTextColor(1, kFALSE);
    ForceGain = false;
    ForceGainButton ->SetOn(kFALSE, kFALSE);
    ForceGainButton ->SetEnabled(kFALSE);
    DJLabel->Disable(kFALSE);
    DJehLabel->Disable(kFALSE);
    SetDJValue(DJEntry->GetNumber());
    SetDJehValue(DJEntry->GetNumber());

  }
  else{

    DJEntry ->SetState(kFALSE);
    DJehEntry ->SetState(kFALSE);
    if (!GetCCEOn()) ForceGainButton ->SetEnabled(kTRUE);
    // if (!GetCCEOn()) ForceGainButton->SetTextColor(1, kFALSE);
    DJLabel->Disable(kTRUE);
    DJehLabel->Disable(kTRUE);

  }
}
/////////////////////////////////////////////////////////////////////////////////////////////
TH2F* WFGUI::Getwhist() {
	return whist;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TH2F* WFGUI::Getdhist() {
	return dhist;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TH2F* WFGUI::GetExhist() {
	return Exhist;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TH1F* WFGUI::GetEnhist() {
	return Enhist;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TH1F* WFGUI::GetMeasEnhist() {
	return MeasEnhist;
}
////////////////////////////////////////
double WFGUI::GetGainvalue( double k, int charge, int GainKind) {
  // k = E field in [V/m], this is the gain/micron //Impact ionization
  //  double Galpha = 330000;
  // double Gbeta = 1170000;
  // Sentaurus Manual pag 345
  //  int GainKind =  GetGainKind();
  // double CoefCor = 1;
  k *= 0.01; // V/cm 
 
  double gain = 0;
  
  if (GainKind == 1) //van Ost
    {
      double gamma = 1.;
      double Galpha = 0; //cm^-1
      double Gbeta = 0; // V/cm
      double hw = 0.063;
      double kT = 0.0257; // eV with T = 298
      double T0 = 298.;
      gamma = 1.*tanh(hw/(2*kT))/tanh(hw/(2*kT*TempEntry->GetNumber()/T0));
      
      if (charge == -1) 
	{
	  Galpha = 703000; //cm^-1
	  Gbeta = 1231000; // V/cm
	  //      cout << " TCAD model = " << gamma*Gbeta << " " ;
	}
      else if (charge ==+1)
	{
	  //      Galpha = 671000; //cm^-1
	  // Gbeta  = 1693000; // V/cm
	  // Galpha = 703000; //cm^-1
	  // Gbeta = 1231000; // V/cm
	  Galpha = 1582000; //cm^-1
	  Gbeta  = 2036000; // V/cm
	}
      
      
      //  gamma = 1.;
      // cout << "gamma = " << gamma << endl; 
      
      
      // gain = 1./(1.-Galpha*exp(-Gbeta/(k*1000.))*0.0001);
      gain = gamma*Galpha*exp(-(gamma*Gbeta)/(k))*100; //gain/meter
      // if (k>320) gain = 146.1-1.073*k+0.002*k*k;;
      // cout << " gain = " << gain << endl;
    }
  else if (GainKind == 2) //  Massey
    {
      //    double aexp = -1.18E-06;
      // double bexp = 1.18;
      // double gexp =  aexp*k+bexp ; // this parameter makes the growth less violent with E: higher at low field, lower at high field

      // double aga = -2.1E-06;
      // double bga = 1.25;
      //  double gamma =  aga*k+bga ; // this parameter makes the growth less violent with E: higher at low field, lower at high field


      double aexp = -0.382;
      double bexp = 5.58;
      double gexp =  aexp*log(k)+bexp ; // this parameter makes the growth less violent with E: higher at low field, lower at high field

      
      double aga = -0.577;
      double bga = 7.8;
      double gamma =  aga*log(k)+bga ; // this parameter makes the growth less violent with E: higher at low field, lower at high field

      double CoefFluence = 0; 
      if (GetGainQuenching()) CoefFluence = Quench;
      
      // Old
      //      double GC = 966000; // V*cm^-1 default: 966000 
      // double GD = 499; // V*cm^1K^1
      // double Galpha = 443000; //cm^-1

      // New
      double GC = 841250; // V*cm^-1 default: 966000 
      double GD = 998; // V*cm^1K^1
      double Galpha = 443000; //cm^-1


	  //	  cout << " new model = " << Gbeta << endl ;
     
      if (charge ==+1)
	{
	  
	  GC = 1710000; // V*cm^-1  1710000
	  GD = 1090; // V*cm^1K^1
	  Galpha = 1130000; //cm^-1
	  gexp = 1;
	  gamma = 1;
	  
	}

      
      // k = V/cm-1

      double Gbeta = GC+GD*TempEntry->GetNumber()+CoefFluence*GetFluence() ; // V/cm critical field
      gain = gamma*Galpha*exp(-(gexp*Gbeta)/(k))*100; //gain/meter
      //      cout << " gamma,1 = " << gamma << ", " << gamma1 << " gexp,1 = " << gexp << " ," << gexp1 << " field " << k << " 1./gain " << 1./gain <<  endl;

      

    }
  else if (GainKind == 3) //  Okudo
    {
      double T0 = 298.;
      double a = 0.8; // V^-1 0.426
      double b = 4.81e5; // V*cm^-1
      double c =3.05e-4; //K^-1
      double d = 6.86e-4; // K^-1
      double delta = 2; // Okuto
      if (charge ==+1)
	{
	  a = 0.3; // V^-1 0.24
	  b = 6.53e5; // V*cm^-1
	  c = 5.35e-4; //K^-1
	  d = 5.67e-4; // K^-1
	  delta = 2;
	}
      
      double DT = (TempEntry->GetNumber()-T0);
      gain = a*(1+c*DT)*k*pow(exp(-(b*(1+d*DT)/k)),delta)*100; //gain/meter

    }

  else if (GainKind == 4) //  Bologna
    {
      double a0 = 0;   double a1 = 0;  double a2 = 0; double b0 = 0;  double b1 = 0;
      double c0 = 0;double c1 = 0;double c2 = 0;  double d0 = 0;double d1 = 0;double d2 = 0;
      double aT = 0; double bT = 0; double cT= 0 ; double dT= 0; // Bologna
      double T = TempEntry->GetNumber();
      if (charge == -1) 
	{	  
	  a0 = 4.3383; // V
	  a1 = -2.42e-12; // V
	  a2 = 4.1233; // 1
	  b0 = 0.235; // V
	  b1 = 0; // 1
	  c0 = 1.68e4; //Vcm-1
	  c1 = 4.3796; //Vcm-1
	  c2 = 0.13; //Vcm^-1
	  d0 = 1.233e6; // Vcm-1
	  d1 = 1.2039e3; // Vcm-1
	  d2 = 0.567; // Vcm-1

	  aT = a0+a1*pow(T,a2);
	  bT = b0;
	  cT = c0+c1*T+c2*T*T;
	  dT = d0+d1*T+d2*T*T;

	  //	  cout << " new model = " << Gbeta << endl ;
	}
      else if (charge ==+1)
	{
	  a0 = 2.376; // V
	  a1 = 1.033e-2; // V
	  a2 = 0; // 1
	  b0 = 0.177; // V
	  b1 = -2.178e-3; // 1
	  c0 = 9.47e-3; //Vcm-1
	  c1 = 2.49; //Vcm-1
	  c2 = 0; //Vcm^-1
	  d0 = 1.4043e6; // Vcm-1
	  d1 = 2.9744e3; // Vcm-1
	  d2 = 1.48; // Vcm-1

	  aT = a0+a1*T;
	  bT = b0*exp(b1*T);
	  cT = c0*pow(T,c1);
	  dT = d0+d1*T+d2*T*T;
	}
      

      gain = k/(aT+bT*exp(dT/(k+cT)))*100; // gain/meter
  
    }
   else if (GainKind == 5) //  Massey optimized 
    {
     
       double CoefFluence = 0; 
      if (GetGainQuenching()) CoefFluence = Quench;
      
      // Old
      //      double GC = 966000; // V*cm^-1 default: 966000 
      // double GD = 499; // V*cm^1K^1
      // double Galpha = 443000; //cm^-1

      // New
      double GC = 1020000; // V*cm^-1 default: 966000 
      double GD = 1040; // V*cm^1K^1  default: 499;
      double Galpha = 1186000; // 443000; //cm^-1


	  //	  cout << " new model = " << Gbeta << endl ;
     
      if (charge ==+1)
	{
	  
	  GC = 1851000; // V*cm^-1  1710000
	  GD = 1828; // V*cm^1K^1 1090
	  Galpha = 2250000; //cm^-1

	  
	}

      
      // k = V/cm-1

      double Gbeta = GC+GD*TempEntry->GetNumber()+CoefFluence*GetFluence() ; // V/cm critical field
      gain = Galpha*exp(-(Gbeta)/(k))*100; //gain/meter
      //      cout << " gamma,1 = " << gamma << ", " << gamma1 << " gexp,1 = " << gexp << " ," << gexp1 << " field " << k << " 1./gain " << 1./gain <<  endl;

      

    }
   else if  (GainKind == 6) //van Ost
    {
      double gamma = 1.;
      double Galpha = 0; //cm^-1
      double Gbeta = 0; // V/cm
      double hw = 0.063;
      double kT = 0.0257; // eV with T = 298
      double T0 = 298.;
      gamma = 1.*tanh(hw/(2*kT))/tanh(hw/(2*kT*TempEntry->GetNumber()/T0));
      
      if (charge == -1) 
	{
	  Galpha = 1149000;// 703000; //cm^-1
	  Gbeta = 1325000;// 1231000; // V/cm
	}
      else if (charge ==+1)
	{

	  Galpha = 2519000;//1582000; //cm^-1
	  Gbeta  = 2428000;//2036000; // V/cm
	}
      
      gain = gamma*Galpha*exp(-(gamma*Gbeta)/(k))*100; //gain/meter

    }
    

  return gain;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TH2F* WFGUI::Getchist() {
	return chist;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TH2F* WFGUI::Getchhist() {
	return chhist;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TH2F* WFGUI::Getctothist() {
	return ctothist;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TLine* WFGUI::GetNLine1() {
	return NLine1;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TEllipse* WFGUI::GetEllipse() {
	return ellipse;
}

/////////////////////////////////////////////////////////////////////////////////////////////
TLine* WFGUI::GetNLine2() {
	return NLine2;
}

/////////////////////////////////////////////////////////////////////////////////////////////
WFGUI::~WFGUI() {
	Cleanup();
	
	
// 	for (int i = 0; i < (dwpot.GetYMAX()); i++) {
// 	   if (df[i] != NULL) delete[] df[i];
// 	}
// 	delete[] df;
// 	
// 	for (int i = 0; i < (dwpot.GetYMAX()); i++){
// 	   if (wf[i] != NULL) delete[] wf[i];
// 	}
// 
//         delete[] wf;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::CloseWindow() // Got close message for this MainFrame. Terminates the application.
{
  SetStopOn();
  gApplication->Terminate();
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::LoadData(){

  string fname;// "sensors/data/";
  Eflag = 0;      //Eflag initialization
  string fnamed;

  // if (WFButton->GetState())
  // {
  //   fnamed = SaveFileNameWF->GetText();
  //  }
  // else
  fnamed =SaveFileName->GetText();
  
  std::string delimiter = ".";
	
  if(fnamed.find(delimiter)>100 || fnamed.find(delimiter)<1)  fnamed +=".dat";

   delimiter = "ta/";
   if (fnamed.find(delimiter) != 10)  fname = "sensors/data/"+fnamed;
   else fname = fnamed;

  std::map<std::string, double> valueMap = GetParameters(fname.c_str());

  // partial loading for now

  ifstream ifile(fname.c_str());
  if (ifile)
    {
      std::cout << "Loading data from file " << fname.c_str() << std::endl;
      //    SetGainDoping(valueMap["DOPING_GL"]); //call callsetdopingGL
      //  SetGainShape(valueMap["SHAPE_GL"]);  //call setgaindoping and setgainkind
      // SetGainKind(valueMap["KIND_GL"]); //call noone
      
      GainKind->Select(valueMap["KIND_GL"]); 
      GainDoping->Select(valueMap["DOPING_GL"]);
      GainShape->Select(valueMap["SHAPE_GL"]);
      Dopingentry->SetNumber(valueMap["DOP_LEV"]);
      Dopingentry->SetNumber(valueMap["DOP_LEV"]);
      Biasentry->SetNumber(valueMap["BIAS_VOLTAGE"]);
      Depletionentry->SetNumber(valueMap["DEPL_VOLTAGE"]);
      YMAXentry->SetNumber(valueMap["DETECT_HEIGHT"]);
      XMAXentry->SetNumber(valueMap["STRIP_NUMB"]); 
      Pitchentry->SetNumber(valueMap["STR_PITCH"]); 
      Widthentry->SetNumber(valueMap["STR_WIDTH"]);
      TempEntry->SetNumber(valueMap["TEMPERATURE"]);
      IrradiationEntry2->SetNumber(valueMap["BETA_ELECTRONS"]);
      IrradiationEntry3->SetNumber(valueMap["BETA_HOLES"]);
      QuenchEntry->SetNumber(valueMap["QUENCH"]);
      StepxEntry->SetNumber(valueMap["STEPX"]);
      StepyEntry->SetNumber(valueMap["STEPY"]);

      // electronics
      CDEntry->SetNumber(valueMap["CAPACITANCE"]);
      LDEntry->SetNumber(valueMap["INDUCTANCE"]);
      CSAImpEntry->SetNumber(valueMap["IMPEDANCE"]);
      OscBWEntry->SetNumber(valueMap["OSCOPE_BW"]);
      TRiseEntry->SetNumber(valueMap["SHPR_INT_TIME"]); 
      TFallEntry->SetNumber(valueMap["SHPR_DCY_TIME"]);
      ShTransEntry->SetNumber(valueMap["SHPR_TRANS"]);
      ShNoiseEntry->SetNumber(valueMap["SHPR_NOISE"]);
      CSAVthEntry->SetNumber(valueMap["VTH"]);
      BBBWEntry->SetNumber(valueMap["BBBW"]);
      BBGainEntry->SetNumber(valueMap["BBGAIN"]);
      BBVthEntry->SetNumber(valueMap["BBVTH"]);
      BBNoiseEntry->SetNumber(valueMap["BB_NOISE"]);
      BBImpEntry->SetNumber(valueMap["BB_IMP"]);
      
      
      LoadButton->SetBackgroundColor(0x00ff00);
      LoadButton->SetTitle("Load");
      
      
    }
  else
    {
      LoadButton->SetBackgroundColor(0xff0000);
      LoadButton->SetTitle("?");      
    }

  
  string fnameg = "sensors/graph/";
  string fnameg1 = SaveFileName->GetText();


   delimiter = "ta/";
   // size_t pos = 0;
  if (fnameg1.find(delimiter) == 10)  fnameg1.erase(0, fnameg1.find(delimiter) + delimiter.length());
  //  fnameg1.erase(0, fnameg1.find(delimiter) + delimiter.length());
  fnameg+=fnameg1;  
  delimiter = ".";
   if(fnameg.find(delimiter)<100 && fnameg.find(delimiter)>1) fnameg.erase(fnameg.find(delimiter), fnameg.length());
  fnameg +=".root";
  // myfile.open(fname, ios::out);
  ifstream ifileg(fnameg.c_str());
  TFile* file_open = TFile::Open(fnameg.c_str());
  TFile* file_openWF;
  
  if (WFButton->GetState())
    {
      string fnamegWF = "sensors/graph/";
      string fnameg1WF = SaveFileNameWF->GetText();
      
      
      delimiter = "ta/";
      // size_t pos = 0;
      if (fnameg1WF.find(delimiter) == 10)  fnameg1WF.erase(0, fnameg1WF.find(delimiter) + delimiter.length());
      //  fnameg1.erase(0, fnameg1.find(delimiter) + delimiter.length());
      fnamegWF+=fnameg1WF;  
      delimiter = ".";
      if(fnamegWF.find(delimiter)<100 && fnamegWF.find(delimiter)>1) fnamegWF.erase(fnamegWF.find(delimiter), fnamegWF.length());
      fnamegWF +=".root";
      // myfile.open(fname, ios::out);
      ifstream ifilegWF(fnameg.c_str());
      
      file_openWF = TFile::Open(fnamegWF.c_str());
      cout << "Opening file for WF = " << fnamegWF.c_str() << endl;
    }
  else
    {
    file_openWF = TFile::Open(fnameg.c_str());
    }
  cout << "Opening file = " << fnameg.c_str() << endl;
  
  if (file_open !=0)
    {
      
      //      printf("File opened successfully, loading potentials and fields\n");

      TH2* readThisd = 0;
      TH2* readThisw = 0;
      
      
      file_open->GetObject("dhist1", readThisd);
      file_openWF->GetObject("whist1", readThisw);

      if( GetGainDoping()>0)
	{
	  //	  SetStepy(0.011);
	  // StepyEntry->SetNumber(0.011);
	}
      
      CallBoundaryConditions();

      cout << "Reading file " << fnameg.c_str()  << ": Number of bins in the histogram potential (x,y): " << readThisd->GetNbinsX() << " , " << readThisd->GetNbinsY() << endl;
      
      dwpot.SetPitchWidthXY(this, GetYMax(),GetNStrips(),Pitchentry->GetNumber(),Widthentry->GetNumber(), GetStepx(), GetStepy() );	    
      
      if (readThisd->GetNbinsX() ==  dwpot.GetXMAX() && readThisd->GetNbinsY() ==  dwpot.GetYMAX())
	{
	  for(int i=0;i<dwpot.GetXMAX();i++)
	    { // fill histogram to show the boundary conditions
	      for(int j=0;j<dwpot.GetYMAX();j++)
		{
		  dwpot.SetdPotential(j,i,readThisd->GetBinContent(i+1,j+1));
		  dwpot.SetwPotential(j,i,readThisw->GetBinContent(i+1,j+1));
		  
		  
		  //	    whist->SetBinContent(i+1,j+1,dwpot.Getwpot(j,i));
		}	      
	    }
	  //      cout << " reading i = 20 ,j  = 20 dhist1 = " <<  readThisd->GetBinContent(20,20) << " dwpot.Getdpot(Ypos,Xpos) " << dwpot.Getdpot(20,20) << endl;
	  CallCalculateFields(); 			// calculate electric field
	  FillHist();
	  DrawAllGraph(1);
	  SetAllButton(1);
	}
      else
	{
	  cout <<  "Root and potentials have different bins, cannot load the potential files"<< endl;
	  cout <<  "Number of bins in the file potential (x,y): " << dwpot.GetXMAX() << " , " << dwpot.GetYMAX() << endl;
	}
    }
  else
    {
      cout <<  "File " << fnameg.c_str()  << " does not exist"<< endl;      
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SaveData(){

  
  UserValues["BBBW"] = BBBWEntry->GetNumber();
  UserValues["BBGAIN"] = BBGainEntry->GetNumber();
  UserValues["BBVTH"] = BBVthEntry->GetNumber(); 
  UserValues["BB_IMP"] = BBImpEntry->GetNumber(); 
  UserValues["BB_NOISE"] = BBNoiseEntry->GetNumber();
  UserValues["BETA_ELECTRONS"] = IrradiationEntry2->GetNumber(); 
  UserValues["BETA_HOLES"] = IrradiationEntry3->GetNumber(); 
  UserValues["BIAS_VOLTAGE"] = Biasentry->GetNumber(); 
  UserValues["CALIB"] = GetCalibEntry();
  UserValues["CAPACITANCE"] = CDEntry->GetNumber();
  UserValues["DEPL_VOLTAGE"] = Depletionentry->GetNumber();
  UserValues["DETECT_HEIGHT"] = YMAXentry->GetNumber();
  UserValues["DOP_LEV"] = Dopingentry->GetNumber(); 
  UserValues["DOUBLEJUNCTION"] = DJEntry->GetNumber(); 
  UserValues["GAIN_LYR_RSS"] = GainIndententry->GetNumber(); 
  UserValues["GAIN_SCL"] = Gainentry->GetNumber();
  UserValues["SGAIN_SCL"] = ScreenedGainEntry->GetNumber();
  
  //  UserValues["AC"] = ACEntry->GetNumber();
  
  UserValues["HE_GAIN_RAT"] = 0; 
  UserValues["IMPEDANCE"] = CSAImpEntry->GetNumber();
  UserValues["INDUCTANCE"] = LDEntry->GetNumber();
  UserValues["IRRADIATION"] = IrradiationEntry->GetNumber();
  UserValues["NA_OVER_ND"] = DJehEntry->GetNumber(); 
  UserValues["NUMBERP"] = NumberEntry->GetNumber(); 
  UserValues["OSCOPE_BW"] = OscBWEntry->GetNumber(); 
  UserValues["PRECISION"] = PrecisionEntry->GetNumber(); 
  UserValues["SAMPLING"] = SamplingEntry->GetNumber(); 
  UserValues["SET_RANGE"] = 10;
  UserValues["SHPR_DCY_TIME"] = TFallEntry->GetNumber(); 
  UserValues["SHPR_INT_TIME"] = TRiseEntry->GetNumber(); 
  UserValues["SHPR_NOISE"] = ShNoiseEntry->GetNumber(); 
  UserValues["SHPR_TRANS"] = ShTransEntry->GetNumber(); 
  UserValues["STEPX"] = StepxEntry->GetNumber(); 
  UserValues["STEPY"] = StepyEntry->GetNumber(); 
  UserValues["STRIP_NUMB"] = XMAXentry->GetNumber(); 
  UserValues["STR_PITCH"] = Pitchentry->GetNumber(); 
  UserValues["STR_WIDTH"] = Widthentry->GetNumber();
  UserValues["TEMPERATURE"] = TempEntry->GetNumber(); 
  UserValues["USERQ"] = GetMPV(); 
  UserValues["VTH"] = CSAVthEntry->GetNumber(); 
  UserValues["YPOSITION"] = EdgeNumberentry->GetNumber();
  UserValues["BETA_ELECTRONS"] =  IrradiationEntry2->GetNumber();
  UserValues["BETA_HOLES"] =  IrradiationEntry3->GetNumber();
  UserValues["QUENCH"] =  QuenchEntry->GetNumber();

  UserValues["KIND_GL"] =GetGainKind();
  UserValues["DOPING_GL"] =NGainDoping;
  UserValues["SHAPE_GL"] =GetGainShape();
  //  UserValues["AC"] =ACEntry->GetNumber();  



  
  map<string, double>::iterator it;

  std::ofstream myfile;
  string fname;
  string fnamed =SaveFileName->GetText();

  
  std::string delimiter = "ta/";
  if (fnamed.find(delimiter) != 10)
    {
      fname ="sensors/data/"+fnamed;  
    }
  else
    {
      fname = fnamed;  
    }


  delimiter = ".";
  if(fname.find(delimiter)>100 || fname.find(delimiter)<1)  fname +=".dat";

  cout << "Saving data in "<< fname.c_str() << std::endl;

  
 
  myfile.open(fname, ios::out);
  
  ifstream ifile(fname.c_str());
  
  if (ifile) 
    {
      if (Eflag !=1 )
	{
	  //	  cout << "The file exists, overwrite?" << endl;
	  SaveButton->SetBackgroundColor(0xff0000);
	  SaveButton->SetTitle("sure?");
	  
	  Eflag = 1;
	  return;
	} 
      else if (Eflag == 1)
	{
	  Eflag = 0;
	  SaveButton->SetBackgroundColor(0x00ff00);
	  SaveButton->SetTitle("Save");
	  std::cout << "Overwriting" << std::endl; 
	  
	}

      for (it = UserValues.begin(); it!=UserValues.end(); it++){
	myfile << it->first << "  " << it->second <<  "\n";
	myfile.flush();
      }
      std::cout << "Saving data in " << fname.c_str() << std::endl;	
      myfile.flush();
      myfile.close();
      
    }
  else
    {
      cout << "The directory does not exists" << endl;
      cout << "Data not saved" << endl;
      SaveButton->SetBackgroundColor(0xff0000);
      SaveButton->SetTitle("XXX");
      return;
    }

  string fnameg1 = SaveFileName->GetText();

  delimiter = "ta/";


  string fnameg = "sensors/graph/";

  if (fnameg1.find(delimiter) == 10)  fnameg1.erase(0, fnameg1.find(delimiter) + delimiter.length());
  
  fnameg+=fnameg1;

  
  //  cout << "Position of ta = " <<  fnameg1.find(delimiter) << endl;



   delimiter = ".";
   if(fnameg.find(delimiter)<100 && fnameg.find(delimiter)>1)  fnameg.erase(fnameg.find(delimiter), fnameg.length());  

   fnameg +=".root";
   
  // myfile.open(fname, ios::out);
  ifstream ifileg(fnameg.c_str());
  
 

  
  // save only if the histogrmas exist
  
  if (dwpot.GetXMAX()>0)
    {
  // Added saving of canvases
      std::cout << "Saving graph in " << fnameg.c_str() << std::endl;
      TFile *outputFileg = new TFile(fnameg.c_str(), "RECREATE");  
      Getcanvasp()->Write("potentials");
      
      Getcanvaspc()->cd(1);
      Getcanvaspc()->Write("eh_pairs");
      Getcanvaspc()->cd(2);
      Getcanvaspc()->Write("potential_currents");
      Getcanvasw()->Write("weighting");
      
      
      Getcurcanvas()->Write("currents");
      Getosccanvas()->Write("oscilloscope");
      
      // Getdhist()->Write();
      // Getwhist()->Write();
      // GetExhist()->Write();
      // GetEnhist()->Write();
      // GetMeasEnhist()->Write();
      
      
      
      dhist1 = new TH2F("dhist1"," ; x [um]; y [um]",1, 0, 1 , 1,0, 1 );  
      whist1 = new TH2F("whist1"," ; x [um]; y [um]",1, 0, 1 , 1,0, 1 );  
      dhist1->TH2F::SetBins(dwpot.GetXMAX(),0,dwpot.GetXMAX()*dwpot.GetBinSizex() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );
      whist1->TH2F::SetBins(dwpot.GetXMAX(),0,dwpot.GetXMAX()*dwpot.GetBinSizex() , dwpot.GetYMAX(),0, dwpot.GetYMAX()*dwpot.GetBinSizey() );

      if (dhist1->GetNbinsX() > 0)
	{
	  cout << "Saving...Number of bins in the histogram potential (x,y): " << dhist1->GetNbinsX() << " , " << dhist1->GetNbinsY() << endl;
	  
	  for(int i=0;i< dwpot.GetXMAX();i++)
	    {		// fill histogram
	      for(int j=0;j< dwpot.GetYMAX();j++)
		{
		  dhist1->SetBinContent(i+1,j+1,dwpot.Getdpot(j,i));
		  whist1->SetBinContent(i+1,j+1,dwpot.Getwpot(j,i));
		}
	    }
      
	  dhist1->Write();
	  whist1->Write();
	}
      else
	  cout << "Histograms not saved as they have not been computed yet" << endl;	
      
      //  outputFileg->Write();
      outputFileg->Close();
      cout << "done "<< endl;
    }
  else
    {
      cout << "Cannot save the potentials, they are not computed yet" << endl;
    }
  
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SaveGraph(){
    std::cout << "Saving graph" << std::endl;
    //   string fname = "sensors/graph/";
  /*
  
  ofstream myfile;
  
  fname+=SaveFileName1->GetText();
  fname +=".root";

  ifstream ifile(fname.c_str());
  std::cout << "Saving graph in " << fname.c_str() << std::endl;
 
      if (ifile &&  Eflag == 0)
	{
	  cout << "The file exists, and is open for input" << endl;
	  SaveButton1->SetBackgroundColor(0xff0000);
	  SaveButton1->SetTitle("sure?");
	  
	  Eflag = 1;
	  return;
	} 
      else if ( ifile && Eflag == 1)
	{
	  Eflag = 0;
	  SaveButton1->SetBackgroundColor(0x00ff00);
	  SaveButton1->SetTitle("Save");
	}

      */
      //    }
//  else 
//   {
//     cout << "The directory does not exists" << endl;
//     SaveButton1->SetBackgroundColor(0xff0000);
//      SaveButton1->SetTitle("XXX");
//      return;
//    }


//  Getcanvasp()->SaveAs("sensors/plots/potentials.png");

 
   

   // Added saving of canvases
      
   //TFile *outputFile = new TFile(fname.c_str(), "RECREATE");
    // Getcanvasp()->Write("potentials");
  Getcanvaspc()->Update();
  Getcanvasp()->SaveAs("sensors/plots/potentials.png");


  Getcanvaspc()->cd(1);
  // Getcanvaspc()->Write("eh_pairs");


  Getcanvaspc()->cd(2);
  // Getcanvaspc()->Write("potential_currents");
  Getcanvaspc()->Update();
  Getcanvaspc()->SaveAs("sensors/plots/Potcurrents.png");

  //  Getcanvasw()->Write("weighting");
  Getcanvasw()->Update();
  Getcanvasw()->SaveAs("sensors/plots/WF.png");


  //  Getcurcanvas()->Write("currents");
  Getcurcanvas()->Update();
  Getcurcanvas()->SaveAs("sensors/plots/currents.png");
  //  Getosccanvas()->Write("oscilloscope");



  
  // outputFile->Write();
  //  outputFile->Close();

  std::cout << "Done saving graph" << std::endl;
  
}


/////////////////////////////////////////////////////////////////////////////////////////////
TThread* WFGUI::GetPotentialThread() {
	return PotentialThread;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void* WFGUI::StartPotentialCalcTh(void* arg)
{


  
  WFGUI* gui = (WFGUI*) arg;

  // gui->CalculatingLabel->SetBackgroundColor(0xff0000); // set progress label color to red
  //  gui->CalculatingLabel->SetTitle("Calculating  Fields ..."); // update progess label title
  
  //  cout << __LINE__<< endl;
  //  cout << "lock10 " << __LINE__<< endl;	
  gui->SetAllButton(0);
       

  gui->CalculatingLabel->SetBackgroundColor(0xff0000); // set progress label color to red
  gui->CalculatingLabel->SetTitle("Calculating potentials ..."); // update progess label title

  gui->CallCalculatePotentials();
  
  // gui->CalculatingLabel->SetBackgroundColor(0xff0000); // set progress label color to red
  
  gui->ThreadstopPotential();
  //  cout << __LINE__<< endl;	
  //  gui->CalculatingLabel->SetTitle("Drawing...");
  //  gui->CalculatingLabel->SetBackgroundColor(0xff0000); // set progress label color to red
  gui->FillHist(); // does not increase memory
  gui->DrawAllGraph(1);
  //  cout << __LINE__<< endl;	


  
  //	sleep(1);
  if (gui->GetGainon()==true  && (gui->GetStepy()-0.1)>0. )
    {

      gui->CalculateButton->SetEnabled(kFALSE);
      gui->CalcPotButton->SetEnabled(kTRUE);
      gui->SetButton->SetEnabled(kTRUE);
            gui->CalculatingLabel->SetBackgroundColor(0xff0000);	// when calculation completed, set progress label color to green
       gui->CalculatingLabel->SetTitle("Calculate the potential again");
      
    }
  
  else
    {
      gui->SetAllButton(1);
      gui->CalculatingLabel->SetBackgroundColor(0x00ff00);	// when calculation completed, set progress label color to green
      gui->CalculatingLabel->SetTitle("Done calculating potential");

    }
  
  return NULL;	
}
/////////////////////////////////////////////////////////////////////////////////////////////
Int_t WFGUI::ThreadstartPotential(){


  CalculatingLabel->SetBackgroundColor(0xff0000); // set progress label color to red
  CalculatingLabel->SetTitle("Calculating potentials ..."); // update progess label title

  //  if( GetGainDoping()>0)
  // {
      //      SetStepy(0.011);
      // StepyEntry->SetNumber(0.011);
  // }
  if (CallBoundaryConditions() == 0)
   {
      // cout << __LINE__<< endl;
      WFGUI* arg = this;
     StartPotentialCalcTh((void*) arg);
	
     //  if(!PotentialThread && CallBoundaryConditions() == 0 ){

	// the following command generates:
	// Error in <TReentrantRWLock::WriteUnLock>: Write lock already released for 0x7fc2b8a08698
	
     //	PotentialThread = new TThread("memberfunction",
     //				      (void(*) (void *)) & StartPotentialCalcTh,(void*) arg);
	//		cout << __LINE__<< endl;	
     //	PotentialThread->Run();
	//		cout << __LINE__<< endl;	
	
	//		cout << "Lock1 " <<  __LINE__<< endl;
	return 0;
      }
	//	cout << __LINE__<< endl;	
	//	return 1;
  // 	cout << "Lock2 " <<  __LINE__<< endl;
      return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////
Int_t WFGUI::ThreadstopPotential(){
  // FillHist(); // does not increase memory


	//	if(fieldyes==true) DrawFieldHist();
	if(PotentialThread){
	  //Nicolo
	  // stopped=true;
	  //	  	cout << __LINE_<< endl;	
		TThread::Delete(PotentialThread);
		//	delete PotentialThread; // dont' remove it crashes
			PotentialThread=0;
		return 0;
	}      
	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void* WFGUI::StartCurrentsCalcTh(void* arg)
{
  //  cout << __LINE__<< endl;	
  WFGUI* gui = (WFGUI*) arg;
  
 
  if (gui == NULL) cout << " gui pointer null" << endl;
  gui->CallCalculateCurrents();
  gui->ThreadstopCurrents();

  gui->GetCalculatingLabel()->SetBackgroundColor(0x00ff00);
  gui->GetCalculatingLabel()->SetTitle("Done !");



  return NULL;	
}
/////////////////////////////////////////////////////////////////////////////////////////////
Int_t WFGUI::ThreadstartCurrents(){

  //CallCalculateCurrents();
	if(!CurrentsThread){
		//stopped=false;
		WFGUI* arg = this;
		//		cout << __LINE__<< endl;	
		CurrentsThread= new TThread("memberfunction",
			            &StartCurrentsCalcTh,
			            (void*) arg);
		CurrentsThread->Run();	
		return 0;
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////
Int_t WFGUI::ThreadstopCurrents(){
	if(CurrentsThread){
		//stopped=true;
		TThread::Delete(CurrentsThread);
		// delete CurrentsThread;
		CurrentsThread=0;
		return 0;
	}      
	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::KillPotentialThread() {
		TThread::Delete(PotentialThread);
		// delete PotentialThread;
		PotentialThread=0;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TCanvas* WFGUI::Getcanvasp() {
	return canvasp;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TCanvas* WFGUI::Getcanvaspc() {
	return canvaspc;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TCanvas* WFGUI::Getcanvasw() {
	return canvasw;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TCanvas* WFGUI::Getcurcanvas() {
	return curcanvas;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TCanvas* WFGUI::Getosccanvas() {
	return osccanvas;
}
/////////////////////////////////////////////////////////////////////////////////////////////
TGHProgressBar* WFGUI::GetProgressBar() {
	return CurrentsProgressBar;
}
/////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetT() {
	return Temp;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetGainon(bool gain) {
	gainon=gain;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetParticleType(int ptype) {
	ParticleType=ptype;
}
/////////////////////////////////////////////////////////////////////////////////////////////
int WFGUI::GetParticleType() {
	return ParticleType;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetXEntry(double ptype) {
  XEntry=ptype; // generated pairs
}
/////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetXEntry() {
  return XEntry; //generated pairs
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetAngleEntry(double ptype) {
  AngleEntry=ptype; // generated pairs
}
/////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetAngleEntry() {
  return AngleEntry; //generated pairs
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetNumPairs(double ptype) {
  NumPairs=ptype; // generated pairs
}
/////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetNumPairs() {
  return NumPairs; //generated pairs
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBField(double ptype) {
  BField=ptype; // generated pairs
}
/////////////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBField() {
  return BField; //generated pairs
}
//////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetGainon() {
	return gainon;
}
//////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetGainRatio(double gain) {
	GainRatio=gain;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetGainRatio() {
	return GainRatio;

}
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetGainLayerVdepletion(double Doping) {
  
  double GainLayerV= 0;
  double DopingRem = 1;
  // if (Doping != 0)
    {
   GainLayerV= Doping*ECHARGE/(2*EPSILON*EPSILONR)*1e-12*GetGainLength()*GetGainLength();
   if (GetInitialDopRemoval()) DopingRem = InitialDopingRem(GetDopinggainlayerValue()*1e-6 , GetFluence(), 1);
   GainLayerVdepletion= GainLayerV*DopingRem; // E = V/d
   cout << "SetGainlayerVdepletion = "  <<  GainLayerVdepletion << endl;
   SetGainRegionVdepletion(Doping);

    }  

}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetGainLayerVdepletion() {
  //  cout << " GetGainlayerVdepletion = " << GainLayerVdepletion << endl;
  return GainLayerVdepletion;

}
//////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetGainRegionVdepletion(double Doping) { // this include also the space between the gain layer and the n++ electrode
  // GainRegionVdepletion=GetGainLayerVdepletion()*(1.+ GetGainDepth()); // E = V/d

  // Gain layer depletion  Vd = Nqx^2/2ee
  // Electric field at the end of the GL E = dV/dx = 2Vd/x  = GetGainLayerVdepletion()*2/GetGainLength()
  // Potential to keep the field constant  V = E*d =  GetGainLayerVdepletion()*2(GetGainDepth()-ELECTRODE_DEPTH)/GetGainLength()

  // assume 3% constant doping tail
  
  //  double FractDopingTail = 0.03;
  double ccc = .9; //
  Doping = 0;
  // double RegionV=FractDopingTail*Doping*ECHARGE/(2*EPSILON*EPSILONR)*1e-12*(GetGainDepth()-ELECTRODE_DEPTH)*(GetGainDepth()-ELECTRODE_DEPTH);
  GainRegionVdepletion = 0;
  
  if (GetGainLength() >0) GainRegionVdepletion=ccc*GetGainLayerVdepletion()*(1+2.*(GetGainDepth()-ELECTRODE_DEPTH)/GetGainLength()); // E = V/d

  // 1 micron space, twice the doping
  if( GetGainShape() == 10) GainRegionVdepletion=ccc*GetGainLayerVdepletion()*(1+2.*(1.)/GetGainLength())+GetGainLayerVdepletion(); // E = V/d
  
  // GainRegionVdepletion=GetGainLayerVdepletion()(1+ ; // E = V/d
  // GainRegionVdepletion=GetGainLayerVdepletion()+(GetGainDepth()-ELECTRODE_DEPTH)/(dwpot.GetYMAX()*dwpot.GetBinSizey())*GetVDepl(); // E = V/d

  cout << "Gain layer region V depletion: " << setprecision(3) << GainRegionVdepletion << " [V]" << endl;
  //  cout << "Bias for tail depletion: " << setprecision(3) << RegionV << " [V]" << endl;

  if (GainRegionVdepletion >0 ) SetGainon(true);
  
  // cout << "Gain layer region V depletion VD*(width+depth)/depth: " << GetGainLayerVdepletion()*(GetGainDepth()+GetGainLength())/GetGainLength() << endl;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetGainRegionVdepletion() {
	return GainRegionVdepletion;

}
//////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetPairs(double val) {
  Pairs=val; //measured pairs
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetPairs() {
  return Pairs; //measured pairs

}
//////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetVBias(double temp) {
  double VdepGainRegion= GetGainRegionVdepletion();

  //  VBias=temp;
  if (VFROMDOP ==1 ) VBias=temp;
  else VBias = temp - VdepGainRegion;
  if (VBias < 0) cout << "Warning: underdepleted sensor" << endl;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetVBias() {
  
	return VBias;

}
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetBiasEntry(double temp) {
  BiasEntry = temp;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetBiasEntry() {
  
	return BiasEntry;

}
//////////////////////////////////////////////////////////////////////////////////////




void WFGUI::SetVDepl(double temp) {
	VDepl=temp;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetVDepl() {
	return VDepl;
}
//////////////////////////////////////////////////////////////////////////////////////



void WFGUI::SetYGain(double y_gain) {
	ygain=y_gain;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetYGain() {
	return ygain;
}

//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetAngle() {
	//angle = (TMath::Pi()/180)*CarriersAngleNumberentry->GetNumber();
	angle = CarriersAngleNumberentry->GetNumber(); //angle in degrees
	return angle;
}
/////////////////////////////////////////////////////////////////////////
int WFGUI::GetDimMaxCarriers(){
	return dimMaxCarriers;
}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetAllButton(int NN) {
  if (NN == 1)
    {
      if(!GetLessPlot())
	{
	  //	  cout << __LINE__<< endl;	
	  OnStripsButton->SetEnabled(kTRUE);
	  BetweenStripsButton->SetEnabled(kTRUE);
	  ExButton->SetEnabled(kTRUE);
	  EyButton->SetEnabled(kTRUE);
	  OnStripsButton2->SetEnabled(kTRUE);
	  BetweenStripsButton2->SetEnabled(kTRUE);
	  ExButton2->SetEnabled(kTRUE);
	  EyButton2->SetEnabled(kTRUE);
	  DrawCutsUserEntry->SetEnabled(kTRUE);
	  DrawCutsUserEntry2->SetEnabled(kTRUE);
	}
	CalculateButton->SetEnabled(kTRUE);
	CalcPotButton->SetEnabled(kTRUE);
	CalcPotButton->SetTextColor(1,kFALSE);

    }
  else
    {
      //	cout << __LINE__<< endl;	
	OnStripsButton->SetEnabled(kFALSE);
	BetweenStripsButton->SetEnabled(kFALSE);
	ExButton->SetEnabled(kFALSE);
	EyButton->SetEnabled(kFALSE);
	//		cout << __LINE__<< endl;	
	//	EtotButton->SetEnabled(kFALSE);
	CalculateButton->SetEnabled(kFALSE);
	//	cout << __LINE__<< endl;	
	CalcPotButton->SetEnabled(kFALSE);
	OnStripsButton2->SetEnabled(kFALSE);
	BetweenStripsButton2->SetEnabled(kFALSE);
	ExButton2->SetEnabled(kFALSE);
	EyButton2->SetEnabled(kFALSE);
	DrawCutsUserEntry->SetEnabled(kFALSE);
	DrawCutsUserEntry2->SetEnabled(kFALSE);
	//	EtotButton2->SetEnabled(kFALSE);
    }

	return;

}

/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetFileName(string* fname) {
  //  SetLess2DPlot(kTRUE);
  fileName = *fname;
}


/////////////////////////////////////////////////////////////////////////////////////////////
string* WFGUI::GetFileName() {
  
  if (fileName=="" || strncmp(fileName.c_str(),"wf_",3)==0)
    {
      
      //      CallSetFileName("wf");
      CallSetFileName();  
    }
  
  //
  return &fileName;
}
/////////////////////////////////////////////////////////////////////////////////////////////

string* WFGUI::CallSetFileName() {

  
  //  TDatime *NameDate = new TDatime();
  Int_t Dif = 0;
  Float_t BW=0.;
  if(Getdiffusionon() ) Dif = 1;  
  if(OscOn==true)   BW = GetOscBW();
  string Part;
  Int_t Qc = 0;

  switch (radiobuttonstatus){
  case MIPunif:
    Part = "_MIPU";
    //   d =  YMAXentry->GetNumber()/cos(TMath::Pi()/180*GetAngle());
    // Qc = (0.027*log(d)+0.126); //keV/micron ==> log base "e"

    Qc = GetMPV();
    break;
    
  case MIPnonunif:
    Part = "_MIPNONU";
    Qc = 0;
    break;
    
  case MIPlandau:
    Part = "_MIPL";
    Qc = 0;
    break;
    
  case ALPHA_TOP:
    Part = "_AT";
          if (ParticleSpecificsEntry->GetNumber()<=30){//putting the 30µm range limit here
          Qc = ParticleSpecificsEntry->GetNumber();//MODIFIEDRangeEntry->GetNumber();
          }
          else{
              Qc = 30;
          }
    break;
    
  case ALPHA_BOTTOM:

    Part  = "_AB";
          if (ParticleSpecificsEntry->GetNumber()<=30){//putting the 30µm range limit here
              Qc = ParticleSpecificsEntry->GetNumber();//MODIFIEDRangeEntry->GetNumber();
          }
          else{
              Qc = 30;
          }
    break;
    
  case USR_CHARGE:
    Part = "_USRQ";
          Qc = ParticleSpecificsEntry->GetNumber();//MODIFIEDRChargeentry->GetNumber();
    break;
    
  default: break;
  }
  
  string defaultName = FileNameEntry->GetText();
  if (defaultName=="") defaultName = "wf";


  //  cout << Part << endl;
  //  ss << FileNameEntry->GetText()
  std::stringstream ss;

  ss << defaultName 
    //  ss << FileNameEntry->GetText()
    << "_BV" <<  Biasentry->GetNumber()
     << "_DV" << Depletionentry->GetNumber()
     << "_W" << GetNStrips()
     << "_H" << GetYMax()
    << "_SP" << Pitchentry->GetNumber()
    << "_SW" << Widthentry->GetNumber()
    << "_G" << Gainentry->GetNumber()
    //    << "_GR" << GainRatioentry->GetNumber()
    << "_T" << TempEntry->GetNumber()
     << "_A" << CarriersAngleNumberentry->GetNumber()
     << "_D" << Dif  
     << "_BW" << BW
     << "_C" << GetCDet()
     << "_L" << GetLDet()
     << "_GS" << GetSampling()*1e12
     << "_BBBW" << GetBBBW() 
     << Part
     << Qc;

    //    << "_" << NameDate->GetDate()
    // << "_" << NameDate->GetTime()
    //    << ".txt";
	fileName = ss.str();
	if (GetFileNameOn() ) 
	  cout <<"Creating output file: "<< fileName << endl;
	return &fileName;

}
/////////////////////////////////////////////////////////////////////////////////////////////
/*
string* WFGUI::CallSetFileName(string defaultName) {
  
  std::stringstream ss;
  ss << FileNameEntry->GetText()
    << "_BV" <<  Biasentry->GetNumber()
    << "_W" << XMAXentry->GetNumber()
    << "_H" << YMAXentry->GetNumber()
    << "_SP" << Pitchentry->GetNumber()
    << "_SW" << Widthentry->GetNumber()
    << "_G" << Gainentry->GetNumber()
    << "_T" << TempEntry->GetNumber()
    //    << "_" << NameDate->GetDate()
    // << "_" << NameDate->GetTime()
    << ".txt";
	fileName = ss.str();
	cout <<"Creating output file: "<< fileName << endl;
	return &fileName;

}
 
*/
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetAlphaRange(double valrange) {
	AlphaRange=valrange;
	cout << "Range of alpha particle set to " << valrange << " microns " << endl;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetAlphaRange() {
	return AlphaRange;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetXRayRange(double valrange) {
	XRayRange=valrange;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetXRayRange() {
	return XRayRange;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetPrecision(int valprecision) {
	Precision=valprecision;
}
//////////////////////////////////////////////////////////////////////////////////////
int WFGUI::GetPrecision() {
	return Precision;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetSampling(double valprecision) {
  Sampling = valprecision*1e-12; // in [s]
  
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetSampling() {
	return Sampling;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetPrintSampling( int valprecision) {
  PrintSampling = valprecision;
  cout << "Print sampling set to " <<  PrintSampling << " [GS/s]" << endl;
  cout << "Saving current each " << (1000/GetPrintSampling()) << " ps; every "<< (int) (1000./GetPrintSampling())/(GetSampling()*1e12) << " time steps" << endl;
}
//////////////////////////////////////////////////////////////////////////////////////
int WFGUI::GetPrintSampling() {
	return PrintSampling;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetStepx( double val) {
    Stepx = val;
    // if(StepXButton[0]->IsOn())  Stepx = 1.;
    // else Stepx = 0.1;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetStepx() {
	return Stepx+0.05;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetStepy( double val)
{
  // if(StepYButton[0]->IsOn())  Stepy = 1.;
  //  cout << "setting step y = "<< val << endl;
 Stepy = val;
  
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::GetStepy() {
  return Stepy+0.01;
}
/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetFluence(double valneq) {
  Fluence=(valneq+0.000001)*1E14; //to avoid zeros
  if (!GetInitialDopRemoval() && !GetAcceptorCreation() && !GetGainQuenching() && !GetCCEOn() &&  !GetLnAcceptorCreation() )  Fluence= 0;
  //else Fluence = 0;

}

double WFGUI::GetFluence() {
  return Fluence;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetQuenchCoef(double valneq) {
  double base = 2E-11;
  Quench=valneq*base; //to avoid zeros
  //else Fluence = 0;
}

double WFGUI::GetQuenchCoef() {
  return Quench;
}



/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetBetaElectrons(double valneq) {
    if (GetCCEOn())   BetaElectrons=valneq;
    else BetaElectrons = 0;
    
}
double WFGUI::GetBetaElectrons() {
    return BetaElectrons;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetBetaHoles(double valneq) {
    if (GetCCEOn())   BetaHoles=valneq;
    else BetaHoles = 0;
    
}
double WFGUI::GetBetaHoles() {
    return BetaHoles;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetDJValue(double valneq) {
    DJ=valneq;
}
double WFGUI::GetDJValue() {
    return DJ;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//////////////nicco///////////////////////////////////////////////////////////////////////////////
void WFGUI::SetDopinggainlayerValue(double valneq) {
  DopingValue= ((CallGetDopingBulk()==PTYPE) ? -1.0 : 1.0)*valneq*1e16*1e6; // 10^16/cm3==> values in N/m^3
  // DopingValue= valneq*1e16*1e6; // 10^16/cm3==> values in N/m^3
  cout << "Gain Implant Doping Value = " << DopingValue*1e-6 << " N/cm^3" << endl; 
}
double WFGUI::GetDopinggainlayerValue() {
    return DopingValue;
}
/////////////////////////////////////////////////////////////////////////////////////////////


void WFGUI::SetDJehValue(double valneq) {
    DJeh=valneq;
}
double WFGUI::GetDJehValue() {
    return DJeh;
}


/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetNumberP(int number) {
	NumberP=number;
}
//////////////////////////////////////////////////////////////////////////////////////
int WFGUI::GetNumberP() {
	return NumberP;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetConstQFlag(bool cflag) {
	ConstQFlag=cflag;
}
//////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetConstQFlag() {
	return ConstQFlag;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetCalibFlag(bool cflag) {
	CalibFlag=cflag;
}
//////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetCalibFlag() {
	return CalibFlag;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetUniformQFlag(bool uflag) {
	UniformQFlag=uflag;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetUserUniformQFlag(bool uflag) {
	UserUniformQFlag=uflag;
}
//////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetUniformQFlag() {
	return UniformQFlag;
}
//////////////////////////////////////////////////////////////////////////////////////
bool WFGUI::GetUserUniformQFlag() {
	return UserUniformQFlag;
}
//////////////////////////////////////////////////////////////////////////////////////

void WFGUI::SetEvNumber(int ev) {
	EvNumber=ev;
}
//////////////////////////////////////////////////////////////////////////////////////
int WFGUI::GetEvNumber() {
	return EvNumber;
}
//////////////////////////////////////////////////////////////////////////////////////
void WFGUI::Setygainlow(float ev) {
	YGLow=ev;
}
//////////////////////////////////////////////////////////////////////////////////////
float WFGUI::Getygainlow() {
	return YGLow;
}
//////////////////////////////////////////////////////////////////////////////////////
float WFGUI::GetACValue() {
	return ACValue;
}
//////////////////////////////////////////////////////////////////////////////////////
void WFGUI::Setygainhigh(float ev) {
	YGHigh=ev;
}
//////////////////////////////////////////////////////////////////////////////////////
float WFGUI::Getygainhigh() {
	return YGHigh;
}
//////////////////////////////////////////////////////////////////////////////////////
int WFGUI::GetEvent() {
	return ThisEvent;
}
//////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetEvent(int thisev) {
	ThisEvent=thisev;
	
}

//////////////////////////////////////////////////////////////////////////////////////
int WFGUI::GetGainRecess() {
	return GainRecess;
}
//////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetGainRecess(int thisev) {
	GainRecess=thisev;
	
}
//////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetHStyle(TH1F* hist, double size) {
  hist->GetYaxis()->SetTitleSize(size);
  hist->GetXaxis()->SetTitleSize(size);
  hist->GetXaxis()->SetTitleOffset(0.9);
  hist->GetYaxis()->SetTitleOffset(0.6);
  hist->GetYaxis()->SetLabelSize(size);
  hist->GetXaxis()->SetLabelSize(size);
  hist->GetYaxis()->SetNdivisions(4);

}
//////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetTGraphStyle(TGraph* hist, double size) {
  hist->GetYaxis()->SetTitleSize(size);
  hist->GetXaxis()->SetTitleSize(size);
  hist->GetXaxis()->SetTitleOffset(0.8);
  hist->GetYaxis()->SetTitleOffset(0.5);
  hist->GetYaxis()->SetLabelSize(size);
  hist->GetXaxis()->SetLabelSize(size);
  hist->GetYaxis()->SetNdivisions(4);

}
//////////////////////////////////////////////////////////////////////////////////////
void WFGUI::SetStyle() {
	      gStyle->SetStatColor(kWhite);
	      gStyle->SetStatFont(42);
	      gStyle->SetStatFontSize(0.1);
	      gStyle->SetStatTextColor(1);
	      gStyle->SetStatFormat("6.4g");
	      gStyle->SetStatBorderSize(1);
	      gStyle->SetStatH(0.1);
	      gStyle->SetStatW(0.2);
	      gStyle->SetStatX(0.9);
	      gStyle->SetStatY(0.9);


	      
	      gStyle->SetLabelSize(0.06,"y");
	      gStyle->SetLabelOffset(.01,"y");
	      gStyle->SetTitleSize(0.05,"y");
	      gStyle->SetTitleOffset(1.,"y");
	      
	      gStyle->SetLabelSize(0.06, "x");
	      gStyle->SetLabelOffset(.01, "x");
	      gStyle->SetTitleSize(0.05, "x");
	      gStyle->SetTitleOffset(1., "x"); 
	          
	      //     gStyle->SetTitleSize(0.15);
	      gStyle->SetTitleX(0.5);		//Title box x position (top left-hand corner)	
	      gStyle->SetTitleY(0.995); 	//Title box y position (default value)		
	      gStyle->SetTitleW(1.);		//Title box width as fraction of pad size
	      gStyle->SetTitleH(0.1); //Title box height as fraction of pad size	
	      //	      gStyle->SetTitleColor(0);	//Title box fill color
	      gStyle->SetTitleTextColor(1);	//Title box text color
	      //	      gStyle->SetTitleStyle(1001);	//Title box fill style!
	      //TitleFont = 10*fontid + 2 (12 is normal, 22 bold 32 italic)
	      ///     gStyle->SetTitleFont(32);  	//Title box font (32=italic times bold)
	      
	      // gStyle->SetTitleBorderSize(2);	//Title box border thickness
	      //	      gStyle->SetLineColor(1);		// set line color to 
	      //gStyle->SetLineWidth(1);
	      // gStyle->SetLineStyle(1);

	      gStyle->SetPadLeftMargin(.1);
	      gStyle->SetPadRightMargin(.1);
}
//////////////////////////////////////////////////////////////////////////////////////


float WFGUI::Getygainlayerhigh(){

  if((StripButton[1]->IsOn() && BulkButton[0]->IsOn()) || (StripButton[0]->IsOn() && BulkButton[1]->IsOn()))
    return (GetYMax()- GetGainDepth() + GetGainLength() )*dwpot.GetYMAX()*dwpot.GetBinSizey()/GetYMax();
  else
    return GetGainDepth();


}
//////////////////////////////////////////////////////////////////////////////////////
float WFGUI::Getygainlayerlow(){

  if((StripButton[1]->IsOn() && BulkButton[0]->IsOn()) || (StripButton[0]->IsOn() && BulkButton[1]->IsOn()) )
    return (GetYMax() - GetGainDepth())*dwpot.GetYMAX()*dwpot.GetBinSizey()/GetYMax() ;
  else
    return GetGainDepth()-GetGainLength();

  //  return 8;
}
//////////////////////////////////////////////////////////////////////////////////////
double WFGUI::InitialDopingRem(double density, double fluence, int type)
{
  //  double Frac = 0;
  // density N/cm3
  //   0.63*N_A/[N_Si*sigma*D2]
  double N_Si = 5.00E+22;
  double sigma = 7.50E-22;
  double N_Ao = 4.50E+16;
 
  double D2 = 1./(1.+ pow(N_Ao/abs(density), 2./3.));
  double c_new = (N_Si*sigma*D2)/(abs(density));
  // cout << "D2 = " << D2 << "c_new num = "<<   (N_Si*sigma*D2)<< " c_new_den = " << (0.63*abs(density)) << "\n";  
  double c = 0;
  // p-doped layer
  //    cout << "CallSetParticleIrr " << ParticleIrrType << endl;
  
  // if (density < 0) c = 2e-9*pow(fabs(density), -0.38); // protons+neutrons
  //  if (density < 0) c = 30e-9*pow(fabs(density), -0.46); // neutrons
    if (ParticleIrrType == 0)
      {
	//	if (density < 0) c = 10e-9*pow(fabs(density), -0.45); // neutrons  new
		if (density < 0) c = 9e-7*pow(fabs(density), -0.57); // neutrons  after TREDI
	//	if (density < 0) c = 1e-10*pow(fabs(density), -0.327); // Gallium neutrons  after TREDI
	
	else  c = 0.1/fabs(density); // n-doped layer
      }
    else
      if (density < 0) c = 2e-9*pow(fabs(density), -0.39); // pions new 200 MeV
    //  if (density < 0) c = 0.08e-9*pow(fabs(density), -0.285); // protons
  //
  //  if (density < 0) c = 0.015*1e-14; // from the signal

    if (type == 1)
      {
	//	c = c/DopingGLType; // coefficient to account for B, BC, Ga, GaC, BLD
	cout << "Density = " << density << " [n/cm3];  c = " << c_new/DopingGLType << "\n";
	c  = c_new/DopingGLType;
      }
    return exp(-c*fluence);
  
}

//////////////////////////////////////////////////////////////////////////////////////
int WFGUI::GetMPV(){
  //  if(StripButton[1]->IsOn()*BulkButton[0]->IsOn() || StripButton[0]->IsOn()*BulkButton[1]->IsOn() ) return (dwpot.GetYMAX()*dwpot.GetBinSizey()- GetGainDepth()) ;
  double Qc = 0;
  double d = 0;
  d =  YMAXentry->GetNumber()/cos(TMath::Pi()/180*GetAngle());
  Qc  = 1000./3.6*(0.027*log(d)+0.126); //keV/micron ==> log base "e"
  if (Qc < 50 || Qc> 100) Qc = 75;
  return Qc;

}

//////////////////

double WFGUI::GetEion() {
	return Eion;
}

///--------------------------------------------------------------------------------------------------------------------------------------------------///

