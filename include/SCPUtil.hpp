/*
 * File:   SCPUtil.hpp
 * 
 * Author: Robert Tizzard
 *
 * Created on April 10, 2017, 2:34 PM
 * 
 * Copyright 2017.
 * 
 */

#ifndef SCPUTIL_HPP
#define SCPUTIL_HPP

// =============
// INCLUDE FILES
// =============

//
// C++ STL
//

#include <string>
#include <vector>
#include <fstream>
#include <vector>

//
// Boost file system, string
//

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

//
// Antik utility
//

#include "CommonUtil.hpp"

//
// Antik Classes
//

#include "CSSHSession.hpp"
#include "CSCP.hpp"

namespace Antik {
    namespace SSH {
        
        void getFile(CSSHSession &sshSession, const std::string &sourceFile, const std::string &destinationFile);
        void putFile(CSSHSession &sshSession, const std::string &sourceFile, const std::string &destinationFile);      
        FileList getFiles(CSSHSession &sshSession, FileMapper &fileMapper, FileCompletionFn completionFn = nullptr);
        FileList putFiles(CSSHSession &sshSession, FileMapper &fileMapper, FileCompletionFn completionFn  = nullptr);

    } // namespace SSH
} // namespace Antik

#endif /* SCPUTIL_HPP */
