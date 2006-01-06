/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef XMLTAG_HPP
#define XMLTAG_HPP
#include <string>
#include <map>
#include <vector>
#include "logEngine.hpp"
#include "xmlParser.hpp"

class XmlTag;
typedef std::map <std::string, std::string> XmlAttributes;
typedef std::map <std::string, std::string>::iterator AttributesIt;
typedef std::map <std::string, bool> XmlAttributesBool;
typedef std::map <std::string, bool>::iterator AttributesBoolIt;
typedef std::vector <XmlTag *> Tags;
typedef std::vector <XmlTag *>::iterator TagsIt;

class XmlTag
{
    private:
        void construct(XmlTag * parent, DOMNode * n);
        std::string filePath;
        std::string name;
        XmlTag * parent;
        XmlAttributes attributes;
        XmlAttributesBool attributesRead;
        Tags tags;
        LogEngine * log;
    public:
        XmlTag (std::string xmlFilePath);
        XmlTag (XmlTag * parent, DOMNode * n);
        ~XmlTag (); 
        std::string getName();
        std::string getFullName();
        std::string getFilePath();
        std::string getAttribute(std::string attributeName);
        int nTags();
        XmlTag * getTag(int tagNumber);
};

#endif
