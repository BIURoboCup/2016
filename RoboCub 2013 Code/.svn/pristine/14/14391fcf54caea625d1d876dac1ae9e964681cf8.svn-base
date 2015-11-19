#ifndef ANGLE_H_
#define ANGLE_H_

#include "IncludesFile.h"
using namespace std;
#include <iostream>

class Angle
{
	public:
		Angle();
		Angle(const Angle& ang);
		Angle(const double& ang);
		Angle operator+(const Angle& ang) const ;
		Angle& operator+=(const Angle& ang);
		friend Angle operator+(const double& ang1, const Angle& ang2);
		Angle operator-(const Angle& ang)const ;
		Angle& operator-=(const Angle& ang) ;
		Angle& operator/=(const Angle& ang);
		friend Angle operator-(const double& ang1, const Angle& ang2);
		friend ostream& operator << (ostream& out,const Angle& ang);

		inline operator double (void) {return m_angle;}

	private:
		void Normalize();
		Angle Normalize(double ang) const;
		double m_angle;
};


#endif
