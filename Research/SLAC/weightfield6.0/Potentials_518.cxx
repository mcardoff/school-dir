//#include "Potentials.h"
#include "WFGUI.h"
//////////////////////////////////////////////////////////////////////
Potentials::Potentials()  //constructor
{	
  multig=0;
  ref=0;
  XMAX=0;
  YMAX=0;
  pitch=0;
  width=0;
  count=0;
  vbias=0;
  vdepl=0;
  vdepl_irr=0;
  poissonf=0;
  dpot=0;
  wpot=0;
  
  strips=0;	
  bulk=1;		
  mipcharge=0;
}
//////////////////////////////////////////////////////////////////////
Potentials::Potentials(int BINY, int BINX)	// constructor 
{

  //  multig=(int)((log10(dimy*dimx/3)/log(4)))+1;
  // cout << " Multigrid = " << multig << endl;
  // multig=(int)( log10(dimy*dimx/3)/log(64) )+1;
  // cout << " Multigrid = " <<endl;
  ref=0;
  //  cout << " Stepx,y " << Stepx << " " << Stepy << endl; 
  XMAX=BINX; //(GRIDDISTX*1e6);
  YMAX=BINY;///(GRIDDISTY*1e6);
  finestX=XMAX; //dimx/(GRIDDISTX*1e6);
  finestY=YMAX; //dimx/(GRIDDISTX*1e6);
  pitch=0;
  width=0;
  count=0;
  poissonf=0;
  strips=0;
  bulk=1;
  mipcharge=75*YMAX; 

  wpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) wpot[i] = new double[XMAX];
  
  dpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) dpot[i] = new double[XMAX];

  
  fix = new int*[YMAX];
  for (int i = 0; i < YMAX; ++i) fix[i] = new int[XMAX];

  
  for(int i=0; i<XMAX; i++) {			// reset potentials to zero as a precaution
    for(int j=0; j<YMAX; j++) {
      dpot[j][i]=0.0;
      wpot[j][i]=0.0;
      fix[j][i]=0;
    }
  }



}
//////////////////////////////////////////////////////////////////////
Potentials::Potentials(int dimy, int dimx, double p, double w) // constructor 
{
  //  cout << " CallC Dimx= " << dimx <<  endl;
  double var = 0.1;
  
  // cout << "Lock4 " <<  __LINE__<< endl;
  //if (dimx != -1) SetPitchWidthXY(dimy, dimx,p,w, var, var);
  return;
}
//////////////////////////////////////////////////////////////////////
void Potentials::SetwPotential(int posy, int posx, double val) {
  wpot[posy][posx]=val;
}
//////////////////////////////////////////////////////////////////////
void Potentials::Setdopyx(int posy, int posx, double val) {
  dopyx[posy][posx]=val;
}
//////////////////////////////////////////////////////////////////////
void Potentials::SetdPotential(int posy, int posx, double val) {
  dpot[posy][posx]=val;
}
//////////////////////////////////////////////////////////////////////
void Potentials::SetV(double bias, double depl) {
  // cout << bias << endl;
  vbias=bias;
  vdepl=depl;
}

/////////////////////////////////////////////////////////////////////
int Potentials::GetXMAX() {
  return XMAX;
}
//////////////////////////////////////////////////////////////////////
int Potentials::GetYMAX() {
  return YMAX;
}

///////////////////////////////////////////////////////////////////////
double Potentials::Getdpot(int posy, int posx) {
  return dpot[posy][posx];
}
///////////////////////////////////////////////////////////////////////
double Potentials::Getwpot(int posy, int posx) {
  return wpot[posy][posx];
}
///////////////////////////////////////////////////////////////////////
double Potentials::Getdopyx(int posy, int posx) {
  return dopyx[posy][posx];
}
//////////////////////////////////////////////////////////////////////
void Potentials::Setmipcharge(int ch) {
  mipcharge=ch;
  return;
}
//////////////////////////////////////////////////////////////////////
int Potentials::Getmipcharge() {
  return mipcharge;
}
/////////////////////////////////////////////////////////////////////
int Potentials::Getfix(int posy, int posx) {
  return fix[posy][posx];
}
//////////////////////////////////////////////////////////////////////
int Potentials::Getref() {
  return ref;
}
//////////////////////////////////////////////////////////////////////
double Potentials::Getvbias() {
  return vbias;
}
//////////////////////////////////////////////////////////////////////
double Potentials::Getpitch() {
  return pitch;
}
//////////////////////////////////////////////////////////////////////
double Potentials::Getwidth() {
  return width;
}
//////////////////////////////////////////////////////////////////////
double Potentials::GetBinSizex() {
  return BinSizex;
}
//////////////////////////////////////////////////////////////////////
double Potentials::GetBinSizey() {
  return BinSizey;
}
//////////////////////////////////////////////////////////////////////
void Potentials::SetDoping(unsigned char s, unsigned char b) {
  strips=s;
  bulk=b;	
}
//////////////////////////////////////////////////////////////////////
void Potentials::SetPitchWidthXY(void *wfgui,int dimy, int nstrip,double p, double w, double Stepx, double Stepy )
{
  //  Stepy = 0.1;
  //   cout << " In Potential SetPitch = " << dimy << " " << nstrip << " " << p << " " << w << " Stepx " << Stepx << " stepy " << Stepy << endl;
  ref=0;
  pitch=p; // here it's in micron
  width=w; // here it's in micron
  //  XMAX=dimx*p;
  // YMAX=dimy;

  
  //  multig=(int)((log10(YMAX*XMAX/3)/log(4)))+1;   // determine the number of multigrids used
  
  //  cout << "Lock3 " <<  __LINE__<< endl;
  //  multig = 2;
  if((int)pitch%2==0) pitch++;
  if((int)width%2==0) width++;      

  count = nstrip;
    
  if(count-(int)count) {
    count=(int)count+1; 
  }
  
  if (((int)count)%2==0)
    {
      count++;		
    }
  // Stepx =  (int) (pitch*count)/100;
  //  Stepy =  (int) (dimy*1E6)/100;
  // cout << " Stepy = " << Stepy << endl;
  // cout << " Stepx = " << Stepx << endl;
  
  XMAX = (int) (pitch*count/Stepx); //here it's in bins
  YMAX= (int) (dimy/Stepy);

  if(YMAX%2==0) YMAX--;
  if(XMAX%2==0) XMAX--;

  // multig=(int)( log10(dimy*dimx/3)/log(2) )+1;
  multig=(int)( log10(YMAX*XMAX/3)/log(2) )+1;
  cout << "Number of multigrid for potential calculation = " << multig << endl;
  //  cout << " dimy " << dimy << " Stepy = " << Stepy << endl;
  //  count = (double)XMAX/pitch;    //number of strips

  
  
  int tempxmax=XMAX;
  int tempymax=YMAX;
  // cout<<"1: xmax= "<<tempxmax<<", ymax= "<<tempymax<<endl;

  for(int i=0; i<multig; i++) { 		//calculates finer grid
    tempxmax=tempxmax/2+1;
    tempymax=tempymax/2+1;
    if((tempxmax)%2==0) XMAX=XMAX+pow(2.0,(i+1));
    if((tempymax)%2==0) YMAX=YMAX+pow(2.0,(i+1)*1.);
  }

  

  // cout<<" After grid adjustments xmax= "<<XMAX<<", ymax= "<< YMAX<<endl;

  BinSizey = 1.*dimy/(1.*YMAX);

  //  if (BinSizey > 0.03)
  //  {
  //    multig ++ ;
  //   cout << " y bin too large: " << BinSizey << " recalculating potential grid" << endl;	
  //       goto MULTI;
  // }
  
  //  BinSizey = Stepy;
  
  BinSizex = 1.*count*pitch/(1.*XMAX);

 //here we change from micron to bins
  //  XMAX = XMAX/BinSizex;
  // YMAX = YMAX/BinSizey;

  finestX=XMAX;
  finestY=YMAX;
  
  pitch=(int) (pitch/BinSizex); // here it's in # bin
  width= (int) (width/BinSizex); // here it's in # bin
  
  cout << "Bin size y = " << GetBinSizey() << " micron " << endl;
  // cout << "Bin size y = " << GetBinSizey() << " micron " << endl;
  cout << "Number of bin: X = " << XMAX<< ", Y =  " << YMAX << endl;

  mipcharge=75*dimy;    // prima era mipcharge=75*YMAX;
  //mipcharge=75*YMAX;

 

  
  
  wpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) wpot[i] = new double[XMAX];
  
  dpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) dpot[i] = new double[XMAX];

  dopyx = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) dopyx[i] = new double[XMAX];
  
  fix = new int*[YMAX];
  for (int i = 0; i < YMAX; ++i) fix[i] = new int[XMAX];
  
  for(int i=0; i<XMAX; i++) {
    for(int j=0; j<YMAX; j++) {
      wpot[j][i]=0.0;
      dpot[j][i]=0.0;
      dopyx[j][i]=0.0;
      fix[j][i]=0;
    }
  }

  //  cout << " In Potential end SetPitch = " << dimy << " " << nstrip << " " << p << " " << w << " Stepx " << Stepx << " stepy " << Stepy << endl;

}
//////////////////////////////////////////////////////////////////////
void Potentials::SetPitchWidthXYG(int XMAX, int YMAX )
{
  //
  
  wpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) wpot[i] = new double[XMAX];
  
  dpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) dpot[i] = new double[XMAX];

  dopyx = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) dopyx[i] = new double[XMAX];
  
  fix = new int*[YMAX];
  for (int i = 0; i < YMAX; ++i) fix[i] = new int[XMAX];
  
  for(int i=0; i<XMAX; i++) {
    for(int j=0; j<YMAX; j++) {
      wpot[j][i]=0.0;
      dpot[j][i]=0.0;
      dopyx[j][i]=0.0;
      fix[j][i]=0;
    }
  }

  //  cout << " In Potential end SetPitch = " << dimy << " " << nstrip << " " << p << " " << w << " Stepx " << Stepx << " stepy " << Stepy << endl;

}
//////////////////////////////////////////////////////////////////////
void Potentials::SetBoundaryConditions( bool ReadOutTopFlag)   // reset electrodes and potentials according to user input
{			  
  for (int i=0; i<XMAX; i++)
    {	// bottom electrode (backplane): set to Vbias for p-type strips, otherwise 0	
      dpot[0][i]=( strips== PTYPE) ? vbias : 0.0;
      wpot[0][i]=(ReadOutTopFlag)? 0.0: 1.0;										             
      fix[0][i]=1;             
    }
  //
  for ( int kk = 1; kk<2;kk++)
    {
      for(int j=0;j<(int)count;j++)	// set strips
	{
	  for(int k=0;k<width;k++)
	    {
	      dpot[YMAX-kk][(int)(XMAX-(int)pitch*(int) count)/2 + j*(int)pitch+(int)pitch/2-(int)width/2+k]=(strips==NTYPE) ? vbias : 0.0;
	      wpot[YMAX-kk][(int)(XMAX-(int)pitch*(int) count)/2 + j*(int)pitch+(int)pitch/2-(int)width/2+k]=0.0;			
	      // cout << " doping " << k << " " << dpot[YMAX-1][(int)(XMAX-(int)pitch*(int) count)/2 + j*(int)pitch+(int)pitch/2-(int)width/2+k] << endl;
	      if(j==((int)count/2))
		{
		  fix[YMAX-kk][(int)(XMAX-(int)pitch*(int) count)/2+j*(int)pitch+(int)pitch/2-(int)width/2+k]=2;
		  if (ReadOutTopFlag)										            
		    wpot[YMAX-kk][(int)(XMAX-(int)pitch*(int) count)/2+j*(int)pitch+(int)pitch/2-(int)width/2+k]=1.0;
		  else
		    wpot[YMAX-kk][(int)(XMAX-(int)pitch*(int) count)/2+j*(int)pitch+(int)pitch/2-(int)width/2+k]=0.0;
		}
	      else
		{
		  fix[YMAX-kk][(int)(XMAX-(int)pitch*(int) count)/2+j*(int)pitch+(int)pitch/2-(int)width/2+k]=1;			
		}
	    }
	}
    }

}

//////////////////////////////////////////////////////////////////////
void Potentials::Iteration(void *wfgui)	// 
{				// method for iterative calculation, see: http://en.wikipedia.org/wiki/Relaxation_%28iterative_method%29
  //int r=1;
  double sum=0;		// [(old potential) - (new potential)]^2/(XMAX*YMAX)
  double err=0.002;
  long it=0;	// iteration index 
  // int N=1;		// every N iterations do plot update

  
  int **tempfix; // allocate memory for fix matrix for current grid
  tempfix = new int*[YMAX];
  for (int i = 0; i < YMAX; ++i) tempfix[i] = new int[XMAX];

  
  //tempfix=FixRestriktor();	// determine fix matrix for current grid
  FixRestriktor(tempfix);
  
  WFGUI* gui= (WFGUI*) wfgui; 


  float binx = gui->GetStepx()*1.*finestX/XMAX;
  float biny =  gui->GetStepy()*1.*finestY/YMAX;
  double fixconst = 0.75; // needed to get the right depletion voltage (?)
  double h2 =  fixconst*biny*biny*1e-12; // matrix square, needed to calculate  the iterative steps

  
  // cout <<  " binyh2 = " << binyh2 << " StepY = " << gui->GetStepy() << " YMAX/finestY= " << 1.*finestY/YMAX << endl;
   
  //  if(XMAX==finestX) N=1000;		// at the finest grid, do plot updates every 600th interation
  // else {
  //  if(XMAX*2-1==finestX) N=700;	// at the first coarser grid, do plot updates every 400th interation
  //  else N=100;				// else, every 100th iteration
  // }

  // float ymin =1;
  // float ymaxbd =YMAX;
  float dist = gui->GetYMax(); // this is linear
  int ymin =1;
  float ymaxbd =YMAX;
  
  if (fabs(vdepl_irr)>vbias && !gui->GetDJOn() && ( ( bulk==PTYPE && strips == NTYPE )  || ( bulk==NTYPE && strips == PTYPE )))
   ymin = max(0.,dist-dist*sqrt(vbias)/sqrt(fabs(vdepl_irr)))/biny;
  if (fabs(vdepl_irr)>vbias && !gui->GetDJOn() && ( ( bulk==PTYPE && strips == PTYPE )  || ( bulk==NTYPE && strips == NTYPE )))
    ymaxbd = dist*sqrt(vbias)/sqrt(fabs(vdepl_irr))/biny;
  // cout << "ymin = " << ymin <<  " Depletion starts at: " << dist-dist*sqrt(vbias)/sqrt(fabs(vdepl_irr)) << endl;
  
  while(1)		// calculate until accuracy is reached
    {
      if(gui->Getstopped()==1) {
	break;
      }
      
      if(XMAX==finestX || XMAX==finestX/2+1) err=0.005; // if calculating on finest grid, set accuracy to 0.001
      

      Potentials newpot(YMAX,XMAX);	// create temporary new object newpot, used for iterative calculation	   
      // inside the grid - use the given equations to average around neighbors

      for (int x=0; x<XMAX; x++)
	{
	  //for (int y=1; y<(YMAX-1); y++)
	  for (int y=ymin; y<(ymaxbd-1); y++)
	    {
	    
	      if (!tempfix[y][x])
		{
		  // weighting potential
		  //nicolo
		  newpot.wpot[y][x]=( 1./biny*wpot[y+1][x]+1./biny*wpot[y-1][x]+1./binx*wpot[y][(x+1+XMAX)%(XMAX)]+1./binx*wpot[y][(x-1+XMAX)%(XMAX)])/(2./(biny)+2./(binx));

		  if (gui->CallGetDetType() == 0 || gui->CallGetDetType() == 2)
		    {
		      // newpot.dpot[y][x]=( (1./biny*dpot[y+1][x]+ 1./biny*dpot[y-1][x]+ 1./binx*dpot[y][(x+1+XMAX)%(XMAX)]+
		      //		   1./binx*dpot[y][(x-1+XMAX)%(XMAX)])/(2./(biny)+2./(binx))+0.25*h2*(dopyx[y][x]));

		        newpot.dpot[y][x]=0.25*(dpot[y+1][x]+dpot[y-1][x]+dpot[y][(x+1+XMAX)%(XMAX)]
						+dpot[y][(x-1+XMAX)%(XMAX)]+h2*(dopyx[y][x])*ECHARGE/(EPSILON*EPSILONR));

			//			newpot.dpot[y][x]=0.25*(dpot[y+1][x]+dpot[y-1][x]+dpot[y][(x+1+XMAX)%(XMAX)]
			//			+dpot[y][(x-1+XMAX)%(XMAX)]+h2*(dopyx[y][x])*ECHARGE/(EPSILON*EPSILONR));

		      // newpot.dpot[y][x]=0.25*(dpot[y+1][x]+dpot[y-1][x]+dpot[y][(x+1+XMAX)%(XMAX)]
		      //		      +dpot[y][(x-1+XMAX)%(XMAX)]+h2*0.25*(dopyx[y+1][x]+dopyx[y-1][x]+dopyx[y][(x+1+XMAX)%(XMAX)]
		      //						      +dopyx[y][(x-1+XMAX)%(XMAX)])*ECHARGE/(EPSILON*EPSILONR));
			      
		    }
		  else
		    {
		      newpot.dpot[y][x]=( 1./biny*dpot[y+1][x]+ 1./biny*dpot[y-1][x]+ 1./binx*dpot[y][(x+1+XMAX)%(XMAX)]+
					  1./binx*dpot[y][(x-1+XMAX)%(XMAX)])/(2./(biny)+2./(binx));	  
		    }
		}
	      else
		{
		  newpot.wpot[y][x]=wpot[y][x];		// weighting potential
		  newpot.dpot[y][x]=dpot[y][x];		// drift potential 
		}
	    }


	  // special treatment for top and bottom rows (potential above/below is replaced by cell itself)
	  if(tempfix[0][x])
	    {
	      newpot.wpot[0][x]=wpot[0][x];			// weighting potential
	      newpot.dpot[0][x]=dpot[0][x];
	      //	      newpot.dpot[0][x]=0.25*(newpot.dpot[1][x]+newpot.dpot[1][x]+newpot.dpot[1][(x+1+XMAX)%(XMAX)]
	      //		      +newpot.dpot[1][(x-1+XMAX)%(XMAX)]);
	      //newpot.wpot[0][x]=( 1./biny*wpot[1][x]+1./biny*wpot[1][x]+1./binx*wpot[1][(x+1+XMAX)%(XMAX)]+1./binx*wpot[1][(x-1+XMAX)%(XMAX)])/(2./(biny)+2./(binx));
	      
	    }
	  
	  if (tempfix[YMAX-1][x]==0)
	    {
	      
	      newpot.wpot[YMAX-1][x]=(1./binx*wpot[YMAX-1][(x-1+XMAX)%(XMAX)]+1./binx*wpot[YMAX-1][(x+1+XMAX)%(XMAX)]
				      +1./biny*wpot[YMAX-2][x])/(2./binx+1./biny);			 // weighting potential

	      //newpot.dpot[YMAX-1][x]= ( (1./binx*dpot[YMAX-1][(x-1+XMAX)%(XMAX)]+1./binx*dpot[YMAX-1][(x+1+XMAX)%(XMAX)]
	      //		 +1./biny*dpot[YMAX-2][x])/(2./binx+1./biny)+0.25*h2*(dopyx[DopBinY*(YMAX-1)][DopBinX*(x-1+XMAX)%(XMAX)]));   // drift potential
	      newpot.dpot[YMAX-1][x]=0.25*(dpot[YMAX-1][(x-1+XMAX)%(XMAX)]+dpot[YMAX-1][(x+1+XMAX)%(XMAX)]+dpot[YMAX-2][x]+dpot[YMAX-1][x]
			      		   +h2*(ECHARGE/(EPSILON*EPSILONR)*dopyx[YMAX-1][(x+1+XMAX)%(XMAX)]));   // drift potential 
	    }
	  
	  if(tempfix[YMAX-1][x])
	    {
	      newpot.wpot[YMAX-1][x]=wpot[YMAX-1][x];	// weighting potential	
	      newpot.dpot[YMAX-1][x]=dpot[YMAX-1][x];	// drift potential
	    } 
	}
      
      if(it%100==0) // check every 1 00 iterations wether break condition fulfilled
	{
	  sum=0.0;
	  for(int i=1; i<XMAX; i++) {		// calculate sum
	    for(int j=ymin; j<YMAX; j++) {
	      sum+=fabs(dpot[j][i]-newpot.dpot[j][i]);
	    }
	  }

	  //	  cout << " err " << sum/((double)(XMAX*YMAX)) << endl;
	  
	  if(sum/((double)(XMAX*YMAX))<err)	//check wether accuracy reached
	    {
	      for(int i=0;i<YMAX;i++) {	// overwrite potential with newpotential
		for(int j=0; j<XMAX;j++) {
		      dpot[i][j]=newpot.dpot[i][j];		  
		      wpot[i][j]=newpot.wpot[i][j];
		    }
		}
	      
	      
	      
	      break;	//break out of while loop if accuracy reached 
	    }
	}//end of if(it%100)
      
      for(int i=0;i<YMAX;i++) {	// overwrite potential with newpotential	
	for(int j=0; j<XMAX;j++)
	  {
	  dpot[i][j]=newpot.dpot[i][j];
	  wpot[i][j]=newpot.wpot[i][j];
	  }
      }
      it++;
      
      ///} // end of if(gui-<Getstopped()==0)
    }//end of while loop

}

//////////////////////////////////////////////////////////////////////
void Potentials::SetDopingProfile(void *wfgui, int rflag)	// 
{				// method to set the doping profile in dopyx
  //int r=1;

  double DJCoef = 0;  
  double poissondens = 0;
  double ehratio = 0.;
  double DJZero = 0.;

  double Acceptorpoissonf = 0;
  double gfluence = 0.02;
  double LnFluence = 5E15;
  

  // standard wafer: 0.02
  // Carbon rich 0.06
  
  double DopingRem = 1;
  double GainLayerThick = 0;
  double fixconst = 1; // needed to get the right depletion voltage (?)
  // double fluence_o = 1.8E15;
  WFGUI* gui= (WFGUI*) wfgui; 

  // double coefy = (1.*YMAX/finestY);
  // cout << " coefy = " << coefy << endl;
  //  float dist = coefy*gui->GetYMax(); // this is linear
  float dist = gui->GetYMax(); // this is linear
  //  float ymin =1;
  // float ymaxbd =YMAX;
  
  //  if (vdepl>vbias && !gui->GetDJOn() && ( ( bulk==PTYPE && strips == NTYPE )  || ( bulk==NTYPE && strips == PTYPE ))) ymin = dist-dist*sqrt(vbias)/sqrt(vdepl);
  // if (vdepl>vbias && !gui->GetDJOn() && ( ( bulk==PTYPE && strips == PTYPE )  || ( bulk==NTYPE && strips == NTYPE ))) ymaxbd = dist*sqrt(vbias)/sqrt(vdepl);
  
  // float ymin =1;
  //float ymaxbd =YMAX;
  
  GainLayerThick =   gui->Getygainlayerhigh() - gui->Getygainlayerlow();
  
  poissonf=((bulk==PTYPE) ? -1.0 : 1.0)*(2.0*vdepl)/(dist*dist*1e-12)*fixconst;	 // right side of Poisson equation => Q/epsilon =  eN/epsilon, this is in 1/m^3 
  poissondens=((bulk==PTYPE) ? -1.0 : 1.0)*(2.0*30)/(dist*dist*1e-12)*fixconst;	 //  Vdepl = 30 V ==> Density for 10 kOhm*cm ==>P-doping= 4.5*10^11 n/m^3

  vdepl_irr =  (poissonf)*dist*dist*1e-12/2.;
  if (rflag ==0)  cout  << "Bulk doping = "<< setprecision(2) <<  (poissonf)*(EPSILON*EPSILONR)/ECHARGE*1e-6<< "[N/cm^3]; V depletion = " <<  vdepl_irr  <<" [V]; " <<endl;
  
  if (  gui->GetAcceptorCreation() )
    {

      Acceptorpoissonf =  -gfluence*gui->GetFluence()*1e6*ECHARGE/(EPSILON*EPSILONR)*fixconst; //always p-type, NEGATIVE
      if (gui->GetLnAcceptorCreation() && gui->GetFluence()>LnFluence)
      // if (gui->GetFluence()>LnFluence)
	Acceptorpoissonf = - (0.507*log(gui->GetFluence())-17.3)*1E14*1e6*ECHARGE/(EPSILON*EPSILONR); //always p-type, NEGATIVE
	//Acceptorpoissonf = - gfluence*gui->GetFluence()*(1/(1+gui->GetFluence()/fluence_o))*1e6*ECHARGE/(EPSILON*EPSILONR); //always p-type, NEGATIVE
    }

  //DopingAdd = gui->GetDopinggainlayerValue()*ECHARGE/(EPSILON*EPSILONR);  
  //  cout << "poissonf = " << poissonf <<  << endl;
  //  cout  << "poissonf = " << poissonf << " Doping/cm^3 = " << poissonf*(EPSILON*EPSILONR)/ECHARGE*1e-6 << endl; // <<== this is correct
  // cout  << "DJ doping ref /cm^3. = " <<  poissondens*(EPSILON*EPSILONR)/ECHARGE*1e-6<< endl;
  // cout  << "Acceptor  doping Value = " <<  Acceptorpoissonf<< endl;
 



  
  if (gui->GetInitialDopRemoval())
    {             
      DopingRem = gui->InitialDopingRem(  poissonf*(EPSILON*EPSILONR)/ECHARGE*1e-6, gui->GetFluence(),2);
      //      cout << " After removal Doping/cm^3 = " << DopingRem*poissonf*(EPSILON*EPSILONR)/ECHARGE*1e-6 << endl; // <<== this is correct
      //     cout << " Inital Accept Removal Bulk  = " << DopingRem << " Initial Doping: " <<  poissonf*(EPSILON*EPSILONR)/ECHARGE*1e-6  << endl;
      if (DopingRem < 0.00001) DopingRem = 0;
      poissonf *= DopingRem;      
    }
  if ( gui->GetInitialDopRemoval() || gui->GetAcceptorCreation())
    {
      vdepl_irr =  (poissonf + Acceptorpoissonf)*dist*dist*1e-12/2.;
      if (rflag ==0)
	{
	  cout  << "After irradiation: Bulk doping = "<< setprecision(2) <<  (poissonf + Acceptorpoissonf)*(EPSILON*EPSILONR)/ECHARGE*1e-6<< "[N/cm^3]; Bulk V depletion = " <<  vdepl_irr  <<" [V]; \n";
	  if ( fabs(vdepl_irr) != 0 && fabs(vdepl_irr) > gui->GetVBias())
	    cout <<"Fraction of depleted bulk: " << setprecision(2) << pow(fabs(gui->GetVBias()/vdepl_irr),0.5) << endl;
	}
      
    }
   
  if (gui->GetDJOn())
    {
      DJCoef = gui->GetDJValue();
      ehratio = gui->GetDJehValue();
      DJZero = 0.5;
    }


  for (int x=0; x<XMAX; x++)
    { 
      for (int y=0; y<YMAX; y++)
	{
	  //dopyx[y][x] = ( poissonf + Acceptorpoissonf);
	  dopyx[y][x] = (poissonf + Acceptorpoissonf)*(EPSILON*EPSILONR)/ECHARGE;
	  
	  //	      cout << " y = " << y << "poissonf = " << poissonf*DopingRem << endl;
	  
	  if (gui->CallGetDJType() == 0 && gui->GetDJOn())
	    {
	      if (y< DJZero*YMAX) dopyx[y][x] += 10./2.1*DJCoef*(-poissondens*DJZero+poissondens*y/YMAX)*(EPSILON*EPSILONR)/ECHARGE;// h density
	      else	    dopyx[y][x] += ehratio*10./2.1*DJCoef*(-poissondens*DJZero+poissondens*y/YMAX)*(EPSILON*EPSILONR)/ECHARGE; // e density
	    }
	  else if (gui->CallGetDJType() == 1 && gui->GetDJOn())
	    {
	      if (y<(DJZero-0.1)*YMAX)  dopyx[y][x] += 1./2.1*DJCoef*poissondens*cbrt(1.*y- DJZero*YMAX)*(EPSILON*EPSILONR)/ECHARGE;
	      else if (y>(DJZero-0.1)*YMAX && y<YMAX*(DJZero+0.1)) dopyx[y][x] += 0.;
	      else   dopyx[y][x] += 1./2.1*ehratio*DJCoef*poissondens*cbrt(1.*y- (DJZero+0.1)*YMAX)*(EPSILON*EPSILONR)/ECHARGE;
	    }
	  //	      cout << " DJ y-x = " << y << " " << x << " " << dopyx[y][x]<< endl;
	  
	  
	}
    }
  
  // here you add the doping layer before the computation of the field, it's the correct thing to do, but it takes a lot of time to compute
  
  if (gui->GetGainShape() !=0 && VFROMDOP == 1) // VFROMDOP = calculate  V from Doping
    //     if (fabs(gui->GetDopinggainlayerValue()) > 0.001 ) 
    {
      //	  cout << "Gain layer V depletion: " << gui->GetGainLayerVdepletion() << " Gain region V depletion: " << gui->GetGainRegionVdepletion() << " micron" << endl;
      //	  cout << "Setdoping: Gain layer region: " << gui->Getygainlayerlow() << " - " << gui->Getygainlayerhigh() << " micron " << endl;
      // cout << "YMAX: " <<  GetYMAX() << " step size " << GetBinSizey()  << " Thickness = " << GetBinSizey()*YMAX << endl;
      double dd = 0;
      //	  double VdepGainLayer=  gui->GetGainLayerVdepletion();
      //fabs(gui->GetDopinggainlayerValue())*ECHARGE/(2*EPSILON*EPSILONR)*1e-12;
      
      
      int GainRecessBins= (int)  (gui->GetGainRecess()/GetBinSizex() );
      //   cout << "Gain Recess = " << gui->GetGainRecess() <<  " bins = "  << GainRecessBins<<  endl;
      
      /// nicolo cut
      
      double LocalDop = 0 ;
	  // add gainlayer doping
	  for(int y=0;y<YMAX;y++)
	    {
	      LocalDop = 0;
	      // if  (y*gui->GetYMax()/GetYMAX()  >= gui->Getygainlayerlow())
	      if  ((y)*GetBinSizey()  >= gui->Getygainlayerlow() )
		{
		  //		  if (y*gui->GetYMax()/GetYMAX()  < gui->Getygainlayerhigh() )
		  if ((y)*GetBinSizey()  < gui->Getygainlayerhigh() )
		    {
		      //		  dd = y*GetBinSizey()-gui->Getygainlayerlow()+1;
		      LocalDop = DopingRem*gui->GetDopinggainlayerValue();// *(gui->GetGainLength()/(gui->Getygainlayerhigh()-gui->Getygainlayerlow())); // local doping in N/m^3
		      dd += GetBinSizey();
		      //		      dd += gui->GetYMax()/GetYMAX();
		      
		      
		      //		      cout << " gain layer thickness: " << dd << " at position = " << y*GetBinSizey()<< " y index = " << y << " Doping = " << LocalDop <<  endl;		  
		      
		      
		      
		      // Gain recess
		      
		      for(int x=0; x<XMAX;x++)
			{
			  if (bulk == PTYPE && strips == NTYPE )
			    {
			      if (y !=YMAX-1)
				{
				  
				  if (dpot[YMAX-1][x]== vbias && dpot[YMAX-1][(int) (x - GainRecessBins)]  == vbias && dpot[YMAX-1][(int) (x+ GainRecessBins)] == vbias )
				    {
				      //if (x == 100)  cout << " before = x-y = " << x << " - " << y << " y pos = "  << y*GetBinSizey() << " doping= " << dopyx[y][x]<< endl;				 
				      dopyx[y][x] +=  LocalDop*pow(dd/gui->GetGainLength(),gui->GetGainExp());
				      //	      if (x == 100)  cout << " After x-y = " << x << " - " << y << " y pos = "  << y*GetBinSizey() << " doping= " << dopyx[y][x]<< endl;				  
				    }
				}
			    }
			  else if (bulk == NTYPE && strips == PTYPE) 
			    {
			      if (y !=YMAX-1)
				{
				  if (dpot[YMAX-1][x]== 0 && dpot[YMAX-1][ (int) (x - gui->GetGainRecess()/GetBinSizey())]  == 0 &&
				      dpot[YMAX-1][ (int) (x+ gui->GetGainRecess()/GetBinSizey())] == 0 )
				    {
				      dopyx[y][x] +=  LocalDop*pow(dd/gui->GetGainLength(),gui->GetGainExp());
				    }
				  
				}
			    }
			}
		      for(int x=0; x<XMAX ;x++)
			{	      
			  if (bulk == NTYPE && strips == NTYPE )
			    {
			      dopyx[y][x] +=  LocalDop*pow(dd/gui->GetGainLength(),gui->GetGainExp());
			    }
			  else if  (bulk == PTYPE && strips == PTYPE)
			    {
			      //	      cout << (GainLayerThick - dd) << " " << y << endl;
			      dopyx[y][x] +=  LocalDop*pow((GainLayerThick - dd)/gui->GetGainLength(),gui->GetGainExp());     
			    }
			}
		    } //gainlayerhigh
		} //>gainlayerlow
	    } // YMAX
	}  // doping layer
       
       /// nicolo's cut
       
}

//////////////////////////////////////////////////////
void Potentials::Smoothing()	// 
{				// method to smooth the potential

  TGraph *gVd = new TGraph();
  TF1 *Vd = new TF1("Vd","pol1",GetYMAX());
  // int npoints=5;
  // g->Set(npoints);
  // TF1 *lin = new TF1("lin","pol2");

  int step = 10;
  // float NewPot = 0;
  //  cout << XMAX << " XMAX " << YMAX << endl;
  for (int x=0; x< XMAX; x++)
	{ 
	  for (int y=0; y< YMAX-step; y+=2)
	    {
	      //     NewPot = 0;
	      for (int y1=0; y1< step; y1++)
		{
		  gVd->SetPoint(y1,y1,Getdpot(y+y1,x));
		}
	      gVd->Fit("Vd","QN","goff",0,GetYMAX());
	      SetdPotential(y+ (int) step/2.,x, Vd->Eval((int)step/2.));
	      if (x==100) cout << "E field: " << Vd->Derivative((int)step/2.) << endl;

	    }
	}
  
}

//////////////////////////////////////////////////////////////////////
Potentials::~Potentials() {	// destructor
  for (int j = 0; j < YMAX; j++) delete[] dpot[j] ;	    	
  delete[] dpot;
  
  for (int j = 0; j < YMAX; j++) delete[] wpot[j] ;
  delete[] wpot;

  for (int j = 0; j < YMAX; j++) delete[] fix[j] ;
  delete[] fix;
  
}
//////////////////////////////////////////////////////////////////////
void Potentials::Restriktor()    //method to restrict potentials to a coarser grid, with XMAX/2+1
{
  //  cout << __LINE__<< endl;
  
  int z=0,s=0;	// z: lines, s: columns
  ref=ref+1;	// increase ref 
  Potentials newpot(YMAX/2+1,XMAX/2+1); // temporary new object newpot on coarser grid
 
  for(int i=0;i<YMAX/2+1;i++)	// copy mutual points from 'old' potential to new potential
    {
      s=0;			
      for(int j=0; j<XMAX/2+1;j++)
	{
	  
	  newpot.dpot[i][j]=dpot[z][s];
	  newpot.wpot[i][j]=wpot[z][s];
	  s=s+2;

	}
      z=z+2;
    }
  
  XMAX=XMAX/2+1;		// set XMAX and YMAX to new values
  YMAX=YMAX/2+1;		
  
  wpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) wpot[i] = new double[XMAX];

  dpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) dpot[i] = new double[XMAX];
  
  for(int i=0;i<YMAX;i++)	 // overwriting dpot and wpot with dpot and wpot of new potential
    {		
      for(int j=0; j<XMAX;j++)
	{
	  dpot[i][j]=newpot.dpot[i][j];
	  wpot[i][j]=newpot.wpot[i][j];
	}
    }	
}
//////////////////////////////////////////////////////////////////////
//int** Potentials::FixRestriktor()  
void Potentials::FixRestriktor(int** nfix)
{
  int z=0,s=0;   // lines and columns counter 
//  int **nfix=0;
  
//  nfix = new int*[YMAX];	// allocate memory for restricted fix matrix
//  for (int i = 0; i<YMAX; i++) nfix[i] = new int[XMAX];
  
  for(int i=0;i<YMAX;i++)	// copy mutual points from old fix matrix to nfix
    {
      s=0;			
      for(int j=0; j<XMAX;j++)
	{
	  nfix[i][j]=fix[z][s];	
	  s=s+(int)pow(2.0,ref*1.);
	}
      z=z+(int)pow(2.0,ref*1.);
    }		
  return;
}
//////////////////////////////////////////////////////////////////////
void Potentials::Prolongation(bool ReadOutTopFlag) // method to prolongate potentials to finer grid with XMAX*2-1
{			
  ref=ref-1;	//decrease ref (going from coarser to finer grid)											
  int z=0,s=0;	// columns and lines counter for loop
  YMAX=YMAX*2-1;	// setting YMAX and XMAX
  XMAX=XMAX*2-1;
  Potentials newpot(YMAX,XMAX);	// temporary Potentials object
  
  int **tempfix=0;	// allocate memory for temporary fix matrix 
  tempfix = new int*[YMAX];
  for (int i = 0; i < YMAX; ++i) tempfix[i] = new int[XMAX];
  
//   tempfix=FixRestriktor();	// set fix matrix
  FixRestriktor(tempfix);

  //float binx = GetBinSizex();
  // float biny = GetBinSizey();

  // binx = 1.;
  // biny = 1.;
  
  for(int i=0;i<YMAX;i=i+2)    // copy mutual points from coarser grid to finer grid
    {		
      s=0;	
      for(int j=0; j<XMAX;j=j+2)
	{
	  newpot.dpot[i][j]=dpot[z][s];
	  newpot.wpot[i][j]=wpot[z][s];
	  s++;
	}
      z++;
    }

   
  for(int i=1;i<YMAX;i=i+2)	// calculate mean value for grid points without 4 neighbours
    {							
      for(int j=1;j<XMAX;j=j+2)
	{
	  newpot.dpot[i][j]=0.25*(newpot.dpot[i-1][j-1]+newpot.dpot[i-1][j+1]+newpot.dpot[i+1][j+1]+newpot.dpot[i+1][j-1]);
	  newpot.wpot[i][j]=0.25*(newpot.wpot[i-1][j-1]+newpot.wpot[i-1][j+1]+newpot.wpot[i+1][j+1]+newpot.wpot[i+1][j-1]);
	}
    }

  // set backplane to potential
  for (int j=0; j<XMAX; j++)
    {
      newpot.dpot[0][j]=(strips==PTYPE) ? vbias : 0.0; // bottom electrode (backplane): set to Vbias for p-type strips, otherwise 
      newpot.wpot[0][j]=(ReadOutTopFlag)? 0.0: 1.0;
    } 
  int l=0,f=0;  // auxiliary variable to calculate boundary points	
  for(int i=1;i<YMAX;i++)	// average over neighbours of grid points which are not set yet
    {						
      for(int j=(i+1)%2;j<XMAX;j=j+2)  //// depending on line (i), counter j starts at 0 or 1				
	{							
	  if(tempfix[i][j]==1) // fix jenes gitters auf das erweitert wird
	    {
	      newpot.dpot[i][j]=(strips==NTYPE) ? vbias : 0.0;	
	      newpot.wpot[i][j]=(ReadOutTopFlag)? 0.0: 0.0;										             
	    }
	  
	  if(tempfix[i][j]==2)
	    {
	      newpot.dpot[i][j]=(strips==NTYPE) ? vbias : 0.0;
	      newpot.wpot[i][j]=(ReadOutTopFlag)? 1.0: 0.0;										             

	    }
	  
	  if(tempfix[i][j]==0)
	    {
	      f=0;	//special treatment for bottom and top (potentials above/below is replaced by cell itself)	
	      l=0;							
	      
	      if(i==YMAX-1) f=1;
	      if(i==0) l=1;
	      if (XMAX != 1)
		{
		  newpot.dpot[i][j]=0.25*(newpot.dpot[i][(j-2+XMAX) % (XMAX-1)]+newpot.dpot[i+1-f][j]+newpot.dpot[i-1+l][j]+newpot.dpot[i][(j+XMAX)%(XMAX-1)]);
		  
		  //newpot.dpot[i][j]=(1./binx*newpot.dpot[i][(j-2+XMAX) % (XMAX-1)]+1./biny*newpot.dpot[i+1-f][j]
		  //		     +1./biny*newpot.dpot[i-1+l][j]+1./binx*newpot.dpot[i][(j+XMAX)%(XMAX-1)])/(2./binx+2./biny);
		  
		  newpot.wpot[i][j]=0.25*(newpot.wpot[i][(j-2+XMAX)% (XMAX-1)]+newpot.wpot[i+1-f][j]+newpot.wpot[i-1+l][j]+newpot.wpot[i][(j+XMAX)%(XMAX-1)]);
		  //newpot.wpot[i][j]=(1./binx*newpot.wpot[i][(j-2+XMAX)% (XMAX-1)]+1./biny*newpot.wpot[i+1-f][j]
		  //		     +1./biny*newpot.wpot[i-1+l][j]+1./binx*newpot.wpot[i][(j+XMAX)%(XMAX-1)])/(2./binx+2./biny);
		  
		}
	    }
	}
    }
  
  wpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) wpot[i] = new double[XMAX];
  
  dpot = new double*[YMAX];
  for (int i = 0; i < YMAX; ++i) dpot[i] = new double[XMAX];

  
  
  for(int i=0;i<YMAX;i++) {			// overwriting old dpot and wpot with new ones
    for(int j=0;j<XMAX;j++) {
      wpot[i][j]=newpot.wpot[i][j];
      dpot[i][j]=newpot.dpot[i][j];
  
    }
  }

 // delete[] tempfix;
  for (int i = 0; i < YMAX; ++i) delete[] tempfix[i];
  delete[] tempfix;
 
}
//////////////////////////////////////////////////////////////////////////
void Potentials::Multigrid(void *wfgui, bool ReadOutTopFlag)
{
  //  cout << __LINE__<< endl;
  
  WFGUI* gui= (WFGUI*) wfgui;


  
  for(int i=0; i<multig; i++) { // starting with calculation on coarsest grid, we need to restrict potentials
    Restriktor();
  }
  //  cout << __LINE__<< endl;
  double LocalDop = 0;
  double DopingRem = 1;
  char * gridlabel;
  double DopScale = 1;
  gridlabel = new char[60];
  //sprintf(gridlabel, "Calculating Potentials: grid number: %d/%d",1,multig+1);
  
  char * gridlabel2;
  gridlabel2 = new char[60];
  // sprintf(gridlabel2, "Calculating Potentials: grid number: %d/%d",1,multig+1);


  SetDopingProfile(gui,0);

  Iteration(gui);
  
  gui->GetCalculatingLabel2()->SetTitle(gridlabel2);
  gui->GetCalculatingLabel()->SetTitle(gridlabel);
  
  for(int i=0; i<multig; i++) { 
    sprintf(gridlabel, "Calculating Potentials: grid number: %d/%d",i+2,multig+1);
    sprintf(gridlabel2, "Calculating Potentials: grid number: %d/%d",i+2,multig+1);	
    Prolongation(ReadOutTopFlag);
    
    gui->GetCalculatingLabel2()->SetTitle(gridlabel2);
    gui->GetCalculatingLabel()->SetTitle(gridlabel);
    SetDopingProfile(gui,1);
    Iteration(gui);
    //    Smoothing();
  }
   //  double GainLayerThick =  gui->Getygainlayerhigh() - gui->Getygainlayerlow();
  
  //  Smoothing();
  
  gui->Getdhist()->GetXaxis()->SetLabelColor(1);	
  gui->Getwhist()->GetYaxis()->SetLabelColor(1);

 
  // Add gainlayer doping
  //if (fabs(gui->GetDopinggainlayerValue()) > 0.001 && 1 == 2)
  //if ( fabs(gui->GetDopinggainlayerValue()) > 0.001 && VFROMDOP !=1)
  // cout << "gui->GetGainShape() "  << gui->GetGainShape() << endl;
  if (gui->GetGainShape() !=0 && VFROMDOP != 1) // add gain  layer by hand
    {
      // cout << "Gain layer region: " << gui->Getygainlayerlow() << " - " << gui->Getygainlayerhigh() << " micron" << endl;
      double dd = 0;
      double ddout = 0;
      
      double DD = 0;
      double DDm1 = 0;
      double DDm2 = 0;
      

      
      double DDop = 0;
      double VdepGainLayer= gui->GetGainLayerVdepletion();
      // cout << " checking..  = " << VdepGainLayer << endl;
      // VdepGainLayer= gui->GetGainRegionVdepletion();

      //gui->GetGainLayerVdepletion();
          if (gui->GetInitialDopRemoval())
	{	  
	  DopingRem = gui->InitialDopingRem(gui->GetDopinggainlayerValue()*1e-6 , gui->GetFluence(), 1) ;
	  //  cout << " After removal  = " << VdepGainLayer << endl; // <<== this is correct
	  //	  cout << " Inital Acceptor Removal Gain Layer = " << DopingRem << " Initial Doping: " << gui->GetDopinggainlayerValue()*1e-6  << endl;
	 if (gui->GetGainShape() != 0)  cout << "Fraction of inital acceptor in the gain implant still present = " << DopingRem <<
	   " Final gain implant doping: " <<  DopingRem*gui->GetDopinggainlayerValue()*1e-6  <<  " [N/cm^3] " << endl;
	   gui->SetGainLayerVdepletion(DopingRem*gui->GetDopinggainlayerValue());
	}
      
      //      float nm = 3.2;
      // cout << "nm = " << nm << endl;
      if (gui->GetGainShape() !=0) cout << "Gain implant V depletion: " << setprecision(4) << gui->GetGainLayerVdepletion() << " V " << endl;
      if (gui->GetGainShape() != 0) cout << "Gain implant region: " << setprecision(4) << gui->Getygainlayerlow() << " - " << gui->Getygainlayerhigh() << " micron \n";
  

      int GainRecessBins= (int)  (gui->GetGainRecess()/GetBinSizex() );
      //   cout << "Gain Recess = " << gui->GetGainRecess() <<  " bins = "  << GainRecessBins<<  endl;

      /// nicolo cut
       //float FieldDepth = (gui->GetYMax()-ELECTRODE_DEPTH) - gui->Getygainlayerlow();
      //  float FieldDepth = gui->GetGainLength();
      // add gainlayer doping
      // Since the gain layer does not have exaclty the lenght requested, the doping of the gain layer is scaled up or down
      // V = N*d^2, to keep V constant N is multiplied by the scale factor (d_req/d_true)^2
      
      for(int y=0;y<YMAX;y++)	 
	  if  (y*GetBinSizey()  >= gui->Getygainlayerlow())
	      if (y*GetBinSizey()  <  (gui->Getygainlayerhigh() ))
		{
		  dd += GetBinSizey();
		  //		  cout << " gain layer thickness: " << dd << " at position = " << y*GetBinSizey()<< " y index = " << y << " Doping = " << LocalDop <<  endl;		  
		}    
      DopScale = pow((gui->Getygainlayerhigh() -   gui->Getygainlayerlow())/dd,2);
      
      cout << "Actual gain layer thickness: " << dd << " Requested = " << gui->Getygainlayerhigh() -   gui->Getygainlayerlow()<<  " Doping scaled by = " << DopScale << endl;
		   
      dd = 0;	      	

      if  ( (bulk == PTYPE && strips == NTYPE ) || (bulk == NTYPE && strips == PTYPE ))
	{
	  for(int y=0;y<YMAX;y++)
	    {
	      LocalDop = 0;	      
	      if  (y*GetBinSizey()  >= gui->Getygainlayerlow())
		{
		  if (y*GetBinSizey()  <  (gui->Getygainlayerhigh() ))
		    {
		      //		  dd = y*GetBinSizey()-gui->Getygainlayerlow()+1;
		      LocalDop = DopScale*DopingRem*gui->GetDopinggainlayerValue(); // local doping in N/m^3
		      dd += GetBinSizey();
		      //		  if ( (y+1)*GetBinSizey()  >  gui->Getygainlayerhigh() )  dd =   gui->Getygainlayerhigh() -   gui->Getygainlayerlow();	
		      
		    }
		  

		  // not sure why there is a factor of 2, but it works...
		  // DD = 2.*pow(dd/(gui->GetGainLength()+gui->GetGainDepth()),gui->GetGainExp()+2); //+2
		  DD = pow(dd/(gui->GetGainLength()),gui->GetGainExp()+2); //+2
		  DDop = pow(dd/gui->GetGainLength(),gui->GetGainExp());
		  if (DDm1 != DD)
		    {
		      DDm2 = DDm1;
		      DDm1 = DD;		  
		    }
		  DDm1 = DD;
		  if (y*GetBinSizey()  >=  gui->Getygainlayerhigh() )
		    {
		      if (y*GetBinSizey()  <=  (gui->GetYMax()-ELECTRODE_DEPTH) )
			{
			  ddout += DD-DDm2; // this is correct: it is the derivative after the last step to keep the E field constat
			  
			}
		    }
		  //pow((dd-GetBinSizey())/(gui->GetGainLength()+gui->GetGainDepth()),gui->GetGainExp()+2);
		  
		  
		}
	      
	      // insert the potential term and include the gain recess
	      
	      for(int x=0; x<XMAX;x++)
		{
		  if (bulk == PTYPE && strips == NTYPE )
		    {
		      if (y !=YMAX-1)
			{
			  
			  if (dpot[YMAX-1][x]== vbias && dpot[YMAX-1][(int) (x - GainRecessBins)]  == vbias && dpot[YMAX-1][(int) (x+ GainRecessBins)] == vbias )
			    {
			      //	      cout << " xbin = " << x << " x = "<< x*GetBinSizey()<< endl;
			      dpot[y][x] += VdepGainLayer*(DD+ddout);
			      //			      if (y*GetBinSizey()  <  (gui->Getygainlayerhigh() ))  dpot[y][x] += VdepGainLayer*DD;
			      // else if (y*GetBinSizey() > gui->Getygainlayerhigh() ) dpot[y][x] += VdepGainLayer*(DD+ddout);
			      if (y*GetBinSizey()  <  gui->Getygainlayerhigh() ) dopyx[y][x] +=  LocalDop*DDop;
			      
			    }
			}
		      else if (y ==YMAX-1)			
			if (dpot[YMAX-1][x]== vbias)   dpot[y][x] += VdepGainLayer*(DD+ddout);			    		      
		    }
		  else if (bulk == NTYPE && strips == PTYPE) 
		    {
		      if (y !=YMAX-1)
			{
			  if (dpot[YMAX-1][x]== 0 && dpot[YMAX-1][ (int) (x - GainRecessBins) ]  == 0 &&
			      dpot[YMAX-1][ (int) (x+GainRecessBins)] == 0 )
			    {
			      dpot[y][x] -= VdepGainLayer*(DD+ddout);
			      if (y*GetBinSizey()  <  gui->Getygainlayerhigh() ) dopyx[y][x] +=  LocalDop*DDop;
			    }
			  
			}
		      else if (y ==YMAX-1)			
			if (dpot[YMAX-1][x]== 0)   dpot[y][x] -= VdepGainLayer*(DD+ddout);			    		      
		      
		    }
		  		 
		} 
	      
	    }
	}// end ntype & ptype

      if  ( (bulk == PTYPE && strips == PTYPE ) || (bulk == NTYPE && strips == NTYPE ))
	{
	  for(int y= YMAX-1; y>-1;y--)
	    {
	      LocalDop = 0;
	  
	      if  (y*GetBinSizey()  < gui->Getygainlayerhigh())
		{
		  if (y*GetBinSizey()  >=  (gui->Getygainlayerlow() ))
		    {
		      //		  dd = y*GetBinSizey()-gui->Getygainlayerlow()+1;
		      LocalDop = DopScale*DopingRem*gui->GetDopinggainlayerValue(); // local doping in N/m^3
		      dd += GetBinSizey();
		      //		  if ( (y+1)*GetBinSizey()  >  gui->Getygainlayerhigh() )  dd =   gui->Getygainlayerhigh() -   gui->Getygainlayerlow();	
		      
		    }		  
		  
		  DD = pow(dd/(gui->GetGainLength()),gui->GetGainExp()+2); //+2
		  DDop = pow(dd/gui->GetGainLength(),gui->GetGainExp());
		  if (DDm1 != DD)
		    {
		      DDm2 = DDm1;
		      DDm1 = DD;		  
		    }
		  DDm1 = DD;
		  if (y*GetBinSizey()  <=  gui->Getygainlayerlow() )
		    {
		      if (y*GetBinSizey()  >  0)
			{
			  ddout += DD-DDm2; // this is correct: it is the derivative after the last step to keep the E field constat
			  
			}
		    }
		  
		}
	      
	      
	      // insert the potential term and include the gain recess
	      
	      for(int x=0; x<XMAX;x++)
		{
		  if (bulk == PTYPE && strips == PTYPE )
		    {
			  dpot[y][x] += VdepGainLayer*(DD+ddout);
			  if (y*GetBinSizey()  <  gui->Getygainlayerhigh() ) dopyx[y][x] +=  LocalDop*DDop;
			
		    }
		  else if (bulk == NTYPE && strips == NTYPE) 
		    {		      			  
		      dpot[y][x] -= VdepGainLayer*(DD+ddout);
		      if (y*GetBinSizey()  <  gui->Getygainlayerhigh() ) dopyx[y][x] +=  LocalDop*DDop;			    			  
		      		      
		    }
		}
	    }

	} 	// end   (bulk == PTYPE && strips == PTYPE ) || (bulk == NTYPE && strips == NTYPE 
    } // end adding the gain layer by hand

   
      
      double Vfix = gui->GetBiasEntry()/abs(dpot[YMAX-1][XMAX/2]-dpot[0][XMAX/2]);

      //      cout << "V@ Ymax = " << dpot[YMAX-1][XMAX/2] << " V @ 0 = " << dpot[0][XMAX/2] <<  endl;
      cout << "Vmax = " << abs(dpot[YMAX-1][XMAX/2]-dpot[0][XMAX/2]) << " Vbias = " << gui->GetBiasEntry() << " V Scaling factor = " << setprecision(4) << Vfix <<   endl;
      // cout << "Gain layer region V depl = " << dpot[YMAX-1][XMAX/2] - gui->GetVBias() << endl;
      
      //  Vfix = 1.;
      float Dshift = 0;
      // rescale the potential
      for(int y=0;y<YMAX;y++)
	for(int x=0;x<XMAX;x++)
	  {
	    if (x==0 && y ==0)
	      {
		if (strips == PTYPE && bulk == NTYPE )
		  {
		    Dshift = dpot[YMAX-1][XMAX/2];
		      
		      }
		if (strips == NTYPE && bulk == NTYPE  )
		  {
		    Dshift =dpot[0][XMAX/2] ; // set the backplane to 0
		  }
		//		cout << "DShift = " << Dshift << endl; 
	      }
	    dpot[y][x] -=Dshift;  ; // set the p strip to 0
	    dpot[y][x] *= Vfix;
	  }
	
  /// nicolo's cut
  
  //     if (gui->GetInitialDopRemoval() && (bulk==PTYPE) )
  if (gui->GetInitialDopRemoval())
     {
       double poissonf=((bulk==PTYPE) ? -1.0 : 1.0)*(2.0*vdepl)/( gui->GetYMax()* gui->GetYMax()*1e-12);    
       DopingRem = gui->InitialDopingRem(poissonf*(EPSILON*EPSILONR)/ECHARGE*1e-6, gui->GetFluence(),2);
       //       cout << " After removal Doping/cm^3 = " << DopingRem*poissonf*(EPSILON*EPSILONR)/ECHARGE*1e-6 << endl; // <<== this is correct
       if (DopingRem < 0.00001) DopingRem = 0;
       cout << "Fraction of inital doping in the bulk still present = " << DopingRem << " Final bulk doping: " <<  DopingRem*poissonf*(EPSILON*EPSILONR)/ECHARGE*1e-6  <<"  N/cm^3 " << endl;
     }
     
  delete [] gridlabel;
  delete [] gridlabel2;
  
}
//////////////////////////////////////////////////////////////////////////
unsigned char Potentials::GetDoping() {
  return strips;
}
//////////////////////////////////////////////////////////////////////////
void Potentials::DriftPal()
{
  static Int_t  colors[999];
  static Bool_t initialized = kFALSE;
  gStyle->SetNumberContours(999);
  
  Double_t r[]    = {0.0, 0.0, 0.0, 1.0, 1.0}; //0.54 1 1 0 0 0
  Double_t g[]    = {0.0, 1.0, 1.0, 1.0, 0.0}; //0.17 0 1 1 1 0
  Double_t b[]    = {1.0, 1.0, 0.0, 0.0, 0.0}; //0.89 0 0 0 1 1
  Double_t stop[] = {0.0, 0.25, 0.50,.75,1.0};
  
  if(!initialized)
    {
      Int_t FI = TColor::CreateGradientColorTable(5, stop, r, g, b, 999);
      for(int i=0;i<999;i++) colors[i] = FI+i;
      initialized = kTRUE;
      return;
    }
  gStyle->SetPalette(999,colors);
}
//////////////////////////////////////////////////////////////////////////
void Potentials::WeightPal()
{
  static Int_t  colors[999];
  static Bool_t initialized = kFALSE;
  gStyle->SetNumberContours(999);
  
  Double_t r[]    = {0.0, 0.0, 0.0, 1.0, 1.0}; //0.54 1 1 0 0 0
  Double_t g[]    = {0.0, 1.0, 1.0, 1.0, 0.0}; //0.17 0 1 1 1 0
  Double_t b[]    = {1.0, 1.0, 0.0, 0.0, 0.0}; //0.89 0 0 0 1 1
  Double_t stop[] = {0.0, 0.02, 0.20, .30, 1.0};
  
  if(!initialized)
    {
      Int_t FI = TColor::CreateGradientColorTable(5, stop, r, g, b, 999);
      for(int i=0;i<999;i++) colors[i] = FI+i;
      initialized = kTRUE;
      return;
    }
  gStyle->SetPalette(999,colors);
}
//////////////////////////////////////////////////////////////////////
void Potentials::SetAbove(bool above) {
  alpha_above=above;
}
//////////////////////////////////////////////////////////////////////////////////////

bool Potentials::GetAbove() {
  return alpha_above;
}
