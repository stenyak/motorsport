
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

#include "system.hpp"
#include "world.hpp"
#include "graphicsEngine.hpp"
#include <stdlib.h>

GraphicsEngine::GraphicsEngine ( )
{
    //first of all start the logger (automatically logs the start of itself)
    log = new LogEngine ( LOG_VERBOSE, "GFX" );

    //get the direction of the graphics data
    log->put ( LOG_INFO, "Setting up data pointers..." );
    worldData = WorldData::getWorldDataPointer();
    systemData = SystemData::getSystemDataPointer();

    log->put ( LOG_INFO, "Setting screen properties..." );
    width = 1027;
    height = 768;
    bpp = 0;
    log->format ( LOG_INFO, "Graphics data initialized for %ix%i@%ibpp", width, height, bpp );

    ogreRoot = new Ogre::Root (  );
    setupResources (  );
    // Initialise the system
    if ( !manualInitialize (  ) )
    {
//        return false;
    }
    // Here we choose to let the system create a default rendering window
    // by passing 'true'
    systemData->ogreWindow = ogreRoot->initialise ( true );
    ogreSceneManager = ogreRoot->getSceneManager ( Ogre::ST_GENERIC );
    // Create the camera
    int numberOfCameras = Camera::cameraList.size();
    for (int i=0; i<numberOfCameras; i++)
    {
        char name[20];
        sprintf ( name, "Camera%i", i );
        Camera::cameraList[i]->ogreCamera = ogreSceneManager->createCamera ( name );
        Camera::cameraList[i]->ogreCamera->setFixedYawAxis(true,Ogre::Vector3(0,0,1));
        Camera::cameraList[i]->ogreCamera->setPosition ( Ogre::Vector3 ( -2000, -2000, 500 ) );
        Camera::cameraList[i]->ogreCamera->lookAt ( Ogre::Vector3 ( 0, 0, 0 ) );
        Camera::cameraList[i]->ogreCamera->setNearClipDistance ( 5 );
    }
    
    // Create one viewport, entire window
    Ogre::Viewport *vp = systemData->ogreWindow->addViewport ( Camera::cameraList[0]->ogreCamera );
    vp->setBackgroundColour ( Ogre::ColourValue ( 0, 0, 0 ) );
    
    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton (  ).setDefaultNumMipMaps ( 5 );

    // Create the skybox
    Ogre::Quaternion rotationToZAxis;
    rotationToZAxis.FromRotationMatrix(Ogre::Matrix3(1,0,0,0,0,-1,0,1,0));
    ogreSceneManager->setSkyBox ( true, "skybox", 5000, true, rotationToZAxis );
                                                           
    //Create cubes
    int numberOfCubes = Cube::cubeList.size();
    for ( int i = 0; i < numberOfCubes; i++ )
    {
        char name[20];
        sprintf ( name, "Cube%i", i );
        Cube::cubeList[i]->cubeEntity = ogreSceneManager->createEntity ( name, "../data/cube.mesh" );
        Cube::cubeList[i]->cubeEntity->setMaterialName("cube");
        Cube::cubeList[i]->cubeNode = static_cast < Ogre::SceneNode * >( ogreSceneManager->getRootSceneNode ( )->createChild ( ) );
        Cube::cubeList[i]->cubeNode->attachObject (Cube::cubeList[i]->cubeEntity);
    }

    //Set some graphics settings
    Ogre::MaterialManager::getSingleton (  ).setDefaultAnisotropy (1 );
    Ogre::MaterialManager::getSingleton (  ).setDefaultTextureFiltering ( Ogre::TFO_BILINEAR );
}

bool GraphicsEngine::manualInitialize (  )
{
    Ogre::RenderSystem *renderSystem;
    bool ok = false;
    Ogre::RenderSystemList *renderers = Ogre::Root::getSingleton (  ).getAvailableRenderers (  );
    // See if the list is empty (no renderers available)
    if ( renderers->empty (  ) )
        return false;
    for ( Ogre::RenderSystemList::iterator it = renderers->begin (  );
          it != renderers->end (  ); it++ )
    {
        renderSystem = ( *it );
        if ( strstr ( &( *renderSystem->getName (  ) ), "OpenGL" ) )
        {
            ok = true;
            break;
        }
    }
    if ( !ok )
    {
        // We still don't have a renderer; pick up the first one from the list
        renderSystem = ( *renderers->begin (  ) );
    }

    Ogre::Root::getSingleton (  ).setRenderSystem ( renderSystem );
    char resolution[32];
    sprintf ( resolution, "%i x %i", width, height );

    // Manually set configuration options. These are optional.
    renderSystem->setConfigOption ( "Video Mode", resolution );

    return true;
}

void GraphicsEngine::setupResources ( void )
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load ( "resources.cfg" );

    // Go through all settings in the file
    Ogre::ConfigFile::SettingsIterator i = cf.getSettingsIterator (  );
    Ogre::String typeName, archName;
    while ( i.hasMoreElements (  ) )
    {
        typeName = i.peekNextKey (  );
        archName = i.getNext (  );
        Ogre::ResourceManager::addCommonArchiveEx ( archName, typeName );
    }
}

int GraphicsEngine::step ( void )
{
    //Update Ogre's cubes positions with Ode's positions.
    int numberOfCubes = Cube::cubeList.size();
    for ( int currentCube = 0; currentCube < numberOfCubes; currentCube++ )
    {
        Cube::cubeList[currentCube]->updateOgrePosition();
        Cube::cubeList[currentCube]->updateOgreOrientation();
    }
    //Let the listener frames be started and ended: they are needed for particle systems.
    ogreRoot->_fireFrameStarted (  );
    systemData->ogreWindow->update (  );
    ogreRoot->_fireFrameEnded (  );
    
    return ( 0 );
}

GraphicsEngine::~GraphicsEngine ( void )
{
    log->put ( LOG_INFO, "Unloading ogre window data from memory..." );
    delete ( systemData->ogreWindow );

    //finally stop the log engine
    delete log;
}
