/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "SDL/SDL.h"
#include <shared.hpp>

//forward declaration
class SystemData;
SHARED_PTR_MAP(LogEngine, pLogEngine, LogEngines, LogEnginesIt);
namespace Ogre {
    class Camera;
};

/// Manages everything related to the gui rendering process.
/** Manages everything related to the process of system data (graphical user interface) graphics rendering on screen (menues and similar gadgets)
*/
class GuiEngine
{
  private:
    static GuiEngine * guiEnginePointer;
    // / Log engine used by the gui engine.
    /** This log engine allows the gui engine to write data in the log file, allowing to be differenciated from other engines.
    */
    pLogEngine log;
    // / Pointer to the system data, used by different engines in order to store common data.
    SystemData *systemData;
    // / Allows or disallows to show statistics on screen
    bool showStatistics;
    std::string telemetryText;
    std::string loadscreenText;
    std::string tempLine;
    Ogre::Camera * tmpOgreCamera;
    int telemetryLines;
    std::string time;
    std::string lapTime;
  public:
    // / Creates a new gui engine.
    /** Creates a new gui engine, with its associated own log engine. It initializes all necessary related data, such as initial graphics representation of system data (backgrounds, fonts,...), and sets up the underlying rendering libray (Ogre).
    */
      GuiEngine ();
    // / Deletes the gui engine.
    /** Deletes de gui engine, as well as its associated log engine. It also removes all the related data from memory.
    */
     ~GuiEngine ();
    // / Updates some statistics.
    /** Updates some statistics (framerates, etc...) in order to show them on screen if needed..
    */
    void updateStatistics (void);
    // / Renders all the gui on screen.
    /** Renders on screen all the system data (gui) for this frame.
    */
    int computeStep (void);

    void addTelemetryLine (const std::string & line);
    void hideLoadscreen ();
    void showLoadscreen ();
    void addLoadscreenLine (const std::string & line);
    void updateLapTime (double time);
    void updateTime (double time);

    static GuiEngine *getGuiEnginePointer();
};

