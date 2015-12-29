#include "Angle.h"

Angle::Angle():m_angle(0)
{
}

Angle::Angle(const Angle& ang)
{
	m_angle = ang.m_angle;
}

Angle::Angle(const double& ang)
{
	m_angle = ang;
	Normalize();
}

void Angle::Normalize()
{
	while(m_angle >180)
		m_angle -= 360;
	while(m_angle <= -180)
		m_angle +=360;
}

Angle Angle::Normalize(double ang) const
{
	double val = ang;
	while(val >180)
		val -= 360;
	while(val <= -180)
		val +=360;
	return Angle(val);
}


 Angle Angle::operator+(const Angle& ang) const
 {
	 return Normalize(m_angle+ang.m_angle);
 }

 Angle& Angle::operator+=(const Angle& ang)
{
	m_angle += ang.m_angle;
	Normalize();
	return *this;
}

Angle operator+(const double& ang1, const Angle& ang2)
{
	return ang2+ang1;
}
Angle Angle::operator-(const Angle& ang)const
{
	 return Normalize(m_angle-ang.m_angle);
}
Angle& Angle::operator-=(const Angle& ang)
{
	m_angle -= ang.m_angle;
	Normalize();
	return *this;
}
Angle& Angle::operator/=(const Angle& ang)
{
	m_angle /= ang.m_angle;
	Normalize();
	return *this;
}
Angle operator-(const double& ang1, const Angle& ang2)
{
	return ang2-ang1;
}


ostream& operator << (ostream& out,const Angle& ang){
	return	out<<ang.m_angle;
}
