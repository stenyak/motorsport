/******************************************************************************
*
* Copyright (C) 2004 Bruno Gonz�lez Campo (stenyak@users.sourceforge.net)
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

#include "vehicle.hpp"
#include "data/xercesc_fwd.hpp"
#include "body.hpp"

void Vehicle::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
}

void Vehicle::setPosition (double posX, double posY, double posZ)
{
    body->setPosition (posX, posY, posZ);
}

void Vehicle::setRotation (double rotX, double rotY, double rotZ)
{
    body->setRotation (rotX, rotY, rotZ);
}

void Vehicle::stopPhysics ()
{
    body->stopPhysics();
}

void Vehicle::stepPhysics ()
{
    body->stepPhysics();
}