/*****************************************************************************
* Copyright (C) 2004 Daniel Toplak (homer.simpson123@gmx.de)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*   
* See the GNU General Public License for more details.
*   
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*   
******************************************************************************/

#ifndef VECTOR3D_HPP
#   define VECTOR3D_HPP
#   include <vector>
#   include <cmath>

class Vector3d
{
  public:
    double x;
    double y;
    double z;
    Vector3d ();
    Vector3d (double x, double y, double z);
    Vector3d(double qw, double qx, double qy, double qz); //quat to euler radians
//    Vector3d (const Vector3d & cpy); default copy constructor used.
    const Vector3d & operator= (const Vector3d & cpy);
    bool operator== (const Vector3d & cpy) const;
    bool operator!= (const Vector3d & cpy) const;
    Vector3d operator+ (const Vector3d & k) const;
    Vector3d operator- (const Vector3d & k) const;
    const Vector3d & operator+= (const Vector3d & k);
    const Vector3d & operator-= (const Vector3d & k);
    double distance () const;
    double distance (const Vector3d & k) const;
    Vector3d & degreesToRadians();
    Vector3d & scalarAdd(const double value);
    Vector3d & scalarMultiply(const double value);
    Vector3d & scalarDivide(const double value);
};

#endif
