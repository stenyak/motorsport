/*****************************************************************************\
|* Copyright (C) 2003, 2007 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#include "pedal.hpp"
#include "engine.hpp"
#include "world.hpp"
#include "system.hpp"
#include <SDL/SDL_keysym.h>

pPedal Pedal::create(XmlTag * tag)
{
    pPedal tmp(new Pedal(tag));
    return tmp;
}

Pedal::Pedal (XmlTag * tag)
    :WorldObject("pedal")
{
    constructFromTag(tag);
}
void Pedal::readCustomDataTag(XmlTag * tag)
{
    log->__format (LOG_CCREATOR, "Starting to parse a pedal node");
    angleRange = 180;
    currentAngle = 0.0;
    userDriver = false;
    angleRange = stod (tag->getAttribute("angleRange"));
}

Pedal::~Pedal ()
{
}


void Pedal::setUserDriver ()
{
    log->__format(LOG_ENDUSER, "Pedal %s will now be driven by a human.", getName().c_str());
    userDriver = true;
}

void Pedal::stepPhysics ()
{
    currentAngle = 0;
    if ( userDriver )
    {
        if (getName() == "(pedal)gas")
        {
            currentAngle = 1 - System::get()->axisMap[getIDJoyAxis(0,2)]->getValue();
            if (currentAngle == 0) {
                currentAngle = System::get()->axisMap[getIDKeyboardKey(SDLK_KP9)]->getValue() * 3 / 3;
                if (currentAngle == 0) {
                    currentAngle = System::get()->axisMap[getIDKeyboardKey(SDLK_KP6)]->getValue() * 2 / 3;
                    if (currentAngle == 0) {
                        currentAngle = System::get()->axisMap[getIDKeyboardKey(SDLK_KP3)]->getValue() * 1 / 3;
                        if (currentAngle == 0) {
                            currentAngle = System::get()->axisMap[getIDKeyboardKey(SDLK_UP)]->getValue() * 3 / 3;
            }   }   }   }
        } else if (getName() == "(pedal)brake")
        {
            currentAngle = 1 - System::get()->axisMap[getIDJoyAxis(0,1)]->getValue();
            if (currentAngle == 0) {
                currentAngle = System::get()->axisMap[getIDKeyboardKey(SDLK_KP8)]->getValue() * 3 / 3;
                if (currentAngle == 0) {
                    currentAngle = System::get()->axisMap[getIDKeyboardKey(SDLK_KP5)]->getValue() * 2 / 3;
                    if (currentAngle == 0) {
                        currentAngle = System::get()->axisMap[getIDKeyboardKey(SDLK_KP2)]->getValue() * 1 / 3;
                        if (currentAngle == 0) {
                            currentAngle = System::get()->axisMap[getIDKeyboardKey(SDLK_DOWN)]->getValue() * 3 / 3;
            }   }   }   }
        } else if (getName() == "(pedal)clutch")
        {
            currentAngle = System::get()->axisMap[getIDKeyboardKey(SDLK_KP7)]->getValue() * 3 / 3;
            if (currentAngle == 0) {
                currentAngle = System::get()->axisMap[getIDKeyboardKey(SDLK_KP4)]->getValue() * 2 / 3;
                if (currentAngle == 0) {
                    currentAngle = System::get()->axisMap[getIDKeyboardKey(SDLK_KP1)]->getValue() * 1 / 3;
                    if (currentAngle == 0) {
                        currentAngle = System::get()->axisMap[getIDKeyboardKey(SDLK_q)]->getValue() * 3 / 3;
            }   }   }
        }
    }
    currentAngle *= angleRange;
}

double Pedal::getNormalizedAngle()
{
    return (currentAngle / angleRange);
}
