#include "TGClient.h"
#include "TGNumberEntry.h"
#include "TGFrame.h"
#include "TGButtonGroup.h"
#include "TGCanvas.h"
#include "TCanvas.h"
#include "TGButton.h"
#include "TGLabel.h"
#include "TGTab.h"
#include "TGObject.h"
#include "TRootEmbeddedCanvas.h"
#include "Riostream.h"
#include "TLatex.h"
#include "TApplication.h"
#include "TH1.h"
#include "TF1.h"
#include "TGaxis.h"
#include "Carriers.h"
#include "TThread.h"
#include "TRandom3.h"
#include "TMutex.h"
#include "TDatime.h"
#include  <ctime>
#include "TGProgressBar.h"
#include "TGComboBox.h"
#include "TGScrollBar.h"
#include "TFile.h"
#include "TEllipse.h"
#include "TBox.h"
// #include "TGuiBldDragManager.h"
// #include "TRootGuiBuilder.h"
#include "TGListTree.h"
#include "TColor.h"
#include <string.h>
#include <sstream>

using namespace std;

typedef enum ParticleRadioButtonStatus_enum {
	MIPunif,
	MIPnonunif,
	MIPlandau,
	//ALPHA_UNDEF,//original particle selection
	ALPHA_TOP,
	ALPHA_BOTTOM,
	USR_CHARGE,
	CALIB,
	EDGE,
	XRAY,
	TRENCH
}ParticleRadioButtonStatus;

class WFGUI : public TGMainFrame {

 private:

  bool plotupdate;
  bool LessPlot;
  bool Less2DPlot;
  bool stopped;
  bool bfieldon;
  bool diffusionon;
  bool chargecloudon;
  bool currentson;
  bool fieldyes;
  bool abscuryes;
  bool showcuryes;
  bool gainon;
  bool OscOn;
  bool NA62On;
  bool SC_CSAOn;
  bool FileNameOn;
  bool BatchOn;
  bool BatchRandomOn;
  bool ForceGain;
  bool ScreenedGain;
  bool PrecisionGain;
  bool AC;
  bool CCEOn;
  bool InitialDopRemoval;
  bool AllOn;
  bool AcceptorCreation;
  bool LnAcceptorCreation;
  bool GainQuenching;
  bool DJOn;
  bool ConstQFlag;
  bool CalibFlag;
  bool UniformQFlag;
  bool UserUniformQFlag;
  bool ReadOutTopFlag;
  int Eflag;
  unsigned char stripdoping;
  unsigned char  bulkdoping;
  
  double Galpha;
  double Gbeta;
  double *gain; // absolute value of weighting field
  double Ygain[500]; // absolute value of weighting field
  
  
  int DetType;
  int ParticleType;
  int ParticleIrrType;
  float DopingGLType;
  int carriersin;
  float ygain;
  float GainRatio;	       
  float GainIndent;
  double GainLayerVdepletion;
  double GainRegionVdepletion;
  double Pairs;
  double BField;
  double BiasEntry;
  double VBias;
  double VDepl;
  double Temp;	      // temperature
  double XEntry;
  double AngleEntry;
  double NumPairs;
  int dimMaxCarriers;
  int Precision;
  double AlphaRange;
  double XRayRange;
  double Sampling;
  int NumberP;
  double OscBW; //oscilloscope bandwidth

  int PrintFrequency;
  int PrintSampling; 

  
  double BBImp; 					//oscilloscope bandwidth
  double CSAImp; 					//oscilloscope bandwidth
  double CDet; 					//Cap. Detector
  double LDet; 					//Ind. Detector
  int EvNumber;					//total number of events
  int ThisEvent;					//
  int GainRecess;					//
  int NGainDoping;
  int NGainKind;
  int NGainShape;
  double GainShapeDepth;
  double GainShapeExp;
  double GainShapeLength;
  std::map<std::string, double> UserValues;
  
  float YGLow;
  float YGHigh;
  float ACValue;
  double angle;
  double TFall;
  double SWidth;
  double TRise;
  double CSAVth;
  double ShTrans;
  double ShImp;
  double ShNoise;
  double Fluence;
  double Quench;
  double BetaElectrons;
  double BetaHoles;
  
  double DJ;
  double DopingValue;
  double DJeh;
  
  double BBVth;
  double BBNoise;
  double BBGain;
  double BBBW;
  
  double CSAJitter;
  double BBJitter;
  double CSATVth;
  double CSAFJitter;
  double CSAFTVth;
  double BBFTVth;
  double BBTVth;
  int YBin;
  int XBin;

  double Eion;
  
  string fileName;
  
  int DJvalue;
  
  ParticleRadioButtonStatus radiobuttonstatus;
  
  //TMutex* mut;
  
  TRootEmbeddedCanvas *driftcanvas;
  TRootEmbeddedCanvas *driftpcanvas;		
  TRootEmbeddedCanvas *weightcanvas;
  TRootEmbeddedCanvas *weightpcanvas;
  TRootEmbeddedCanvas *driftfcanvas;
  TRootEmbeddedCanvas *weightfcanvas;
  TRootEmbeddedCanvas *currentscanvas;
  TRootEmbeddedCanvas *oscilloscopecanvas;
  TRootEmbeddedCanvas *electronicsIIcanvas;
  
  TGTab *PotentialTab;
  TGCompositeFrame *DriftPTab;
  TGCompositeFrame *WeightingPTab;
  TGCompositeFrame *CurrentsTab;
  TGCompositeFrame *OscilloscopeTab;
  TGCompositeFrame *ElectronicsIITab;
  
  
  TGVerticalFrame *SettingsGlobalFrame;	
  // Contains:
  //		TGHorizontalFrame *SettingsFrame; 
  TGVerticalFrame *SettingsFrame;
  
  TGHorizontalFrame *SettingsFrame2; 		
  TGHorizontalFrame *SettingsFrame3;
  TGHorizontalFrame *BFieldFrame; 
  
  // SettingsFrame contains:
  TGGroupFrame *NameFrame;
  TGHorizontalFrame *SaveFrame;
  TGHorizontalFrame *SaveFrameWF;
  TGGroupFrame *SaveFrameV;
  
  TGGroupFrame *BatchFrame;
  TGHorizontalFrame *BatchOnButtonFrame;
  TGHorizontalFrame *BatchEventNumberFrame;
  
  //		TGGroupFrame *CarriersInFrame;

  TGGroupFrame *ReadoutFrame;
  TGHorizontalFrame *DJFrame;
  TGHorizontalFrame *IrrParticleFrame;
  TGHorizontalFrame *CarriersInFrame;
  TGHorizontalFrame *CarriersInLabelFrame;
  TGHorizontalFrame *CarriersInSetFrame;
  
  //		TGGroupFrame *EdgeFrame;
  TGHorizontalFrame *EdgeFrame;
  TGVerticalFrame *EdgeLabelFrame;
  TGHorizontalFrame *EdgeSetFrame;
  
  TGVerticalFrame *DJDetailsFrame;
  TGHorizontalFrame *DJDetailsTopFrame;
  TGHorizontalFrame *DJDetailsBottomFrame;
  
  // SettingsFrame2 contains:
  //		TGGroupFrame *ControlFrame;
  TGVerticalFrame *ControlFrame;
  TGVerticalFrame *RightFrame;	
  
  //ControlFrame contains 
  TGGroupFrame *ParticlesPropertiesFrame;
  //dropdownlistbox frame
  TGVerticalFrame *ParticlesKindFrame;
  TGHorizontalFrame *ParticlesSpecificsFrame;
  TGHorizontalFrame *ParticlesSpecificsLabelFrame;
  TGVerticalFrame *ParticlesSpecificsSetFrame;
  TGComboBox *ParticleKind;
  TGComboBox *CSAKind;
  
  
  TGVerticalFrame *GainKindFrame;
  TGVerticalFrame *GainShapeFrame;
  TGVerticalFrame *GainDopingFrame;
  //        TGHorizontalFrame *GainSpecificsFrame;
  //TGHorizontalFrame *GainSpecificsLabelFrame;
  // TGVerticalFrame *GainSpecificsSetFrame;
  TGComboBox *GainDoping;
  TGComboBox *GainKind;
  TGComboBox *GainShape;
  
  //TGLabel *ChargeLabel;
  //TGLabel *RangeLabel;//already in the original
  //TGLabel *CalibLabel;
  
  
  //RightFrame contains 
  TGGroupFrame *DetectorPropertiesFrame;
  //	TGVerticalFrame *ElectronicsFrame;
  
  TGGroupFrame *ElectronicsFrame;
  
  TGHorizontalFrame *ExitFrame;			
  //TGGroupFrame *ParticlesPropertiesFrame1;//not used
  TGGroupFrame *ChargeCollectionFrame;
  TGGroupFrame *LorentzInfoFrame;
  TGGroupFrame *LTInfoFrame;
  TGHorizontalFrame *DimensionsFrame;
  TGHorizontalFrame *VoltageFrame;
  TGGroupFrame *CurrentsFrame;
  TGGroupFrame *PlotGroupFrame;
  TGGroupFrame *SelectPartFrame; //!!!!!!!!!!!!!!
  //		TGGroupFrame *OscilloscopeFrame;
  
  
  TGVerticalFrame *OscilloscopeLeftFrame;
  TGVerticalFrame *OscilloscopeRightFrame;
  TGHorizontalFrame *OscilloscopeFrame;
  
  
  
  TGVerticalFrame *DimSetFrame;
  TGVerticalFrame *DimLabelFrame; 
  //		TGVerticalFrame *VolSetFrame;
  TGHorizontalFrame *VolSetFrame;
  TGVerticalFrame *VolLabelFrame; 
  
  TGHorizontalFrame *WPFrame;
  TGHorizontalFrame *ThickFrame;
  TGHorizontalFrame *ForceGainFrame;
  TGHorizontalFrame *ScreenedGainFrame;
  
  TGButtonGroup *TypeFrameGroup;
  TGButtonGroup *TypeButtonGroup;
  TGButtonGroup *DopButtonGroup;
  TGButtonGroup *PartButtonGroup;
  
  //		TGHorizontalFrame *ButtonPotFrame;
  TGGroupFrame *ButtonPotFrame;
  TGHorizontalFrame *ButtonCurFrame;
  TGHorizontalFrame *UpdateFrame;
  TGHorizontalFrame *DriftPTabFrame;
  TGHorizontalFrame *WeightPTabFrame;
  TGHorizontalFrame *LabelsTabFrame;
  TGHorizontalFrame *LabelsTabFrame2;
  TGHorizontalFrame *CurrentsLabelFrame;
  TGHorizontalFrame *ElectronicsLabelFrame;
  TGVerticalFrame *ChargeLabelFrame1;
  TGVerticalFrame *ChargeLabelFrame2;
  TGVerticalFrame *ChargeLabelFrame3;
  TGVerticalFrame *ChargeNumberFrame1;
  TGVerticalFrame *ChargeNumberFrame2;
  TGVerticalFrame *ChargeNumberFrame3;
  TGVerticalFrame *CurrentsInfoFrame;
  TGHorizontalFrame *TempSetFrame;
  TGVerticalFrame *DiffusionAndDispersionFrame;
  //TGHorizontalFrame *AlphaRangeFrame;//original particle selection
  TGGroupFrame *PrecisionGroupFrame;
  TGHorizontalFrame *PrecisionFrame;
  TGHorizontalFrame *SamplingFrame;
  TGHorizontalFrame *NumberFrame;
  TGHorizontalFrame *OscSetFrame;
  TGHorizontalFrame *CDSetFrame;
  TGHorizontalFrame *TFallSetFrame;
  TGHorizontalFrame *VthSetFrame;
  TGHorizontalFrame *ShTransFrame;
  TGHorizontalFrame *ShNoiseFrame;
  TGHorizontalFrame *ShImpFrame;
  TGHorizontalFrame *LCFrame;
  
  TGHorizontalFrame *BBBWFrame;
  TGHorizontalFrame *BBNoiseFrame;
  
  TGHorizontalFrame *TRiseSetFrame;
  TGHorizontalFrame *BatchEventSetFrame;
  
  
  //TGButtonGroup *DopBulkFrame;
  TGHorizontalFrame *DopingFrame;
  TGHorizontalFrame *DopStripFrame;
  TGButtonGroup *DopStripButtonGroup;
  TGButtonGroup *ParticleIrrButtonGroup;
  TGButtonGroup *DopingGLButtonGroup;
  
  TGButtonGroup *DopBulkButtonGroup;
  TGButtonGroup *StepYButtonGroup;
  TGButtonGroup *StepXButtonGroup;
  TGButtonGroup *DJButtonGroup;
  
  TGButtonGroup *ReadOutButtonGroup;
  
  // 		TGLabel *DopStripLabel;
  TGHorizontalFrame *DopBulkFrame;
  // 		TGLabel *DopBulkLabel;
  
  //		TGGroupFrame *GainFrame;
  TGGroupFrame *GainBigFrame;
  TGHorizontalFrame *GainFrame;
  TGVerticalFrame *GainSetFrame;
  TGVerticalFrame *GainLabelFrame;
  
  TGGroupFrame *IrradiationFrame;
  TGHorizontalFrame *IrradiationValuesFrame;
  TGVerticalFrame *IrradiationOnFrame;
  TGVerticalFrame *IrradiationSetFrame;
  TGVerticalFrame *IrradiationLabelFrame;
  
  
  TGNumberEntry *XMAXentry;
  TGNumberEntry *YMAXentry;
  TGNumberEntry *Pitchentry;
  TGNumberEntry *Widthentry;
  TGNumberEntry *Gainentry; //!!!
  TGNumberEntry *ScreenedGainEntry; //!!!
  TGNumberEntry *ACEntry; //!!!
  TGNumberEntry *Dopingentry; //!!!
  TGNumberEntry *GainRatioentry;
  TGNumberEntry *GainIndententry;
  TGNumberEntry *IrradiationEntry;
  TGNumberEntry *IrradiationEntry2;
  TGNumberEntry *IrradiationEntry3;
  TGNumberEntry *QuenchEntry;
  TGNumberEntry *DJEntry;
  TGNumberEntry *DJehEntry;
  TGNumberEntry *Biasentry;
  TGNumberEntry *Depletionentry;
  TGNumberEntry *PlotUpdateEntry;
  TGNumberEntry *LessPlotEntry;
  TGNumberEntry *Less2DPlotEntry;
  TGNumberEntry *BfieldEntry;
  TGNumberEntry *TempEntry;
  TGNumberEntry *WhereCut;
  TGNumberEntry *WhereCut2;		
  TGNumberEntry *CarriersInNumberentry;
  TGNumberEntry *CarriersAngleNumberentry;
  TGNumberEntry *EdgeNumberentry;
  //TGNumberEntry *Chargeentry;//original particle selection
  //TGNumberEntry *Calibentry;//original particle selection
  //TGNumberEntry *RangeEntry;//original particle selection
  TGNumberEntry *ParticleSpecificsEntry;//dropdwonlist particle selection
  TGNumberEntry *PrecisionEntry;
  TGNumberEntry *SamplingEntry;
  TGNumberEntry *PrintEntry;
  TGNumberEntry *StepyEntry;
  TGNumberEntry *StepxEntry;
  TGNumberEntry *NumberEntry;
  TGNumberEntry *OscBWEntry;
  TGNumberEntry *BBImpEntry;
  TGNumberEntry *CSAImpEntry;
  TGNumberEntry *CDEntry;
  TGNumberEntry *LDEntry;
  TGNumberEntry *TFallEntry;
  TGNumberEntry *CSAVthEntry;
  TGNumberEntry *ShTransEntry;
  TGNumberEntry *ShImpEntry;
  TGNumberEntry *ShNoiseEntry;
  
  TGNumberEntry *BBVthEntry;
  TGNumberEntry *BBNoiseEntry;
  TGNumberEntry *BBBWEntry;
  TGNumberEntry *BBGainEntry;
  
  
  TGNumberEntry *TRiseEntry;
  TGNumberEntry *EventsEntry;
  TGNumberEntry *EventsEntryStart;
  TGRadioButton *TypeButton[3];
  TGRadioButton *BulkButton[2];
  TGRadioButton *StripButton[2];
  TGRadioButton *ParticleIrrButton[2];
  TGRadioButton *DopingGLButton[2];
  TGRadioButton *StepYButton[2];
  TGRadioButton *StepXButton[2];
  TGRadioButton *DJButton[2];
  //TGRadioButton *SelectPartButton[7];//original particle selection
  TGRadioButton *ReadOutButton[3];
  TGCheckButton *CurrentsButton[3];
  TGCheckButton *Eyes;
  TGCheckButton *AbsCur;
  TGCheckButton *ShowCur;
  TGCheckButton *UpdatePlotCheck;
  TGCheckButton *LessPlotCheck;
  TGCheckButton *Less2DPlotCheck;
  TGCheckButton *OscOnButton;
  TGCheckButton *NA62OnButton;
  TGCheckButton *FileNameOnButton;
  TGCheckButton *BatchOnButton;
  TGCheckButton *BatchRandomButton;
  TGCheckButton *ForceGainButton;
  TGCheckButton *ScreenedGainButton;
  TGCheckButton *PrecisionGainButton;
  TGCheckButton *ACButton;
  TGCheckButton *IrradiationOnButton;
  TGCheckButton *AcceptorCreationButton;
  TGCheckButton *LnAcceptorCreationButton;
  TGCheckButton *GainQuenchingButton;
  TGCheckButton *InitialDopRemovalButton;
  TGCheckButton *AllOnButton;
  TGCheckButton *DJOnButton;
  TGCheckButton *AdaptiveThicknessButton;
  TGCheckButton *WFButton;
  
  TGLabel *XMAXLabel;
  TGLabel *YMAXLabel;
  TGLabel *PitchLabel;
  TGLabel *WidthLabel;
  TGLabel *GainLabel;	//!!!
  TGLabel *DopingLabel;	//!!!
  TGLabel *GainRatioLabel;    
  TGLabel *ForceGainLabel;
  TGLabel *ACLabel;
  TGLabel *GainIndentLabel;
  TGLabel *IrradiationLabel;
  TGLabel *IrradiationLabel2;
  TGLabel *IrradiationLabel3;
  TGLabel *DJLabel;
  TGLabel *DJehLabel;
  TGLabel *BiasLabel;
  TGLabel *DepletionLabel;
  TGLabel *QELabel;
  TGLabel *QHLabel;
  TGLabel *QEHLabel;
  TGLabel *QEGLabel;
  TGLabel *QHGLabel;
  TGLabel *QEHGLabel;
  TGLabel *QETotLabel;
  TGLabel *QHTotLabel;
  TGLabel *QTotLabel;
  TGLabel *PlotUpdateLabel;
  TGLabel *LessPlotLabel;
  TGLabel *Less2DPlotLabel;
  TGLabel *BulkLabel;
  TGLabel *StripLabel;
  TGLabel *QHTextLabel;
  TGLabel *QETextLabel;
  TGLabel *QEHTextLabel;
  TGLabel *QHGTextLabel;
  TGLabel *QEGTextLabel;
  TGLabel *QEHGTextLabel;
  TGLabel *QETotTextLabel;
  TGLabel *QHTotTextLabel;
  TGLabel *QTotTextLabel;
  TGLabel *LorentzhTextLabel;
  TGLabel *LorentzeTextLabel;
  TGLabel *LorentzeLabel;
  TGLabel *LorentzhLabel;
  TGLabel *LThTextLabel;
  TGLabel *LTeTextLabel;
  TGLabel *LTeLabel;
  TGLabel *LThLabel;
  TGLabel *CalculatingLabel;
  TGLabel *CalculatingLabel2;
  TGLabel *DriftCutLabel;
  TGLabel *WeightingCutLabel;
  TGLabel *FieldLabel;
  TGLabel *FieldLabel2;
  TGLabel *ElectronsLabel;
  TGLabel *RCLabel;
  TGLabel *ScopeLabel;
  TGLabel *HolesLabel;
  TGLabel *GainElectronsLabel;
  TGLabel *GainHolesLabel;
  TGLabel *TotalLabel;
  TGLabel *CarriersInLabel;
  TGLabel *EdgeLabel;
  TGLabel *TempLabel;
  //TGLabel *RangeLabel;//original particle slection
  TGLabel *ParticleSpecificsLabel1;//dropdownlist particle selection
  TGLabel *ParticleSpecificsLabel2;//dropdownlist particle selection
  TGLabel *ParticleSpecificsLabel3;//dropdownlist particle selection
  TGLabel *ParticleSpecificsLabel4;//dropdownlist particle selection
  //TGString *ParticleSpecificsLabelName;//dropdownlist particle selection
  TGLabel *PrintLabel;
  TGLabel *PrecisionLabel;
  TGLabel *SamplingLabel;
  TGLabel *StepLabel;
  TGLabel *NumberLabel;
  TGLabel *OscBWLabel;
  TGLabel *ImpLabel;
  TGLabel *CDLabel;
  TGLabel *TRiseLabel;
  TGLabel *TFallLabel;
  TGLabel *VthLabel;
  TGLabel *ShTransLabel;
  TGLabel *ShNoiseLabel;
  
  
  TGLabel *BBBWLabel;
  TGLabel *BBNoiseLabel;
  
  TGLabel *EventsLabel;
  TGLabel *EventsLabel2;
  TGTextButton *CalculateButton;
  // TGButton *CalculateButton;
  TGTextButton *SetButton;
  TGTextButton *StopButton;
  TGTextButton *ExitButton;
  TGTextButton *BetweenStripsButton;
  TGTextButton *BetweenStripsButton2;
  TGTextButton *OnStripsButton;
  TGTextButton *OnStripsButton2;
  //		TGTextButton *EtotButton;
  TGTextButton *ExButton;
  TGTextButton *EyButton;
  //	TGTextButton *EtotButton2;
  TGTextButton *ExButton2;
  TGTextButton *EyButton2;
  TGTextButton *CalcPotButton;
  TGTextButton *DrawCutsUserEntry;
  TGTextButton *DrawCutsUserEntry2;
  TGTextButton *FileNameButton;
  TGLabel *FileNameLabel;
  // Added saving gui buttons
  TGTextEntry *FileNameEntry;
  TGTextEntry *SaveFileName;
  TGTextEntry *SaveFileNameWF;
  TGTextButton *SaveButton;
  TGTextButton *LoadButton;
  
  TGTextEntry *SaveFileName1;
  TGTextButton *SaveButton1;
  
  //TGComboBox *SetAlpha;		//!!!! original particle selection
  TGHorizontalFrame *fSliceExtentL;
  TGComboBox *fSliceLengthUnit;
  
  TGLabel *fSliceExtentLLabel;
  
  TGraph driftcut2;
  
  Potentials dwpot;			// potentials
  Field **wf;					// weighting field
  Field **df;					// drift field
  Carriers *carriers;	// carriers
  TGCanvas *fCanvas;		
  
  TCanvas *canvasp;		// canvas for drift potential histogram
  TCanvas *canvaspc;		// canvas for drift potential cuts
  //		TCanvas *canvaspf;		// canvas for drift potential fields
  TCanvas *canvasw;		// canvas for weighting potential histogram
  TCanvas *canvaswc;		// canvas for weighting potential cuts
  // TCanvas *canvaswf;		// canvas for weighting potential fields
  TCanvas *curcanvas;		// canvas for currents
  TCanvas *osccanvas;		// canvas for oscilloscope output
  TCanvas *eleIIcanvas;		// canvas for electronics II output
  
  TH2F* wfhist;	// wfhist = weighting field histogram, dfhit = drift potentials histogram
  TH2F* dfhist;

  TH2F* wfhist1;	// wfhist = weighting field histogram, dfhit = drift potentials histogram
  TH2F* dfhist1;

  
  TH2F* chist;	// Histogram to store the charge
  TH2F* chhist;	// Histogram to store the charge h
  TH2F* ctothist;	// Histogram to store the charge tot
  TH1F* jhist;		
  TH1F* BBjhist;
  TH1F* tvthhist;
  TH1F* BBtvthhist;
  TH2F* twtothist;
  TH2F* Exhist;
  TH1F* Enhist;
  TH1F* MeasEnhist;
  TH2F* BBtwtothist;
  TF1* FitLandau;
  TF1* FitLandau1;
  TThread *PotentialThread;	// thread for calculation of potentials
  TThread *CurrentsThread;
  
  TH2F* dhist;	// histograms:
  TH2F* whist;	// dhist = drift potentials histogram, whist = weighting potentials histogram

  TH2F* dhist1;	// histograms:
  TH2F* whist1;	// dhist = drift potentials histogram, whist = weighting potentials histogram
  
  TLine *NLine1;
  TLine *NLine2;
  
  TEllipse *ellipse;
  TBox *bb;
  TGHProgressBar *CurrentsProgressBar;
  
 public:
		double Stepx;
		double Stepy;
		double B;		// magnetic field
		double wherecut;	// position draw drift potential cut where? (x coordinate)
		double wherecut2;	// position draw weighting potential cut where?
		
		TGraph *driftcut;	// graph for drift potential cut
		TGraph *weightcut;	// graph for weighting potential cut
		TGraph *dfieldcut;	// graph for drift field cut
		TGraph *bdfieldcut;	// graph for breakdown voltage
		TGraph *dgaincut;	// graph for gain
		TGraph *dgaincut_h;	// graph for gain
		TGraph *wfieldcut;	// graph for weighting potential cut
		TGraph *dfieldxcut;	// graph for drift field cut
		TGraph *wfieldxcut;	// graph for weighting potential cut
		TGraph *dfieldycut;	// graph for drift field cut
		TGraph *wfieldycut;	// graph for weighting potential cut
		TGraph *dopdens;	// graph for doping density

		WFGUI(const TGWindow *p,UInt_t w,UInt_t h, TApplication *app);	// constructor of GUI
		void CallSetDetType();
		//void CallSetDetType(Bulk);
		//		void CallSetDetType();				
		int CallGetDetType();
		int CallGetDopingBulk();
		int CallGetDopingStrip();
		int CallGetDJType();
		int GetParticleType();
		int GetMPV();
		void SetParticleType(int);
		void CallSetDJType();
		void CallCalculatePotentials();			// Method to Calculate Potentials
		void CallSetDopingBulk();					// Method to set bulk n-type, strips p-type
		void CallSetParticleIrr();					// Method to set irradiaiotn particle
		void CallSetDopingGL(Int_t);					// Method to set irradiaiotn particle
		void CallSetCSA(int);					// Method to set irradiaiotn particle
		void CallSetReadOut();					// Method to set bulk p-type, strips n-type
		void CallSetDopingStrip();// readout
		void SetGainKind(Int_t);
		int GetGainKind();
		void SetGainDoping(Int_t);
		int GetGainDoping();
		void SetGainShape(Int_t);
		int GetGainShape();
		void SetGainDepth(double);
		double GetGainDepth();
		void SetGainLength(double);
		double GetGainLength();
		void SetGainExp(double);
		double GetGainExp();
		float GetACValue();


		int CallSetPart(Int_t); //!!!!!!
		//bool CallSetAlpha();//!!!!!! //original particle selection
		int CallBoundaryConditions();			// Method to set boundary conditions
		void CallCalculateFields();				// Method to calculate fields
		void CallCalculateCurrents();				// Method to calculate currents
		//		static void* StartPotentialCalcTh(void*);
		void CloseWindow();						// Method to close window
		void SaveData(); // Method to save plots
		void LoadData(); // Method to save plots
		void SaveGraph(); // Method to save graph
		TH2F* Getwhist();						// Method which returns whist
		TH2F* Getdhist();						// Method which returns dhist
		TH2F* Getchist();						// Method which returns dhist
		TH2F* Getchhist();						// Method which returns dhist
		TH2F* Getctothist();						// Method which returns dhist 
		TH2F* GetExhist();						// Method which returns dhist 
		TH1F* GetEnhist();						// Method which returns dhist
		TH1F* GetMeasEnhist();						// Method which returns dhist 

		TCanvas* Getcanvasp();					// Method which returns canvasp
		TCanvas* Getcanvaspc();					// Method which returns canvasp
		TCanvas* Getcanvasw();					// Method which returns canvasw
		TCanvas* Getcurcanvas();					// Method which returns curcanvas
		TCanvas* Getosccanvas();					// Method which returns osccanvas
		double GetGainvalue(double, int, int);
		void FillHist();						// Method to draw histograms of drift and weighting potential
		void DrawFieldHist();					// Method to draw field histogram
		TLine* GetNLine1();
		TLine* GetNLine2();
		TEllipse* GetEllipse();
		virtual ~WFGUI();						// destructor of GUI
		static void* StartPotentialCalcTh(void*);
		virtual Int_t ThreadstartPotential();
		virtual Int_t ThreadstopPotential();
		static void* StartCurrentsCalcTh(void*);
		virtual Int_t ThreadstartCurrents();
		virtual Int_t ThreadstopCurrents();
		bool Getstopped();
		void Setstopped(Bool_t);
		//bool GetStopOn();
		int SetStopOn();
		TThread* GetPotentialThread();
		void KillPotentialThread();
		bool Getplotupdate();
		bool GetLessPlot();
		bool GetLess2DPlot();
		void SetPlotUpdate(Bool_t);
		void SetBField(Bool_t);
		void SetChargeCloud(Bool_t);
		void SetLessPlot(Bool_t);
		void SetLess2DPlot(Bool_t);
		
		void SetDiffusion(Bool_t);
		void SetPlotFieldHist(Bool_t);
		void SetAbsCur(Bool_t);
		void SetShowCur(Bool_t);
		bool GetShowCur();
		bool GetChargeCloud();
		TGLabel* GetCalculatingLabel();
		TGLabel* GetCalculatingLabel2();
		void DrawCutGraph(int);
		void DrawAllGraph(int);
		void DrawStrips();
		void DrawFieldsAbs(int);
		void DrawDopDensGraph(int);
		//static void* StartPotentialCalcTh(void*);
		void SetCutOnStrips();
		void DrawAll();
		void DrawAllw();
		// static void* SetCutOnStrips(void*);
		void SetCutBetweenStrips();
		void DrawFieldsAbsX();
		void DrawFieldsAbsY();
		bool Getdiffusionon();
		bool Getabscuron();
		//void DrawCurrents();
		TGHProgressBar* GetProgressBar();
		double GetB();
		double GetT();
		double InitialDopingRem(double, double, int);
		
		void SetQLabel(const char*,const char*,const char*,const char*,const char*,const char*,const char*,const char*,const char*);
		void SetAngleLabel(const char*, const char*);
		void SetLTLabel(const char*, const char*);
		void SetGainon(bool);		//sets gain layer active
		bool GetGainon();			//returns if gain layer is active
		void SetYGain(double);	
		void SetAllButton(int);
		double GetYGain();	
		void SetGainRatio(double); 
		double GetGainRatio();
		void SetGainLayerVdepletion(double); 
		double GetGainLayerVdepletion();
		void SetGainRegionVdepletion(double); 
		double GetGainRegionVdepletion();
		void SetPairs(double); 
		double GetPairs();
		void SetBField(double);
		double GetBField();	    
		void SetVBias(double); 
		double GetVBias();
		void SetBiasEntry(double); 
		double GetBiasEntry();
		void SetVDepl(double); 
		double GetVDepl();
		void SetNumPairs(double); 
		double GetNumPairs();
		void SetXEntry(double); 
		double GetXEntry();
		void SetAngleEntry(double); 
		double GetAngleEntry();
		int GetDimMaxCarriers();
		double GetYMax();
		double GetXMax();
		double GetNStrips();
		double GetChargeEntry();
		double GetScreenedGainEntry();
		double GetCalibEntry();
		void SetFileName(string*);
		string* GetFileName();
		//		string* CallSetFileName(string);
		string* CallSetFileName();
		void SetPrecision(int);
		
		void SetAlphaRange(double);
		double GetAlphaRange();
		void SetXRayRange(double);
		double GetXRayRange();
		int GetPrecision();
		void SetSampling(double);
		double GetSampling();
		void SetPrintSampling(int);
		int GetPrintSampling();
		void SetStepx(double);
		double GetStepx();
		void SetStepy(double);
		double GetStepy();
		void SetNumberP(int);
		int GetNumberP();
		void SetConstQFlag(bool);
		bool GetConstQFlag();
		void SetUniformQFlag(bool);
		bool GetUniformQFlag();
		void SetUserUniformQFlag(bool);
		bool GetUserUniformQFlag();

		void SetCalibFlag(bool);
		bool GetCalibFlag();



		void SetCDet(double);
		double GetCDet();
		void SetLDet(double);
		double GetLDet();
		void SetOscBW(double);
		double GetOscBW();
		void SetBBImp(double);
		double GetBBImp();
		void SetCSAImp(double);
		double GetCSAImp();
		void SetTRise(double);
		double GetTRise();
		void SetTFall(double);
		double GetTFall();

		double GetShTrans();
		void SetShTrans(double);
		double GetShNoise();
		void SetShNoise(double);

    
		void SetFluence(double);
		double GetFluence();

		void SetQuenchCoef(double);
		double GetQuenchCoef();

        void SetBetaElectrons(double);
        double GetBetaElectrons();
    
        void SetBetaHoles(double);
        double GetBetaHoles();
    
	void SetDopinggainlayerValue(double);
	double GetDopinggainlayerValue();
	
	void SetDJValue(double);
	double GetDJValue();
	
	
	
	void SetDJehValue(double);
	double GetDJehValue();
	


	void SetBBVth(double);
	double GetBBVth();
	void SetBBNoise(double);
	double GetBBNoise();
	
	
	void SetBBBW(double);
	double GetBBBW();
	void SetBBGain(double);
	double GetBBGain();
	
	
	
	double GetCSAVth();
	void SetCSAVth(double);
	
	void SetCSAFTVth(double);
	double GetCSAFTVth();
	void SetCSATVth(double);
	double GetCSATVth();
	
	void SetJitter(double);
	double GetJitter();
	void SetFJitter(double);
	double GetFJitter();
	
	
		
	void SetBBJitter(double);
	double GetBBJitter();
	void SetBBFTVth(double);
	double GetBBFTVth();
	void SetBBTVth(double);
	double GetBBTVth();
	
	void SetSWidth(double);
	double GetSWidth();
	void SetOscOn(bool);
	bool GetOscOn();
	
	void SetNA62On(bool);
	bool GetNA62On();
	
	void SetSC_CSAOn(bool);
	bool GetSC_CSAOn();
	
	void SetFileNameOn(bool);
	bool GetFileNameOn();
	
	void SetBatchOn(bool);
	void SetBatchRandomOn(bool);
	void SetForceGain(bool);
	void SetScreenedGain(bool);
	void SetWF(bool);
	void SetPrecisionGain(bool);
	void SetAC(bool);
	bool GetBatchOn();
	bool GetBatchRandomOn();
	bool GetForceGain();
	bool GetScreenedGain();
	bool GetPrecisionGain();
	bool GetAC();
	void SetCCEOn(bool);
	bool GetCCEOn();
	
	void SetInitialDopRemoval(bool);
	void SetAllOn(bool);
	
	void SetStyle();
	void SetTGraphStyle(TGraph*, double);	
	void SetHStyle(TH1F*, double);	
	bool GetInitialDopRemoval();
	void SetAcceptorCreation(bool);
	bool GetAcceptorCreation();
	void SetLnAcceptorCreation(bool);
	bool GetLnAcceptorCreation();

	void SetGainQuenching(bool);
	bool GetGainQuenching();
	
	void SetDJOn(bool);
	bool GetDJOn();
	void SetEvNumber(int);
	int GetEvNumber();
	void SetEvent(int);
	int GetEvent();
	void SetGainRecess(int);
	int GetGainRecess();
	float Getygainlayerlow();
	float Getygainlayerhigh();
	void Setygainlow(float);
	void Setygainhigh(float);
	float Getygainlow();
	float Getygainhigh();
	double GetAngle();


	//Bulk
	double GetEion();		
	

	
	//void SetAngle(double);
	
	ClassDef(WFGUI,0);  // muss auskommentiert sein wenn .x ... .cxx+	
};
