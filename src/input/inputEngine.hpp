
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

#include "logEngine.hpp"
#include "SDL.h"
#include "system.hpp"
#include "world.hpp"

class InputEngine
{
    LogEngine log;              //a log object for logging independently from other engines and main
    InputData *inputData;
    SystemData *systemData;
    WorldData *worldData;
  public:
    int start ( WorldData * wrlData, SystemData * sysData );    //starts the input engine
    int step ( void );          //processes user input queue
    void processInputKeyDown ( SDLKey keySymbol );
    void processInputKeyUp ( SDLKey keySymbol );
    int stop ( void );          //stops the input engine
};
