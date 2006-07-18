/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "world.hpp"
#include "vehicle.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "body.hpp"
#include "engine.hpp"
#include "driveJoint.hpp"
#include "gearbox.hpp"
#include "finalDrive.hpp"
#include "wheel.hpp"
#include "suspension.hpp"
#include "pedal.hpp"
#include "camera.hpp"
#include "system.hpp"
#include "SDL.h"
#include "SDL/SDL_keysym.h"
#include "quaternion.hpp"

pVehicle Vehicle::create(std::string vehicleName)
{
    pVehicle vehicle(new Vehicle(vehicleName));
    return vehicle;
}
Vehicle::Vehicle (std::string vehicleName)
    :WorldObject(vehicleName)
{
    setPath(Paths::vehicle(vehicleName));
    System::get()->setCurrentPath(Paths::vehicle(vehicleName));
    setXmlPath(Paths::vehicleXml(vehicleName));
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPath(), "FileSystem", "vehicles." + vehicleName);
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPath()+"skybox.zip","Zip","vehicles."+vehicleName);
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("vehicles." + vehicleName);

    XmlTag * tag = new XmlTag (getXmlPath());
    construct (tag);
    delete tag;

    userDriver = false;
}

Vehicle::~Vehicle ()
{
    log->__format(LOG_DEVELOPER, "Deleting components");
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        delete (i->second);
    }
    components.clear();
}

void Vehicle::setUserDriver ()
{
    userDriver = true;
    log->log (LOG_ENDUSER, LOG_TELEMETRY, "VehSpeed EngineSpeed DiffAngularVel RRWhAngulVel RLWhAngulVel Gear Distance");

    // engage neutral gear
    getGearbox("gearbox")->setGear(2);

    // spread the news to the necessary (input-able) vehicle parts
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        Pedal * tmpPedal;
        if ( (tmpPedal = dynamic_cast<Pedal*>(i->second.get())) != 0) tmpPedal->setUserDriver();
        Wheel * tmpWheel;
        if ( (tmpWheel = dynamic_cast<Wheel*>(i->second.get())) != 0) tmpWheel->setUserDriver();
        Suspension * tmpSusp;
        if ( (tmpSusp = dynamic_cast<Suspension*>(i->second.get())) != 0) tmpSusp->setUserDriver();
    }
}

void Vehicle::construct (XmlTag * tag)
{
    if (tag->getName() == "vehicle")
    {
        setName (     tag->getAttribute("name"));
        description = tag->getAttribute("description");
        author =      tag->getAttribute("author");
        contact =     tag->getAttribute("contact");
        license =     tag->getAttribute("license");

        components["rearDiff"] = LSD::create();
        components["transfer"] = Gear::create();
        XmlTag * t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
        {
            if (t->getName() == "body")         components["body"]       = Body::create       (t);
            if (t->getName() == "engine")       components["engine"]     = Engine::create     (t);
            if (t->getName() == "clutch")       components["clutch"]     = Clutch::create     (t);
            if (t->getName() == "gearbox")      components["gearbox"]    = Gearbox::create    (t);
            if (t->getName() == "finalDrive")   components["finalDrive"] = FinalDrive::create (t);
            if (t->getName() == "pedal")
            {
                pPedal tmp = Pedal::create (t);
                components[tmp->getName()] = tmp;
            }
            if (t->getName() == "wheel")
            {
                pWheel tmp = Wheel::create (t);
                components[tmp->getName()] = tmp;
                System::get()->ogreWindow->update ();
            }
            if (t->getName() == "suspension.unidimensional")
            {
                pUnidimensional tmp = Unidimensional::create (t);
                components[tmp->getName()] = tmp;
                System::get()->ogreWindow->update ();
            }
            if (t->getName() == "suspension.fixed")
            {
                pFixed tmp = Fixed::create (t);
                components[tmp->getName()] = tmp;
                System::get()->ogreWindow->update ();
            }
            if (t->getName() == "suspension.doublewishbone")
            {
                pDoubleWishbone tmp = DoubleWishbone::create (t);
                components[tmp->getName()] = tmp;
                System::get()->ogreWindow->update ();
            }
            if (t->getName() == "camera")
            {
                pCamera tmp = Camera::create (t);
                cameras[tmp->getName()] = tmp;
            }
        }
    }

    // spread the news to the necessary (input-able) vehicle parts
    log->__format(LOG_DEVELOPER, "Setting some drive joint pointers...");
    getDriveJoint("clutch")->setOutputPointer(getDriveMass("gearbox"));
    getDriveJoint("transfer")->setOutputPointer(getDriveMass("finalDrive"));
    
    getDriveJoint("clutch")->setInputPointer(getDriveMass("engine"));
    getDriveJoint("transfer")->setInputPointer(getDriveMass("gearbox"));
    getDriveJoint("rearDiff")->setInputPointer(getDriveMass("finalDrive"));
        
    log->__format(LOG_DEVELOPER, "Placing wheels on suspensions...");
    placeWheelsOnSuspensions();
    log->__format(LOG_DEVELOPER, "Bolting wheels to suspensions...");
    boltWheelsToSuspensions();

    log->__format(LOG_DEVELOPER, "Setting some more drive joint pointers...");
    getDriveJoint("rearDiff")->setOutputPointer(getDriveMass("rr wheel"));
    getLSD("rearDiff")->setOutputPointer2(getDriveMass("rl wheel"));

    getEngine("engine")->setGasPedal(getPedal("gasPedal"));
    getClutch("clutch")->setClutchPedal(getPedal("clutchPedal"));
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        Wheel * tmpWheel;
        if ( (tmpWheel = dynamic_cast<Wheel*>(i->second.get())) != 0)
        {
            tmpWheel->setBrakePedal(getPedal("brakePedal"));
        }
    }

    getDriveJoint("clutch")->enable();
    getDriveJoint("transfer")->enable();
    getDriveJoint("rearDiff")->enable();
    
    //start in 1st gear (hardcoded to 2)
    // this helps brake the vehicle (since it's not user-controlled by default and would roll around freely otherwise)
    getGearbox("gearbox")->setGear(2);

    stepGraphics();
}

void Vehicle::setContainer (pWorldObject container)
{
    WorldObject::setContainer(container);
    // tell all the objects who's the boss.
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        i->second->setContainer(shared_from_this());
    }
    getGearbox("gearbox")->setContainer(shared_from_this());
    for (CamerasIt i = cameras.begin(); i != cameras.end(); i++)
    {
        i->second->setContainer(shared_from_this());
    }
}

void Vehicle::stepGraphics ()
{
    base->stepGraphics();

    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        DoubleWishbone * tmpSusp;
        if ( (tmpSusp = dynamic_cast<DoubleWishbone*>(i->second.get())) != 0) tmpSusp->stepGraphics();
        Wheel * tmpWheel;
        if ( (tmpWheel = dynamic_cast<Wheel*>(i->second.get())) != 0) tmpWheel->stepGraphics();
        Body * tmpBody;
        if ( (tmpBody = dynamic_cast<Body*>(i->second.get())) != 0) tmpBody->stepGraphics();
    }
}

pOdeObject Vehicle::getMainOdeObject()
{
    return getObject("body")->getMainOdeObject();
}

void Vehicle::setPosition (Vector3d position)
{
    Vector3d posDiff = position - getPosition();
    log->__format (LOG_DEVELOPER,"Difference in vehicle position: (%f, %f, %f).", posDiff.x, posDiff.y, posDiff.z);
    getObject("body")->setPosition ( getObject("body")->getPosition() + posDiff );
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        Wheel * tmpWheel;
        if ( (tmpWheel = dynamic_cast<Wheel*>(i->second.get())) != 0)
            tmpWheel->getMainOdeObject()->setPosition ( tmpWheel->getMainOdeObject()->getPosition() + posDiff );
        Suspension * tmpSusp;
        if ( (tmpSusp = dynamic_cast<Suspension*>(i->second.get())) != 0)
            tmpSusp->setPosition ( tmpSusp->getPosition() + posDiff );
    }
}

Vector3d Vehicle::getPosition ()
{
    return getObject("body")->getPosition();
}

void Vehicle::applyRotation (Quaternion rotation)
{
    // apply rotation to wheels
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        Wheel * tmpWheel;
        if ( (tmpWheel = dynamic_cast<Wheel*>(i->second.get())) != 0)
            tmpWheel->applyRotation (rotation);
        Suspension * tmpSusp;
        if ( (tmpSusp = dynamic_cast<Suspension*>(i->second.get())) != 0)
            tmpSusp->applyRotation (rotation);
    }
    // apply rotation to body
    getObject("body")->applyRotation (rotation);
}

Quaternion Vehicle::getRotation ()
{
    return getObject("body")->getRotation();
}

void Vehicle::placeWheelsOnSuspensions()
{
    getWheel("fr wheel")->applyRotation(getSuspension("fr suspension")->getSecondLinkRotation());
    getWheel("fr wheel")->setPosition(getSuspension("fr suspension")->getSecondLinkPosition());
    getWheel("fl wheel")->applyRotation(getSuspension("fl suspension")->getSecondLinkRotation());
    getWheel("fl wheel")->setPosition(getSuspension("fl suspension")->getSecondLinkPosition());
    getWheel("rr wheel")->applyRotation(getSuspension("rr suspension")->getSecondLinkRotation());
    getWheel("rr wheel")->setPosition(getSuspension("rr suspension")->getSecondLinkPosition());
    getWheel("rl wheel")->applyRotation(getSuspension("rl suspension")->getSecondLinkRotation());
    getWheel("rl wheel")->setPosition(getSuspension("rl suspension")->getSecondLinkPosition());
}

void Vehicle::boltWheelsToSuspensions()
{
    getSuspension("fr suspension")->attach(getObject("body"), getObject("fr wheel"));
    getSuspension("fl suspension")->attach(getObject("body"), getObject("fl wheel"));
    getSuspension("rr suspension")->attach(getObject("body"), getObject("rr wheel"));
    getSuspension("rl suspension")->attach(getObject("body"), getObject("rl wheel"));
}

void Vehicle::stepPhysics ()
{
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        Pedal * tmpPedal;
        if ( (tmpPedal = dynamic_cast<Pedal*>(i->second.get())) != 0)
        {
            tmpPedal->stepPhysics();
        }
    }
    if ( userDriver )
    {
        // check the gearshift levers
        if(System::get()->axisMap[getIDKeyboardKey(SDLK_a)]->getValue() == 1)
        {
            getGearbox("gearbox")->gearUp();
        }
        if(System::get()->axisMap[getIDKeyboardKey(SDLK_z)]->getValue() == 1)
        {
            getGearbox("gearbox")->gearDown();
        }
    }

    // step torque transfer components first
    getClutch("clutch")->stepPhysics();
    getGear("transfer")->stepPhysics();
    getLSD("rearDiff")->stepPhysics();
        
    // step rigid bodies    
    getEngine("engine")->stepPhysics();
    getGearbox("gearbox")->stepPhysics();
    getFinalDrive("finalDrive")->stepPhysics();
    getBody("body")->stepPhysics();
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        Suspension * tmpSusp;
        if ( (tmpSusp = dynamic_cast<Suspension*>(i->second.get())) != 0) tmpSusp->stepPhysics();
        Wheel * tmpWheel;
        if ( (tmpWheel = dynamic_cast<Wheel*>(i->second.get())) != 0) tmpWheel->stepPhysics();
    }

    // print telemetry data
    if ( userDriver )
    {
        const dReal * tmp = dBodyGetLinearVel(getObject("body")->getMainOdeObject()->getBodyID());
        double velocity = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
        tmp = dBodyGetPosition(getObject("body")->getMainOdeObject()->getBodyID());
        double distance = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
        log->log (LOG_ENDUSER, LOG_TELEMETRY | LOG_FILE, "%9.5f %12.8f %12.8f %12.8f %12.8f %s %12.8f", velocity, getDriveMass("engine")->getOutputAngularVel(), getDriveMass("finalDrive")->getInputAngularVel(), getWheel("rr wheel")->getInputAngularVel(), getWheel("rl wheel")->getInputAngularVel(), getGearbox("gearbox")->getCurrentGearName().c_str(), distance);
    }
}

pBody Vehicle::getBody (std::string name)
{
    pBody tmp = boost::dynamic_pointer_cast<Body>(components[name]);
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Body");
    return (tmp);
    //return (components[name]);
}
pDriveMass Vehicle::getDriveMass (std::string name)
{
    pDriveMass tmp = boost::dynamic_pointer_cast<DriveMass>(components[name]);
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "DriveMass");
    return (tmp);
}
pDriveJoint Vehicle::getDriveJoint (std::string name)
{
    pDriveJoint tmp = boost::dynamic_pointer_cast<DriveJoint>(components[name]);
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "DriveJoint");
    return (tmp);
}
pClutch Vehicle::getClutch (std::string name)
{
    pClutch tmp = boost::dynamic_pointer_cast<Clutch>(components[name]);
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Clutch");
    return (tmp);
}
pGear Vehicle::getGear (std::string name)
{
    pGear tmp = boost::dynamic_pointer_cast<Gear>(components[name]);
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Gear");
    return (tmp);
}
pLSD Vehicle::getLSD (std::string name)
{
    pLSD tmp = boost::dynamic_pointer_cast<LSD>(components[name]);
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "LSD");
    return (tmp);
}
pEngine Vehicle::getEngine (std::string name)
{
    pEngine tmp = boost::dynamic_pointer_cast<Engine>(components[name]);
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Engine");
    return (tmp);
}
pFinalDrive Vehicle::getFinalDrive (std::string name)
{
    pFinalDrive tmp = boost::dynamic_pointer_cast<FinalDrive>(components[name]);
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "FinalDrive");
    return (tmp);
}
pGearbox Vehicle::getGearbox (std::string name)
{
    pGearbox tmp = boost::dynamic_pointer_cast<Gearbox>(components[name]);
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Gearbox");
    return (tmp);
}
pGearboxGear Vehicle::getGearboxGear (std::string name)
{
    pGearboxGear tmp = boost::dynamic_pointer_cast<GearboxGear>(components[name]);
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "GearboxGear");
    return (tmp);
}
pPedal Vehicle::getPedal (std::string name)
{
    pPedal tmp = boost::dynamic_pointer_cast<Pedal>(components[name]);
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Pedal");
    return (tmp);
}
pSuspension Vehicle::getSuspension (std::string name)
{
    pSuspension tmp = boost::dynamic_pointer_cast<Suspension>(components[name]);
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Suspension");
    return (tmp);
}
pWheel Vehicle::getWheel (std::string name)
{
    pWheel tmp = boost::dynamic_pointer_cast<Wheel>(components[name]);
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Wheel");
    return (tmp);
}
pWorldObject Vehicle::getObject (std::string name)
{
   if (components.find(name) == components.end())
   log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "WorldObject");
   return components[name];
}
