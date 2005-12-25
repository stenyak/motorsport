/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "location.hpp"
#include "xmlParser.hpp"
#include "logEngine.hpp"

Location::Location (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    LogEngine * log = new LogEngine (LOG_DEVELOPER, "VEP");
    if (n->hasAttributes ())
    {
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();
        index = "0";
        position.x = position.y = position.z = 0;
        rotation.x = rotation.y = rotation.z = 0;
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName ());
            if (attribute == "index")
            {
                assignXmlString (index, attNode->getValue ());
                log->__format (LOG_CCREATOR, "Found the position index: %s", index.c_str ());
            }
            if (attribute == "position")
            {
                assignXmlString (attribute, attNode->getValue ());
                log->__format (LOG_CCREATOR, "Found the position: %s", attribute.c_str ());
                position = Vector3d(attribute);
            }
            if (attribute == "rotation")
            {
                assignXmlString (attribute, attNode->getValue ());
                log->__format (LOG_CCREATOR, "Found the rotation: %s", attribute.c_str ());
                rotation = Quaternion(attribute);
            }
        }
//        rotation.degreesToRadians ();
    }
    delete log;
}

Location::~Location ()
{
    // empty
}

Vector3d Location::getPosition ()
{
    return position;
}

Quaternion Location::getRotation ()
{
    return rotation;
}

std::string Location::getIndex ()
{
    return index;
}