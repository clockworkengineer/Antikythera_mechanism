#include "HOST.hpp"
/*
 * File:   IMAPCommandTerminal.cpp
 * 
 * Author: Robert Tizzard
 * 
 * Created on October 24, 2016, 2:33 PM
 *
 * Copyright 2016.
 *
 */

//
// Program: IMAPCommandTerminal
//
// Description: Log on to a given IMAP server and execute commands typed in. The
// raw command responses are echoed back as default but parsed responses are displayed
// if specified in program options.
// 
/// 
// Dependencies: C11++, Classes (CMailIMAP, CMailIMAPParse, CMailIMAPBodyStruct),
//               Linux, Boost C++ Libraries.
//

//
// C++ STL definitions
//

#include <iostream>

//
// Classes
//

#include "CMailIMAP.hpp"
#include "CMailIMAPParse.hpp"
#include "CMailIMAPBodyStruct.hpp"

// Boost program options  & file system library definitions

#include "boost/program_options.hpp" 
#include <boost/filesystem.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

// Command line parameter data

struct ParamArgData {
    std::string userNameStr;        // Email account user name
    std::string userPasswordStr;    // Email account user name password
    std::string serverURLStr;       // SMTP server URL
    std::string configFileNameStr;  // Configuration file name
    bool bParsed;                   // true output parsed
    bool bBodystruct;               // Parsed output includes BODYSTRUCTS
};

//
// Exit with error message/status
//

void exitWithError(std::string errMsgStr) {

    // Closedown email, display error and exit.

    CMailIMAP::closedown();
    std::cerr << errMsgStr << std::endl;
    exit(EXIT_FAILURE);

}

//
// Add options common to both command line and config file
//

void addCommonOptions(po::options_description& commonOptions, ParamArgData& argData) {

    commonOptions.add_options()
            ("server,s", po::value<std::string>(&argData.serverURLStr)->required(), "IMAP Server URL and port")
            ("user,u", po::value<std::string>(&argData.userNameStr)->required(), "Account username")
            ("password,p", po::value<std::string>(&argData.userPasswordStr)->required(), "User password")
            ("parsed", "Response parsed")
            ("bodystruct", "Parsed output includes bodystructs");

}

//
// Read in and process command line arguments using boost.
//

void procCmdLine(int argc, char** argv, ParamArgData &argData) {

    // Default values

    argData.bParsed = false;
    argData.bBodystruct = false;

    // Define and parse the program options

    po::options_description commandLine("Program Options");
    commandLine.add_options()
            ("help", "Print help messages")
            ("config,c", po::value<std::string>(&argData.configFileNameStr)->required(), "Config File Name");

    addCommonOptions(commandLine, argData);

    po::options_description configFile("Config Files Options");

    addCommonOptions(configFile, argData);

    po::variables_map vm;

    try {

        // Process arguments

        po::store(po::parse_command_line(argc, argv, commandLine), vm);

        // Display options and exit with success

        if (vm.count("help")) {
            std::cout << "IMAPCommandTerminal" << std::endl << commandLine << std::endl;
            exit(EXIT_SUCCESS);
        }

        if (vm.count("config")) {
            if (fs::exists(vm["config"].as<std::string>().c_str())) {
                std::ifstream ifs{vm["config"].as<std::string>().c_str()};
                if (ifs) {
                    po::store(po::parse_config_file(ifs, configFile), vm);
                }
            } else {
                throw po::error("Specified config file does not exist.");
            }
        }

        // Display parsed output

        if (vm.count("parsed")) {
            argData.bParsed = true;
        }

        // Display parsed BODYSTRUCT output
        if (vm.count("bodystruct")) {
            argData.bBodystruct = true;
        }

        po::notify(vm);

    } catch (po::error& e) {
        std::cerr << "IMAPCommandTerminal Error: " << e.what() << std::endl << std::endl;
        std::cerr << commandLine << std::endl;
        exit(EXIT_FAILURE);
    }

}

//
// Walk data
//

struct WalkData {
    std::uint32_t count;
};

//
// Body structure tree walk function  too display body details.
//

void walkFn(std::unique_ptr<CMailIMAPBodyStruct::BodyNode>& bodyNode, CMailIMAPBodyStruct::BodyPart& bodyPart, std::shared_ptr<void>& walkData) {

    auto wlkData = static_cast<WalkData *> (walkData.get());

    std::cout << std::string(120, '#') << std::endl;

    std::cout << "PART NO = [" << bodyPart.partNoStr << "]" << std::endl;
    std::cout << "TYPE= [" << bodyPart.parsedPart->typeStr << "]" << std::endl;
    std::cout << "SUBTYPE= [" << bodyPart.parsedPart->subtypeStr << "]" << std::endl;
    std::cout << "PARAMETER LIST = [" << bodyPart.parsedPart->parameterListStr << "]" << std::endl;
    std::cout << "ID = [" << bodyPart.parsedPart->idStr << "]" << std::endl;
    std::cout << "DESCRIPTION = [" << bodyPart.parsedPart->descriptionStr << "]" << std::endl;
    std::cout << "ENCODING = [" << bodyPart.parsedPart->encodingStr << "]" << std::endl;
    std::cout << "SIZE = [" << bodyPart.parsedPart->sizeStr << "]" << std::endl;

    if (!bodyPart.parsedPart->textLinesStr.empty()) {
        std::cout << "TEXTLINES = [" << bodyPart.parsedPart->textLinesStr << "]" << std::endl;
    }

    if (!bodyPart.parsedPart->md5Str.empty()) {
        std::cout << "MD5 = [" << bodyPart.parsedPart->md5Str << "]" << std::endl;
    }

    if (!bodyPart.parsedPart->dispositionStr.empty()) {
        std::cout << "DISPOSITION = [" << bodyPart.parsedPart->dispositionStr << "]" << std::endl;
    }

    if (!bodyPart.parsedPart->languageStr.empty()) {
        std::cout << "LANGUAGE = [" << bodyPart.parsedPart->languageStr << "]" << std::endl;
    }

    if (!bodyPart.parsedPart->locationStr.empty()) {
        std::cout << "LOCATION = [" << bodyPart.parsedPart->locationStr << "]" << std::endl;
    }

    std::cout << "EXTENDED = [" << bodyPart.parsedPart->extendedStr << "]" << std::endl;

    std::cout << "MULTI-EXTENDED = [" << bodyNode->extendedStr << "]" << std::endl;

}

//
// Display parsed IMAP command response
//

void processIMAPResponse(CMailIMAP& imap, CMailIMAPParse::BASERESPONSE& parsedResponse) {

    std::cout << std::string(120, '*') << std::endl;
    if (parsedResponse->status != CMailIMAPParse::RespCode::OK) {
        std::cout << "COMMAND = {" << CMailIMAPParse::commandCodeString(parsedResponse->command) << "}" << std::endl;
        std::cout << "ERROR = {" << parsedResponse->errorMessageStr << "}" << std::endl;
        std::cout << std::string(120, '!') << std::endl;
        return;
    }

    switch (parsedResponse->command) {

        case CMailIMAPParse::Commands::SEARCH:
        {
            auto ptr = static_cast<CMailIMAPParse::SearchResponse *> (parsedResponse.get());
            std::cout << "COMMAND = " << CMailIMAPParse::commandCodeString(ptr->command) << std::endl;
            std::cout << "INDEXES = ";
            for (auto index : ptr->indexes) {
                std::cout << index << " ";
            }
            std::cout << std::endl;
            break;
        }

        case CMailIMAPParse::Commands::SELECT:
        {
            auto ptr = static_cast<CMailIMAPParse::SelectResponse *> (parsedResponse.get());
            std::cout << "COMMAND = " << CMailIMAPParse::commandCodeString(ptr->command) << std::endl;
            std::cout << "MAILBOX = " << ptr->mailBoxNameStr << std::endl;
            std::cout << "ACCESS = " << ptr->mailBoxAccessStr << std::endl;
            for (auto resp : ptr->responseMap) {
                std::cout << resp.first << " = " << resp.second << std::endl;
            }
            break;
        }

        case CMailIMAPParse::Commands::EXAMINE:
        {
            auto ptr = static_cast<CMailIMAPParse::ExamineResponse *> (parsedResponse.get());
            std::cout << "COMMAND = " << CMailIMAPParse::commandCodeString(ptr->command) << std::endl;
            std::cout << "MAILBOX = " << ptr->mailBoxNameStr << std::endl;
            std::cout << "ACCESS = " << ptr->mailBoxAccessStr << std::endl;
            for (auto resp : ptr->responseMap) {
                std::cout << resp.first << " = " << resp.second << std::endl;
            }
            break;
        }

        case CMailIMAPParse::Commands::LIST:
        {
            auto ptr = static_cast<CMailIMAPParse::ListResponse *> (parsedResponse.get());
            std::cout << "COMMAND = " << CMailIMAPParse::commandCodeString(ptr->command) << std::endl;
            for (auto mailboxEntry : ptr->mailBoxList) {
                std::cout << "NAME = " << mailboxEntry.mailBoxNameStr << std::endl;
                std::cout << "ATTRIB = " << mailboxEntry.attributesStr << std::endl;
                std::cout << "DEL = " << mailboxEntry.hierDel << std::endl;
            }
            break;
        }

        case CMailIMAPParse::Commands::LSUB:
        {
            auto ptr = static_cast<CMailIMAPParse::LSubResponse *> (parsedResponse.get());
            std::cout << "COMMAND = " << CMailIMAPParse::commandCodeString(ptr->command) << std::endl;
            for (auto mailboxEntry : ptr->mailBoxList) {
                std::cout << "NAME = " << mailboxEntry.mailBoxNameStr << std::endl;
                std::cout << "ATTRIB = " << mailboxEntry.attributesStr << std::endl;
                std::cout << "DEL = " << mailboxEntry.hierDel << std::endl;
            }
            break;
        }

        case CMailIMAPParse::Commands::STATUS:
        {
            auto ptr = static_cast<CMailIMAPParse::StatusResponse *> (parsedResponse.get());
            std::cout << "COMMAND = " << CMailIMAPParse::commandCodeString(ptr->command) << std::endl;
            std::cout << "MAILBOX = " << ptr->mailBoxNameStr << std::endl;
            for (auto resp : ptr->responseMap) {
                std::cout << resp.first << " = " << resp.second << std::endl;
            }
            break;
        }

        case CMailIMAPParse::Commands::EXPUNGE:
        {
            auto ptr = static_cast<CMailIMAPParse::ExpungeResponse *> (parsedResponse.get());
            std::cout << "COMMAND = " << CMailIMAPParse::commandCodeString(ptr->command) << std::endl;
            std::cout << "EXISTS = ";
            for (auto index : ptr->exists) {
                std::cout << index << " ";
            }
            std::cout << std::endl;
            std::cout << "EXPUNGED = ";
            for (auto index : ptr->expunged) {
                std::cout << index << " ";
            }
            std::cout << std::endl;
            break;
        }

        case CMailIMAPParse::Commands::STORE:
        {
            auto ptr = static_cast<CMailIMAPParse::StoreResponse *> (parsedResponse.get());
            std::cout << "COMMAND = " << CMailIMAPParse::commandCodeString(ptr->command) << std::endl;
            for (auto storeEntry : ptr->storeList) {
                std::cout << "INDEX = " << storeEntry.index << std::endl;
                std::cout << "FLAGS = " << storeEntry.flagsListStr << std::endl;
            }
            break;
        }

        case CMailIMAPParse::Commands::CAPABILITY:
        {
            auto ptr = static_cast<CMailIMAPParse::CapabilityResponse *> (parsedResponse.get());
            std::cout << "COMMAND = " << CMailIMAPParse::commandCodeString(ptr->command) << std::endl;
            std::cout << "CAPABILITIES = " << ptr->capabilitiesStr << std::endl;
            break;
        }

        case CMailIMAPParse::Commands::FETCH:
        {
            auto ptr = static_cast<CMailIMAPParse::FetchResponse *> (parsedResponse.get());
            std::cout << "COMMAND = " << CMailIMAPParse::commandCodeString(ptr->command) << std::endl;

            for (auto fetchEntry : ptr->fetchList) {
                std::cout << "INDEX = " << fetchEntry.index << std::endl;
                for (auto resp : fetchEntry.responseMap) {
                    if (resp.first.compare(CMailIMAP::kBODYSTRUCTUREStr) == 0) {
                        std::unique_ptr<CMailIMAPBodyStruct::BodyNode> treeBase{ new CMailIMAPBodyStruct::BodyNode()};
                        std::shared_ptr<void> walkData{ new WalkData()};
                        CMailIMAPBodyStruct::consructBodyStructTree(treeBase, resp.second);
                        CMailIMAPBodyStruct::walkBodyStructTree(treeBase, walkFn, walkData);
                    } else {
                        std::cout << resp.first << " = " << resp.second << std::endl;
                    }
                }

            }
            break;
        }

        case CMailIMAPParse::Commands::NOOP:
        case CMailIMAPParse::Commands::IDLE:
        {
            auto ptr = static_cast<CMailIMAPParse::NoOpResponse *> (parsedResponse.get());
            std::cout << "COMMAND = " << CMailIMAPParse::commandCodeString(ptr->command) << std::endl;
            if (!ptr->rawResponse.empty()) {
                for (auto str : ptr->rawResponse) {
                    std::cout << str << std::endl;
                }
            } else {
                std::cout << "All quiet!!!" << std::endl;
            }
            break;
        }

        default:
        {
            auto ptr = static_cast<CMailIMAPParse::BaseResponse *> (parsedResponse.get());
            std::cout << "COMMAND = " << CMailIMAPParse::commandCodeString(ptr->command) << std::endl;
            break;
        }
    }

    std::cout << std::string(120, '+') << std::endl;

}

// ============================
// ===== MAIN ENTRY POINT =====
// ============================

int main(int argc, char** argv) {

    try {

        ParamArgData argData;
        CMailIMAP imap;
        std::deque<std::string> startupCommandsStr;

        // Read in command line parameters and process

        procCmdLine(argc, argv, argData);

        // Initialise CMailIMAP internals

        CMailIMAP::init();

        std::cout << "SERVER [" << argData.serverURLStr << "]" << std::endl;
        std::cout << "USER [" << argData.userNameStr << "]" << std::endl;

        // Set mail account user name and password

        imap.setServer(argData.serverURLStr);
        imap.setUserAndPassword(argData.userNameStr, argData.userPasswordStr);

        std::string commandLineStr;

        // Connect

        imap.connect();

        // Loop executing commands until exit

        do {

            // Process any startup commands first

            if (!startupCommandsStr.empty()) {
                commandLineStr = startupCommandsStr.front();
                startupCommandsStr.pop_front();
            }

            // If command line empty them prompt and read in new command

            if (commandLineStr.empty()) {
                std::cout << "COMMAND>";
                std::getline(std::cin, commandLineStr);
            }

            // exit

            if (commandLineStr.compare("exit") == 0) break;

            // Run command

            if (!commandLineStr.empty()) {

                std::string commandResponseStr(imap.sendCommand(commandLineStr));

                if (argData.bParsed) {
                    CMailIMAPParse::BASERESPONSE commandResponse(CMailIMAPParse::parseResponse(commandResponseStr));
                    processIMAPResponse(imap, commandResponse);
                } else {
                    std::cout << commandResponseStr << std::endl;
                }

                commandLineStr.clear();

            }

        } while (true);

        //
        // Catch any errors
        //    

    } catch (CMailIMAP::Exception &e) {
        exitWithError(e.what());
    } catch (std::exception & e) {
        exitWithError(std::string("Standard exception occured: [") + e.what() + "]");
    }

    CMailIMAP::closedown();

    exit(EXIT_SUCCESS);

}