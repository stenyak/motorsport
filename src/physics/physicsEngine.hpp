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

/******************************************************************************
*
* Includes
*
******************************************************************************/

#include "logEngine.hpp"
#include "SDL.h"


/******************************************************************************
*
* Classes
*
******************************************************************************/

class PhysicsEngine
{
    LogEngine log;    //a log object for logging independently from other engines and main
    PhysicsData *physicsData;
    WorldData *worldData;
    SystemData *systemData;
    public:
        int start (WorldData *wrlData, SystemData *sysData);        //starts the graphics engine
        int step (void);        //makes the graphics engine draw one frame
        int stop (void);        //stops the graphics engine
};
