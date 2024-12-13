#include "Potentials.h"

#ifndef Field_H
#define Field_H


class Field {

	private:
		double x,y;
		double abs;

	public:
		Field();
		virtual ~Field();
		double GetFieldx();
		double GetFieldy();
		double Getabs();
		void Setabs(double);
		void SetField(double,double);
		void AddField(double,double);

		ClassDef(Field,0); 
};


#endif

void CalculateFields(Potentials&,Field**,Field**);
void CalculateAbsFields(Potentials&,Field**,Field**);
//Field** RotateField(Potentials &,Field**,double);
void RotateField(Potentials &,Field**,Field**,double);