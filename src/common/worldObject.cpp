/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "worldObject.hpp"
WorldObject::WorldObject (std::string identifier)
{
    construct(identifier);
}
WorldObject::WorldObject ()
{
    construct("none");
}
void WorldObject::construct(std::string identifier)
{
    this->identifier = identifier;
    log = new LogEngine (LOG_DEVELOPER, identifier);
}
WorldObject::~WorldObject ()
{
    delete log;
    log = NULL;
}