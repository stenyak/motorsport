/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "wheel.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "data/xmlParser.hpp"
#include "log/logEngine.hpp"
#include "system.hpp"

void Wheel::startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    std::string mesh = "None";
    Ogre::SceneDetailLevel renderMode = Ogre::SDL_SOLID;
    if (n->hasAttributes ())
    {
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "mesh")
            {
                assignXmlString (mesh, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the wheel graphics mesh filename: %s", mesh.c_str());
            }
            if (attribute == "renderMode")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the wheel rendering mode: %s", attribute.c_str());

                if(attribute == "points")
                    renderMode=Ogre::SDL_POINTS;
                if(attribute == "wireframe")
                    renderMode=Ogre::SDL_WIREFRAME;
                if(attribute == "solid")
                    renderMode=Ogre::SDL_SOLID;
            }
        }
    }
    char name[256];
    sprintf (name, "%i", instancesCount);
    std::string file = SystemData::getSystemDataPointer()->dataDir;
    file.append("/vehicles/");
    file.append(SystemData::getSystemDataPointer()->tmpPath);
    file.append("/");
    file.append(mesh);
    wheelEntity = SystemData::getSystemDataPointer ()->ogreSceneManager->createEntity (name, mesh.c_str());
    wheelEntity->setRenderDetail(renderMode);

    log->format (LOG_CCREATOR, "Wheel mesh has %i submeshes", wheelEntity->getNumSubEntities());
    for(unsigned int i = 0; i < wheelEntity->getNumSubEntities(); i++)
    {
        log->format (LOG_CCREATOR, "Wheel submesh %i material: %s", i, wheelEntity->getSubEntity(i)->getMaterialName().c_str() );
    }

    wheelNode = static_cast < Ogre::SceneNode * >(SystemData::getSystemDataPointer ()->ogreSceneManager->getRootSceneNode ()->createChild ());
    wheelNode->attachObject (wheelEntity);
}
void Wheel::stepGraphics ()
{
    updateOgrePosition ();
    updateOgreOrientation ();
}

void Wheel::stopGraphics ()
{
    // empty
}
