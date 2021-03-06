/*****************************************************************************\
|* Copyright (C) 2003, 2007 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

/*** Hack to permit forward declaration of DOMNode
 *** without including megabytes of includefiles.
 **/ 

#include "xercesc/util/XercesVersion.hpp"

namespace XERCES_CPP_NAMESPACE {
  class DOMNode;
};

