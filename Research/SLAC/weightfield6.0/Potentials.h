#include <iostream>
#include <math.h>
#include <string.h>
#include "TROOT.h"
#include "TH2F.h"
#include "TH1.h"
#include "TF1.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TColor.h"
#include "TExec.h"
#include "TCanvas.h"
#include "TGButton.h"
#include "TGFrame.h"
#include "TGraph.h"
#define NTYPE 0		
#define PTYPE 1
#define GRIDDISTX  0.000001	// grid distance (1um) in m
#define GRIDDISTY  0.000001	// grid distance (1um) in m

class WFGUI;

#ifndef Potentials_H
#define Potentials_H

class Potentials {

	private:
		int XMAX;    			// detector width
		int YMAX;    			// detector height
		double BinSizex;
		double BinSizey;
		double pitch;			// Pitch
		double width;			// strip width
		int count;			// number of strips
		double vbias;			// bias voltage
		double vdepl;			// depletion voltage without irrdiation effects
		double vdepl_irr;	       	// depletion voltage with irradiation effects
		double TrueXMAX;	
		double TrueYMAX;	

		double poissonf;		// right side of Poisson equation
		int **fix;			// ag for fixed potentials [electrodes]: 1=normal electrode, 2=readout electrode, 0=everywhere else
		int ref; 				// ref=0: finest grid, ref=1: 1. coarser grid, ref=2: 2. coarser grid
		double **wpot; 		// weighting potential
		double **dpot; 		// drift potential
		double **dopyx; 		// doping density xy map
		double *dopdensity;     // Doping density

		unsigned char strips;	// doping of strips
		unsigned char bulk;		// doping of bulk
		int multig;			// number of grids
		int finestX;			// reference parameter to finest grid (finest = XMAX of finest grid)
		int finestY;			// reference parameter to finest grid (finest = YMAX of finest grid)
		int mipcharge;		// number of electrons/holes created
		bool alpha_above;	//if alpha particle comes from above or not

	public:
		Potentials();						
		Potentials(int, int,double,double);	// constructor (YMAX,XMAX,pitch,width)
		Potentials(int,int);				// constructor (YMAX,XMAX)
		virtual ~Potentials();				// destructor
		void SetV(double,double);			// set bias and depletion voltage
		void SetwPotential(int, int, double);	// set weighting potential(i,j)
		void SetdPotential(int, int, double);	// set drift potential(i,j)
		void Setdopyx(int, int, double);	// set doping yx (i,j)
		void SetPitchWidthXY(void*, int, int,double,double, double, double); //Set YMAX, XMAX, pitch width Stepx Stepy
		//void SetPitchWidthXY(int, int,double,double, double, double); //Set YMAX, XMAX, pitch width Stepx Stepy
		void SetPitchWidthXYG(int, int); //Set YMAX, XMAX, pitch width Stepx Stepy
		double Getdpot(int,int);				// get drift potential(i,j)
		double Getwpot(int,int);				// get weighting potential(i,j)
		double Getdopyx(int,int);				// get doping(i,j)
		int Getfix(int,int);				// get fix matrix (i,j)
		int GetXMAX();						
		int GetYMAX();
		double GetBinSizex();
		double GetBinSizey();
		//		void SetXMAX(int);						
		//  void SetYMAX(int);						
		int Getref();		
		int Getmipcharge();
		void Setmipcharge(int);
		double Getvbias();					
		double Getpitch();					
		double Getwidth();					
		void SetBoundaryConditions(bool);			// set strips/backplane to bias voltage/zero, depending on doping
		void Restriktor();					// method to restrict potentials to a coarser grid, with XMAX/2+1
		//int** FixRestriktor();				// method to restrict fix matrix to coarser grid
		void FixRestriktor(int**);
		void Prolongation(bool);				// method to prolongate potentials to finer grid with XMAX*2-1
		void Iteration(void*);	// method for iterative calculation
		void SetDopingProfile(void*, int);	// method to fill the doping profile
		void Smoothing();	// method to smooth the potential
		void Multigrid(void*, bool);	// method for multigrid calculation
		void DriftPal(); 							// Color palette for driftfield (linear)
		void WeightPal(); 							// Color palette for weighting field (linear)
		void SetDoping(unsigned char, unsigned char);	// method to set doping of strips and bulk
		unsigned char GetDoping();
		void SetAbove(bool);	//method to set alpha particles coming from above
		bool GetAbove();
		

		ClassDef(Potentials,0); 
};

#endif
