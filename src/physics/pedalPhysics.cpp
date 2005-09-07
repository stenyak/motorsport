/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "engine.hpp"
#include "world.hpp"
#include "system.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"
#include "SDL/SDL_keysym.h"
#include "pedal.hpp"


void Pedal::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    angleRange = 180;
    currentAngle = 0.0;
    id = "none";
    if (n->hasAttributes ())
    {
        // get all the attributes of the node
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();

        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "angleRange")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the pedal angle range: %s", attribute.c_str() );
                angleRange = stod (attribute);
            }
            if (attribute == "id")
            {
                assignXmlString (id, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the pedal identifier: %s", id.c_str() );
            }
        }
    }
}

void Pedal::stopPhysics ()
{
}

void Pedal::stepPhysics ()
{
    currentAngle = 0;
    if ( userDriver )
    {
        if (id == "gasPedal")
        {
            currentAngle = 1 - SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,2)]->getValue();
            if (currentAngle == 0) {
                currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP9)]->getValue() * 3 / 3;
                if (currentAngle == 0) {
                    currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP6)]->getValue() * 2 / 3;
                    if (currentAngle == 0) {
                        currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP3)]->getValue() * 1 / 3;
                        if (currentAngle == 0) {
                            currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_UP)]->getValue() * 3 / 3;
            }   }   }   }
        } else if (id == "brakePedal")
        {
            currentAngle = 1 - SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,1)]->getValue();
            if (currentAngle == 0) {
                currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP8)]->getValue() * 3 / 3;
                if (currentAngle == 0) {
                    currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP5)]->getValue() * 2 / 3;
                    if (currentAngle == 0) {
                        currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP2)]->getValue() * 1 / 3;
                        if (currentAngle == 0) {
                            currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_DOWN)]->getValue() * 3 / 3;
            }   }   }   }
        } else if (id == "clutchPedal")
        {
            currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP7)]->getValue() * 3 / 3;
            if (currentAngle == 0) {
                currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP4)]->getValue() * 2 / 3;
                if (currentAngle == 0) {
                    currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP1)]->getValue() * 1 / 3;
                    if (currentAngle == 0) {
                        currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_q)]->getValue() * 3 / 3;
            }   }   }
        }
    }
    currentAngle *= angleRange;
}

double Pedal::getNormalizedAngle()
{
    log->format (LOG_CCREATOR, "Reading pedal \"%s\" angle: %f", id.c_str(), currentAngle );
    return (currentAngle / angleRange);
}
