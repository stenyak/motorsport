
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

#include "physicsEngine.hpp"

PhysicsEngine::PhysicsEngine ()
{
    PhysicsData *data = new PhysicsData;
    data->physics = this;
    data->cfmValue = -1;
    data->erpValue = -1;
    data->stepType = 1;
    data->dWorldStepFast1MaxIterations = 100;
    processXmlFile ("physicsConfig.xml", &PhysicsEngine::processPhysicsConfigFile, (void *) data);

    // first of all start the logger (automatically logs the start of itself)
    log = new LogEngine (data->localLogLevel, data->localLogName);
    log->put (LOG_INFO, "Temporary parsing data already loaded into memory...");

    // get the direction of the graphics data
    log->put (LOG_INFO, "Setting up data pointers...");
    worldData = WorldData::getWorldDataPointer ();
    systemData = SystemData::getSystemDataPointer ();

    log->put (LOG_INFO, "Setting physics data");
    systemData->physicsDesiredFrequency = data->frequency;
    systemData->physicsTimeStep = 1000 / systemData->physicsDesiredFrequency;
    log->format (LOG_INFO, "Physics rate set @ %i Hz (%i ms)", systemData->physicsDesiredFrequency, systemData->physicsTimeStep);

    log->put (LOG_INFO, "Creating ODE world");
    worldData->worldID = dWorldCreate ();
    worldData->spaceID = dHashSpaceCreate (0);
    worldData->jointGroupID = dJointGroupCreate (0);

    log->put ( LOG_INFO, "Setting ODE world gravity");
    dWorldSetGravity (worldData->worldID, 0,0,-0.000098);

    if (data->cfmValue != -1)
    {
        log->put (LOG_INFO, "Setting ODE cfm value");
        dWorldSetCFM (worldData->worldID, data->cfmValue);
    }
    if (data->erpValue != -1)
    {
        log->put (LOG_INFO, "Setting ODE erp value");
        dWorldSetERP (worldData->worldID, data->erpValue);
    }

    log->put (LOG_INFO, "Setting ODE step type");
    stepType = data->stepType;

    log->put (LOG_INFO, "Setting the max. number of iterations to be calculated with dWorldStepFast1");
    dWorldStepFast1MaxIterations = data->dWorldStepFast1MaxIterations;

    log->put (LOG_INFO, "Unloading temporary parsing data from memory...");
    delete[](data->localLogName);
    delete data;
}


void PhysicsEngine::nearCallback (void *data, dGeomID o1, dGeomID o2)
{
    int i, n;
    dBodyID b1 = dGeomGetBody (o1);
    dBodyID b2 = dGeomGetBody (o2);
    if (b1 && b2 && dAreConnected (b1, b2))
        return;

    const int N = 10;
    dContact contact[N];
    n = dCollide (o1, o2, N, &contact[0].geom, sizeof (dContact));
    if (n > 0)
    {
        for (i = 0; i < n; i++)
        {
            contact[i].surface.mode = dContactSlip1 | dContactSlip2 | dContactSoftERP | dContactSoftCFM | dContactApprox1;
            if (dGeomGetClass (o1) == dSphereClass || dGeomGetClass (o2) == dSphereClass)
                contact[i].surface.mu = 20;
            else
                contact[i].surface.mu = 0.5;
            contact[i].surface.slip1 = 0.0;
            contact[i].surface.slip2 = 0.0;
            contact[i].surface.soft_erp = 0.8;
            contact[i].surface.soft_cfm = 0.01;
            dJointID c = dJointCreateContact (WorldData::getWorldDataPointer ()->worldID, WorldData::getWorldDataPointer ()->jointGroupID, contact + i);
            dJointAttach (c, dGeomGetBody (o1), dGeomGetBody (o2));
        }
    }
}

int PhysicsEngine::computeStep (void)
//makes the graphics engine draw one frame
{
    // mega-verbosity
    log->put (LOG_TRACE, "Doing an step: calculating a physics step");

    int size = Cube::cubeList.size ();
    for (int i = 0; i < size; i++)
    {
        Cube::cubeList[i]->stepPhysics ();
    }

    size = Body::bodyList.size ();
    for (int i = 0; i < size; i++)
    {
        Body::bodyList[i]->stepPhysics ();
    }

    dSpaceCollide (worldData->spaceID, 0, &nearCallback);
    switch (stepType)
    {
    default:
    case 1:
        // traditional (x^y), theorycally slowest, and most accurate physics calculations:
        dWorldStep (worldData->worldID, systemData->physicsTimeStep);
        break;
    case 2:
        // alternative (x*y), fastest and less accurate physics calculations:
        dWorldStepFast1 (worldData->worldID, systemData->physicsTimeStep, dWorldStepFast1MaxIterations);
    }
    dJointGroupEmpty (worldData->jointGroupID);

    // camera should be a physics object?
    {
        float x = 0, z = 0;
        // translation of the camera, advancing or strafing
        x += (Camera::cameraList[0]->goRight) ? systemData->physicsTimeStep : 0;
        x -= (Camera::cameraList[0]->goLeft) ? systemData->physicsTimeStep : 0;
        z -= (Camera::cameraList[0]->goForward) ? systemData->physicsTimeStep : 0;
        z += (Camera::cameraList[0]->goBack) ? systemData->physicsTimeStep : 0;
        x /= 100;
        z /= 100;
        Camera::cameraList[0]->ogreCamera->moveRelative (Ogre::Vector3 (x, 0, z));
    }
    {
        float x = 0, z = 0;
        x -= Camera::cameraList[0]->getRotateRight ();
        x += Camera::cameraList[0]->getRotateLeft ();
        z += Camera::cameraList[0]->getRotateUp ();
        z -= Camera::cameraList[0]->getRotateDown ();
        x /= 100;
        z /= 100;
        Camera::cameraList[0]->ogreCamera->yaw (x * (systemData->physicsTimeStep));
        Camera::cameraList[0]->ogreCamera->pitch (z * (systemData->physicsTimeStep));
    }

    return (0);
}

PhysicsEngine::~PhysicsEngine (void)
{
    log->put (LOG_INFO, "Destroying ODE world");
    dSpaceDestroy (worldData->spaceID);
    log->put (LOG_INFO, "Destroying ODE main collision space");
    dWorldDestroy (worldData->worldID);
    log->put (LOG_INFO, "Destroying ODE joints group");
    dJointGroupDestroy (worldData->jointGroupID);
    log->put (LOG_INFO, "Shutting down ODE");
    dCloseODE ();

    // finally stop the log engine
    delete log;
}
