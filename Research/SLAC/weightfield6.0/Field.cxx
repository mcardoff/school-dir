#include "Field.h"
#include "Carriers.h"
///////////////////////////////////////////////////////////////////
Field::Field() {
	x=0;
	y=0;
	abs=0;
}
//Field::Field(double valx, double valy) {
//	x=valx;
//	y=valy;
//}
void Field::SetField(double valx, double valy) {
	x=valx;
	y=valy;
}
void Field::AddField(double valx, double valy) {
	x +=valx;
	y +=valy;
}

double Field::GetFieldx() {
	return x;
}
double Field::GetFieldy() {
	return y;
}
///////////////////////////////////////////////////////////////////
void CalculateFields(Potentials &pot, Field **wf, Field **df)
{
  //  TGraph *g = new TGraph();
  TGraph *gVd = new TGraph();
  //int nnpoints=0;
  //  TF1 *lin = new TF1();
  //  int limit = 4;
  int step = 1/pot.GetBinSizey();
  //  int step2 = 1/pot.GetBinSizey();
  //  std::cout << " step = " << step << std::endl;
  //  TF1 *lin = new TF1("lin","[0]+[1]*x",0,pot.GetBinSizey()*1e-6*(limit*2+1));
  TF1 *Vd = new TF1("Vd","pol2",0,pot.GetYMAX());

  for (int i=1; i<pot.GetXMAX(); i++)
    {
      for (int j=1; j<pot.GetYMAX()-1; j++)
	{
	  //	  gVd->SetPoint(j,pot.GetBinSizey()*1e-6*j,pot.Getdpot(j,i));
	  
	  //	  if (i== pot.GetXMAX()/2) std::cout << j*pot.GetBinSizey() << " " << ( pot.Getdpot(j,i) - pot.Getdpot(j-1,i) )/(pot.GetBinSizey()*1e-6) << std::endl;
	  wf[j][i].SetField(-( pot.Getwpot(j,i) - pot.Getwpot(j,(i-1+pot.GetXMAX())%(pot.GetXMAX())) )/(pot.GetBinSizex()*1e-6),
			    -( pot.Getwpot(j,i) - pot.Getwpot(j-1,i) )/(pot.GetBinSizey()*1e-6));
	  
	  df[j][i].SetField(-( pot.Getdpot(j,i) - pot.Getdpot(j,(i-1+pot.GetXMAX())%(pot.GetXMAX())) )/(pot.GetBinSizex()*1e-6),
			    -( pot.Getdpot(j,i) - pot.Getdpot(j-1,i) ) /(pot.GetBinSizey()*1e-6));

	    if (df[j][i].GetFieldy()>0 && df[j-1][i].GetFieldy()<0)
	    {
	      if (i ==100)
		{
		  //		  std::cout << "  Ey field change sign = " <<  df[j][i].GetFieldy() << " y = " << j*pot.GetBinSizey()<< std::endl;
		  //             std::cout << " Setting Ey field to = " <<  df[j-1][i].GetFieldy() << std::endl;
		}
	      df[j][i].SetField(df[j][i].GetFieldx(),df[j-1][i].GetFieldy());
	    }
	}
      if (VFROMDOP==1)
	{
	  for (int y=step/2; y< pot.GetYMAX()-3./2.*step; y+=step)
	    {
	      //	      std::cout << y << std::endl;
	      //     NewPot = 0;
	      for (int y1=0; y1< 2*step; y1++)
		{
		  gVd->SetPoint(y1,y1,pot.Getdpot(y+y1-step/2,i));
		  //	  std::cout << y1 << " y1 y+y1-step/2 = " << y+y1-step/2<< " Max " << pot.GetYMAX() <<   std::endl;
		}
	      gVd->Fit("Vd","QN","goff",0,pot.GetYMAX());
	      for ( int y2 = 0; y2<step; y2++)
		{
		  //	  std::cout << y2 << "y2 step = " << step<< std::endl;
		  pot.SetdPotential(y+ y2,i, Vd->Eval(step/2+y2));
		  if ( i== pot.GetXMAX()/2 && Vd->Eval(y2) < 0) std::cout << " Warning: negative potential at y =  " << (y+y2)*pot.GetBinSizey() << " micron " << std::endl;
		  //  if (i==100) std::cout << " y = " <<  (int) (y+ step/2.) << " E field: " << -Vd->Derivative((int)step/2.)*1e7 <<  "  old efield " <<  df[ (int) (y+ step/2.)][i].GetFieldy() << std::endl;
		  df[(int) (y +y2)][i].SetField(-( pot.Getdpot(y,i) - pot.Getdpot(y,(i-1+pot.GetXMAX())%(pot.GetXMAX())) )/(pot.GetBinSizex()*1e-6),
						     			 -( Vd->Derivative(step/2+y2)*1e7 ));
		}
	    }
	}
	      

      // Set the field at y = 0 equal to y= YMAX-2
      for (int y=0; y<=step/2; y++)
            df[y][i].SetField(df[(int) (step/2+1)][i].GetFieldx(),df[(int) (step/2+1)][i].GetFieldy());
      
      //       Set the field at y = YMAX-1 equal to y= YMAX-2

      for (int y=pot.GetYMAX()-step/2; y<pot.GetYMAX(); y++)
	df[y][i].SetField(df[pot.GetYMAX()-step/2-1][i].GetFieldx(),df[pot.GetYMAX()-step/2-1][i].GetFieldy());
      //     

      wf[0][i].SetField(wf[1][i].GetFieldx(),wf[1][i].GetFieldy());
      wf[pot.GetYMAX()-1][i].SetField(wf[pot.GetYMAX()-2][i].GetFieldx(),wf[pot.GetYMAX()-2][i].GetFieldy());

      
      //gVd->Fit("Vd","QN","goff",0,pot.GetBinSizey()*1e-6*pot.GetYMAX());
      // std::cout << " Vd0= " << Vd->GetParameter(0) << " " << Vd->Eval(0.00005) << std::endl;  
    }
}


///////////////////////////////////////////////////////////////////
void CalculateAbsFields(Potentials &pot, Field** wf,Field** df) {
	for (int i=0; i<pot.GetXMAX(); i++)
	{
		for (int j=0; j<pot.GetYMAX(); j++)
		{
			df[j][i].Setabs(sqrt((df[j][i].GetFieldx())*(df[j][i].GetFieldx())+(df[j][i].GetFieldy())*(df[j][i].GetFieldy())));
			wf[j][i].Setabs(sqrt((wf[j][i].GetFieldx())*(wf[j][i].GetFieldx())+(wf[j][i].GetFieldy())*(wf[j][i].GetFieldy())));
			//	if (i== pot.GetXMAX()/2) std::cout << "posiz=" << j*pot.GetBinSizey() << " " << df[j][i].Getabs() << std::endl;
		}
	}
}
///////////////////////////////////////////////////////////////////
/*
Field** RotateField(Potentials &pot, Field** df, double angle) {
	Field **newdf;									// B hinaus, winkel positiv ... B hinein, winkel negativ
	newdf = new Field*[(pot.GetYMAX())];
	for (int i = 0; i < (pot.GetYMAX()); i++) newdf[i] = new Field[(pot.GetXMAX())];

	for(int i=0; i<pot.GetYMAX(); i++) {
		for(int j=0; j<pot.GetXMAX(); j++) {
			newdf[i][j].SetField(cos(angle)*(cos(angle)*df[i][j].GetFieldx()-sin(angle)*df[i][j].GetFieldy()),cos(angle)*(sin(angle)*df[i][j].GetFieldx()+cos(angle)*df[i][j].GetFieldy()));
		}
	}
	return newdf;
}
*/

void RotateField(Potentials &pot, Field** rotatedf, Field** df, double angle) {
	

	for(int i=0; i<pot.GetYMAX(); i++) {
		for(int j=0; j<pot.GetXMAX(); j++) {
			rotatedf[i][j].SetField(cos(angle)*(cos(angle)*df[i][j].GetFieldx()-sin(angle)*df[i][j].GetFieldy()),cos(angle)*(sin(angle)*df[i][j].GetFieldx()+cos(angle)*df[i][j].GetFieldy()));
		}
	}
	for(int i=0; i<pot.GetYMAX(); i++) {
		for(int j=0; j<pot.GetXMAX(); j++) {
			rotatedf[i][j].SetField(cos(angle)*(cos(angle)*df[i][j].GetFieldx()-sin(angle)*df[i][j].GetFieldy()),cos(angle)*(sin(angle)*df[i][j].GetFieldx()+cos(angle)*df[i][j].GetFieldy()));
		}
	}
	return;
}
///////////////////////////////////////////////////////////////////

double Field::Getabs() {
	return abs;
}

void Field::Setabs(double val) {
	abs=val;
}

Field::~Field() {

}
