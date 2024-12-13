#include <iostream>
#include <iomanip>
#include <string.h>
#include <math.h>
#include <istream>
#include <TLatex.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TArrow.h>
#include "string.h"
#include <sstream>
#include "NCutil.h"
#include <fstream> 

using namespace std;

const Int_t n = 10000;
Float_t Time[n];
Float_t Jitter[n];
Float_t TimeSc[n];
Float_t ie[n];
Float_t ih[n];
Float_t ieg[n];
Float_t ihg[n];
Float_t itot[n];
Float_t itotSc[n];
Float_t vout[n];
Float_t q[n];
Float_t qSc[n];

Float_t  qe = 0.;
Float_t  qeg = 0.;
Float_t  qh = 0.;
Float_t  qhg = 0;
Float_t  qtot = 0;
Float_t  qtotg = 0;
Float_t  qtotng = 0;


Float_t gainMip[5];
Float_t gainAT[5];
Float_t gainAB[5];
Float_t bias[5];




TGraph *Currents;

Int_t Calculate_TW ()
{

  NCanvas(1,2,"pippo");
  TH1F *HTW = new TH1F("HTW"," ",360, 0.,6.);
  TH1F *HJ = new TH1F("HJ"," ",200, 0.,200.);

  ifstream myfile ("wf_SCA_BV400_DV50_W300_H200_SP300_SW290_G0_T300_A0_D0_BW1_MIPL0.txt_TW");
 //ifstream myfile ("wf_SCA_BV400_DV50_W300_H200_SP300_SW290_G4_T300_A0_D0_BW1_MIPL0.txt_TW");
  ifstream myfile ("wf_2Vth_BV600_DV50_W300_H200_SP300_SW290_G4_T300_A0_D0_BW1_MIPL0.txt_TW");
  ifstream myfile ("wf_SCA_Sh6_BV300_DV50_W300_H100_SP300_SW290_G4_T300_A0_D0_BW1_MIPL0.txt_TW");
  ifstream myfile ("wf_SCA_Sh3_BV300_DV50_W300_H50_SP300_SW290_G4_T300_A0_D0_BW1_MIPL0.txt_TW");
  ifstream myfile ("wf_SCA_Sh6_BV200_DV50_W300_H100_SP300_SW290_G4_T300_A0_D0_BW1_MIPL0.txt_TW");
  ifstream myfile ("wf_SCA_BV500_DV50_W300_H50_SP300_SW290_G4_T300_A0_D0_BW1_MIPL0.txt_TW");
  ifstream myfile ("wf_SCA_sh1_BV400_DV50_W400_H50_SP400_SW390_G4_T300_A0_D0_BW1_MIPL0.txt_TW");
  ifstream myfile ("wf_BV300_DV50_W300_H50_SP300_SW290_G4_T300_A0_D0_BW1_SH1_MIPL0.txt_TW");
  cout << " Reading File " << endl;
  Int_t ii=1;
  Int_t IMax=0;
  
  while (! myfile.eof() )
    {
      getline (myfile,line);
      istringstream iss(line);
      iss >> Time[ii] >> Jitter[ii];
      if (Time[ii] >0 ) 
	{
	  HTW->Fill(Time[ii]);
	  HJ->Fill(Jitter[ii]);
	}
      cout << Time[ii] << endl;
      ii++;
    } while (iss);
  myfile.close();	  	  
 NStatOn(HTW);
 NSetTitle(HTW, "time [ns]","Entries");
 pippo->cd(1);
 HTW->Draw();
 pippo->cd(2);
 HJ->Draw();
 NSetTitle(HJ, "time [ps]","Entries");

  return;
}

Int_t readfile (Int_t nn = 1, Int_t v=2) {

  // TCanvas *Currents = NCanvas(1,1,"Currents");
  //TCanvas *Signals = NCanvas(1,1,"Signals");
  // Currents = new TGraph(dwpot.GetYMAX(),q,wabs);

  if (nn == 1)
    {  
      if (v == 2)
	{
	  ifstream myfile ("output/wf_BV200_DV70_W400_H300_SP400_SW390_G1.1_T300_D0_BW1.5_MIPU75.txt");
	  ifstream myfileSc ("../Signals/mip/200_M1.txt");
	  string StVolt = " 200 V";
	}
      else if (v == 3)
	{
	  ifstream myfile ("output/wf_BV300_DV70_W400_H300_SP400_SW390_G1_T300_D0_BW1.5_MIPU75.txt");
	  ifstream myfileSc ("../Signals/mip/300_M2.txt");
	  string StVolt = " 300 V";
	}
      else if (v == 4)
	{
	  ifstream myfile ("output/wf_BV400_DV70_W400_H300_SP400_SW390_G0.93_T300_D0_BW1.5_MIPU75.txt");
	  ifstream myfileSc ("../Signals/mip/400_M3.txt");
	  string StVolt = " 400 V";
	}
      else if (v == 5)
	{
	  ifstream myfile ("output/wf_BV500_DV70_W400_H300_SP400_SW390_G0.86_T300_D0_BW1.5_MIPU75.txt");
	  ifstream myfileSc ("../Signals/mip/500_M4.txt");
	  string StVolt = " 500 V";
	}
      else if (v == 6)
	{
	  ifstream myfile ("output/wf_BV600_DV70_W400_H300_SP400_SW390_G0.8_T300_D0_BW1.5_MIPU75.txt");
	  ifstream myfileSc ("../Signals/mip/600_f1.txt");
	  string StVolt = " 600 V";
	}

    }
  else if (nn == 2)
    {
      if (v == 2)
	{
	  ifstream myfile ("output/wf_BV200_DV70_W400_H300_SP400_SW390_G1.1_T300_D0_BW1.5_AT10.txt");
	  ifstream myfileSc ("../Signals/alpha_top/200_M1.txt");
	  string StVolt = " 200 V";
	}
      else if (v == 3)
	{
	  ifstream myfile ("output/wf_BV300_DV70_W400_H300_SP400_SW390_G1_T300_D0_BW1.5_AT10.txt");
	  ifstream myfileSc ("../Signals/alpha_top/300_M2.txt");
	  string StVolt = " 300 V";

	}
      else if (v == 4)
	{
	  ifstream myfile ("output/wf_BV400_DV70_W400_H300_SP400_SW390_G0.93_T300_D0_BW1.5_AT10.txt");
	  ifstream myfileSc ("../Signals/alpha_top/400_M3.txt");
	  string StVolt = " 400 V";

	}
      else if (v == 5)
	{
	  ifstream myfile ("output/wf_BV500_DV70_W400_H300_SP400_SW390_G0.86_T300_D0_BW1.5_AT10.txt");
	  ifstream myfileSc ("../Signals/alpha_top/500_M4.txt");
	  string StVolt = " 500 V";

	}
      else if (v == 6)
	{
	  ifstream myfile ("output/wf_BV600_DV70_W400_H300_SP400_SW390_G0.8_T300_D0_BW1.5_AT10.txt");
	  ifstream myfileSc ("../Signals/alpha_top/600_f1.txt");
	  string StVolt = " 600 V";
	}
    }
  else if (nn == 3)
    {
      if (v == 2)
	{
	  ifstream myfile ("output/wf_BV200_DV70_W400_H300_SP400_SW390_G1.1_T300_D0_BW1.5_AB10.txt");
	  ifstream myfileSc ("../Signals/alpha_bottom/200_M1.txt");
	  string StVolt = " 200 V";

	}
      else if (v == 3)
	{
	  ifstream myfile ("output/wf_BV300_DV70_W400_H300_SP400_SW390_G1_T300_D0_BW1.5_AB10.txt");
	  ifstream myfileSc ("../Signals/alpha_bottom/300_M2.txt");
	  string StVolt = " 300 V";

	}
      else if (v == 4)
	{
	  ifstream myfile ("output/wf_BV400_DV70_W400_H300_SP400_SW390_G0.93_T300_D0_BW1.5_AB10.txt");
	  ifstream myfileSc ("../Signals/alpha_bottom/400_M3.txt");
	  string StVolt = " 400 V";

	}
      else if (v == 5)
	{
	  ifstream myfile ("output/wf_BV500_DV70_W400_H300_SP400_SW390_G0.86_T300_D0_BW1.5_AB10.txt");
	  ifstream myfileSc ("../Signals/alpha_bottom/500_M4.txt");
	  string StVolt = " 500 V";

	}
      else if (v == 6)
	{
	  ifstream myfile ("output/wf_BV600_DV70_W400_H300_SP400_SW390_G0.8_T300_D0_BW1.5_AB10.txt");
	  ifstream myfileSc ("../Signals/alpha_bottom/600_f1.txt");
	  string StVolt = " 600 V";

	}


    }

  Float_t  Vout_Max = 0.;
  Float_t  Vout_Max2 = 0.;
  Float_t  Sc_Max = 0.;
  Float_t  Vout_TMax = 0;
  Float_t  Vout_IMax = 0;
  Float_t  Itot_Max = 0;
  Float_t  Sc_TMax = 0;


  Int_t nl = 0;
  if (myfile.is_open())
    {

      cout << " Reading File " << endl;
      Int_t ii=1;
      Int_t IMax=0;
      

      while (! myfile.eof() )
	{
	  getline (myfile,line);
	  istringstream iss(line);
	  iss >> Time[ii] >> itot[ii] >> ie[ii] >>ih[ii] >>ieg[ii] >>ihg[ii]>> vout[ii];
	  qe += ie[ii];
	  qeg += ieg[ii];
	  qh += ih[ii];
	  qhg += ihg[ii];
	  qtot += itot[ii];
	  qtotng += ie[ii]+ih[ii];

	  if (fabs(vout[ii])>Vout_Max) 
	    {
	      Vout_Max = fabs(vout[ii]);
	    }

	  if (fabs(itot[ii])>Itot_Max) 
	    {
	      Itot_Max = fabs(itot[ii]);
	    }

	  //cout << Itot_Max << endl;
	  //	  cout << " ii = " << ii <<  " " << Time[ii] << " " << itot[ii] << " " << ie[ii]<< " " << ih[ii] << " " << ieg[ii] << " " << vout[ii] << " " << Vout_Max  << endl;
	  ii++;
	  IMax++;
	} while (iss);
      myfile.close();	  	  
    }
  else << "File not found" << endl;
  //  cout << qe << " " << qeg  << " "<< qh  << " "<< qhg  << " "<< qtot  << " "<< qtotng << endl;
  cout << "Gain = " << qtot/qtotng << endl;

  //  cout << Vout_Max << endl;
  //  Vout_Max = fabs(vout[Vout_IMax-5]);
  //cout << Vout_Max << endl;

  ii--;
  IMax--;
  Float_t TIMEUNIT = 0;
  TIMEUNIT = Time[11]-Time[10];
  Vout_IMax = 0;
  for(int k=0;k<IMax;k++) {
    q[k]=Time[k]*1e9; // set t coordinate
    vout[k] *= Itot_Max/Vout_Max*1.2;
    //    cout << k << " " << q[k] << " " << vout[k] << endl;

    if (fabs(vout[k])>Vout_Max2) 
      {
	Vout_Max2 = fabs(vout[k]);
	Vout_IMax = k;
      }
  }
  //  Currents->cd();
  Vout_Max2 = fabs(vout[Vout_IMax-10]);
  cout << Vout_IMax << endl;


 TGraph *gr5;				// Graph for current of gain holes
  gr5 = new TGraph(IMax,q,vout);
  gr5->SetLineColor(kRed);		// set line color to purple
  gr5->SetLineWidth(2);
  gr5->SetTitle("WF");
  gr5->Draw("AL");


  TGraph *gr =  new TGraph(IMax,q,itot);				// Graph for total current
  gr->GetXaxis()->SetTitle("time (ns)");		// set title of x-axis
  gr->GetYaxis()->SetTitle("Current (A)");	// set title of y-axis
  gr->SetLineColor(3);					// set line color to green
  gr->SetTitle(" ");						// set title
  gr->SetLineWidth(2);
  gr->SetTitle("Itot");
  //gr->Draw("L");						// draw graph (axis and line)
  

	
  TGraph *gr1;				// Graph for current of holes
  gr1 = new TGraph(IMax,q,ih);
  gr1->SetLineColor(4);		// set line color to blue
  gr1->SetLineWidth(2);
  gr1->SetTitle("Ih");
  //  gr1->Draw("L");

  
  
  TGraph *gr2;				// Graph for current of electrons
  gr2 = new TGraph(IMax,q,ie);
  gr2->SetLineColor(kGray);		// set line color to red
  gr2->SetLineWidth(2);
  gr2->SetTitle("Ie");
  // gr2->Draw("L");
  
  
  
  TGraph *gr3;				// Graph for current of gain electrons
  gr3 = new TGraph(IMax,q,ieg);
  gr3->SetLineColor(6);		// set line color to lightblue
  gr3->SetLineWidth(2);
  gr3->SetTitle("Ie_gain");
  //gr3->Draw("L");
  
  TGraph *gr4;				// Graph for current of gain holes
  gr4 = new TGraph(IMax,q,ihg);
  gr4->SetLineColor(7);		// set line color to purple
  gr4->SetLineWidth(2);
  gr4->SetTitle("Ih_gain");
  //gr4->Draw("L");



  Int_t TFlag = 0; 
  Int_t nl = 0;
  Float_t Sc_TZero =0;
  if (myfileSc.is_open())
    {
      cout << " Reading File " << endl;
      Int_t iiSc=0;
      Int_t IMaxSc=0;
      
      while (! myfileSc.eof() )
	{
	  getline (myfileSc,line);
	  istringstream iss(line);
	  iss >> TimeSc[iiSc] >> itotSc[iiSc];
	  if (Sc_Max <fabs(itotSc[iiSc])) 
	    {
	      Sc_Max = fabs(itotSc[iiSc]);
	      Sc_TMax = iiSc;
	    }
	  // if (iiSc>3 ) cout << fabs(itotSc[iiSc])/fabs(itotSc[iiSc-2])  << endl;
	  if (iiSc>3 && TFlag == 0 && fabs(itotSc[iiSc])> 2*fabs(itotSc[iiSc-2])) 
	    {
	      Sc_TZero = TimeSc[iiSc];
	      TFlag = 1;
	    }
	  // cout << " ii = " << ii <<  " " << TimeSc[ii] << " " << itotSc[ii] << endl;
	  if (TimeSc[iiSc] != 0)
	    {
	      iiSc++;
	      IMaxSc++;
	    }
	} while (iss);
      myfileSc.close();	  	  
    }
  else << "File not found" << endl;
  
  iiSc--;
  IMaxSc--;
  
  Float_t  Aver=0;
  for(int kSc=0;kSc<5;kSc++) Aver +=itotSc[kSc]/5; 
  Sc_Max +=Aver;
  for(int kSc=0;kSc<IMaxSc;kSc++) {
    qSc[kSc]=(TimeSc[kSc]-Sc_TZero)*1e9; // set t coordinate
    itotSc[kSc] -= Aver;
    itotSc[kSc] *= -1*Vout_Max2/(Sc_Max);
    //    cout << kSc << " " << qSc[kSc] << " " << itotSc[kSc] << endl;
  }
  //  Signals->cd();


  TGraph *grSc =  new TGraph(IMaxSc,qSc,itotSc);				// Graph for total current
  grSc->GetXaxis()->SetTitle("time (ns)");		// set title of x-axis
  grSc->GetYaxis()->SetTitle("Current (A)");	// set title of y-axis
  grSc->SetLineColor(kBlack);					// set line color to green

  grSc->SetLineWidth(2);
  grSc->GetXaxis()->SetRangeUser(-5., 25.); 

  if (nn == 1) string HTitle =   "MIP" ; // grSc->SetTitle("MIP");						// set title
  else if (nn == 2) string HTitle =   "Alpha_top" ; // grSc->SetTitle("Alpha top");						// set title
  else if (nn == 3)  string HTitle =   "Alpha_bottom" ; //
  HTitle += StVolt; 
 
  grSc->SetTitle(HTitle.c_str());						// set title


  grSc->Draw("AL");						// draw graph (axis and line)
  gr->Draw("L");						// draw graph (axis and line)
  gr1->Draw("L");						// draw graph (axis and line)
  gr2->Draw("L");						// draw graph (axis and line)
  gr3->Draw("L");						// draw graph (axis and line)
  gr4->Draw("L");						// draw graph (axis and line)
  gr5->SetLineStyle(2);
  gr5->Draw("L");
  gr->SetFillColor(0);
  gr1->SetFillColor(0);
  gr2->SetFillColor(0);
  gr3->SetFillColor(0);
  gr4->SetFillColor(0);
  gr5->SetFillColor(0);
  grSc->SetFillColor(0);

  //  Signals->BuildLegend();
  gPad->BuildLegend();
  return; 


}

void plot()
{

  TCanvas *Currents = NCanvas(3,5,"Currents");
  //  TCanvas *Signals = NCanvas(1,1,"Signals");
  Currents->cd(1);
  readfile(1,2);
  cout << qtot << " " << qtotng << endl;
  gainMip[0] = qtot/qtotng; 
  bias[0] = 200;
  Currents->cd(2);
  //  readfile(2,2);
  gainAT[0] = 0;
  Currents->cd(3);
  readfile(3,2);
  gainAB[0]  = qtot/qtotng; 

  Currents->cd(4);
  readfile(1,3);
  gainMip[1] = qtot/qtotng; 
  bias[1] = 300;
  
  Currents->cd(5);
  readfile(2,3);
  gainAT[1] = qtot/qtotng;

  Currents->cd(6);
  readfile(3,3);
  gainAB[1] = qtot/qtotng; 

  Currents->cd(7);
  readfile(1,4);
  gainMip[2] = qtot/qtotng; 
  bias[2] = 400;
  Currents->cd(8);
  readfile(2,4);
  gainAT[2] = qtot/qtotng;
  Currents->cd(9);
  readfile(3,4);
  gainAB[2]  = qtot/qtotng; 

  Currents->cd(10);
  readfile(1,5);
  gainMip[4] = qtot/qtotng; 
  bias[4] = 500;
  Currents->cd(11);
  readfile(2,5);
  gainAT[3] = qtot/qtotng;
  Currents->cd(12);
  readfile(3,5);
  gainAB[3]  = qtot/qtotng; 

  Currents->cd(13);
  readfile(1,6);
  gainMip[5] = qtot/qtotng; 
  bias[5] = 600;
  Currents->cd(14);
  readfile(2,6);
  gainAT[4] = qtot/qtotng;
  Currents->cd(15);
  readfile(3,6);
  gainAB[4] = qtot/qtotng; 


  //  readfile(1,3);
  return;
}

void Gain(){

  Float_t bias[5] = {200.,300.,400.,500.,600.};
  Float_t gainMip[5] = {2.39, 2.45, 2.41, 2.42,2.42}; 
  Float_t gainAT[5]= { 0.,   2.32, 2.35, 2.37,2.39};
  Float_t gainAB[5] = {2.397,2.36, 2.38, 2.40, 2.41};
 
  cout <<bias[2] << endl;

  TCanvas *GainC = NCanvas(1,2,"GainC");
  GainC->cd(1);
  TGraph *grGAT;				// Graph for current of gain holes
  grGAT = new TGraph(5,bias,gainAT);
  grGAT->SetMarkerStyle(20);		// set line color to purple
  grGAT->SetMarkerColor(2);		// set line color to purple
  grGAT->SetMarkerSize(1.);		// set line color to purple
  grGAT->GetXaxis()->SetLimits(0.,700.);
  grGAT->GetYaxis()->SetRangeUser(2.,3.);
  //grGAT->GetXaxis->SetLabelSize(1.);		// set line color to purple
  grGAT->SetTitle("Alpha Top");
  //  grGAT->SetMinimu(2.);
  grGAT->Draw("AP");

  grGAT->GetXaxis()->SetTitle("V Bias [V]");		// set title of x-axis
  grGAT->GetYaxis()->SetTitle("Gain");	// set title of y-axis

  TGraph *grGM;				// Graph for current of gain holes
  grGM = new TGraph(5,bias,gainMip);
  grGM->SetMarkerStyle(22);		// set line color to purple
  grGM->SetMarkerColor(4);		// set line color to purple
  grGM->SetMarkerSize(1.);		// set line color to purple
  grGM->SetTitle("MIP");
  grGM->Draw("P");

  TGraph *grGAB;				// Graph for current of gain holes
  grGAB = new TGraph(5,bias,gainAB);
  grGAB->SetTitle("Alpha Bottom");
  grGAB->SetMarkerStyle(24);		// set line color to purple
  grGAB->SetMarkerColor(kGreen);		// set line color to purple
  grGAB->SetMarkerSize(1.);		// set line color to purple
  grGAB->Draw("P");

  grGAB->SetFillColor(0);
  grGAT->SetFillColor(0);
  grGM->SetFillColor(0);

  gPad->BuildLegend();




}




