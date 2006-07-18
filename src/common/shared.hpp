/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef SHARED_HPP
#define SHARED_HPP
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>

// Shared pointer
#define SHARED_PTR(X,Y) class X; typedef boost::shared_ptr<X> Y

// String-indexed map and iterator
#define SHARED_MAP(X,Y,Z) typedef std::map <std::string, X> Y; typedef std::map <std::string, X>::iterator Z

// Shared pointer, string-indexed map and iterator
#define SHARED_PTR_MAP(X,Y,Z,T) class X; typedef boost::shared_ptr<X> Y; typedef std::map <std::string, Y> Z; typedef std::map <std::string, Y>::iterator T
#define SHARED_MAP_PTR(X,Y,Z,T) class X; typedef boost::shared_ptr<X> Y; typedef std::map <std::string, Y> Z; typedef std::map <std::string, Y>::iterator T

#endif