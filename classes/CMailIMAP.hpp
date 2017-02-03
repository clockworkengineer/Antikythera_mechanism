/*
 * File:   CMailIMAP.hpp
 * 
 * Author: Robert Tizzard
 * 
 * Created on Januray 24, 2017, 2:33 PM
 *
 * Copyright 2016.
 
 */

#ifndef CMAILIMAP_HPP
#define CMAILIMAP_HPP

//
// C++ STL definitions
//

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <sstream>

//
// libcurl definitions
//

#include <curl/curl.h>

// ================
// CLASS DEFINITION
// ================

class CMailIMAP {
public:

    // ==========================
    // PUBLIC TYPES AND CONSTANTS
    // ==========================

    struct CommandData {
        std::string tag;
        std::string command;
        std::string commandLine;
    };
    
    //
    // STORE responses data
    //
    
    struct FetchRespData {
        uint64_t index;                         // EMail Index/UID
     //   std::string flags;                      // EMail flags
     //   std::vector<std::string> body;          // Fetch body
     //   uint64_t bodyLength;                    // Fetch body length
     //   std::string envelope;                   // Fetch envelope
     //   std::string bodyStructure;              // Fetch body structure
     //   std::string fastBody;                   // Fetch fast body 
        std::unordered_map<std::string, std::string> responseMap;
    };
    
    //
    // LIST response data
    //
    
    struct ListRespData {
        uint8_t     hierDel;       // Hierarchy Delimeter
        std::string attributes;    // Mailbox attributes
        std::string name;          // Mailbox name
    };
    
    //
    // STORE responses data
    //
    
    struct StoreRespData {
        uint64_t index;      // EMail Index/UID
        std::string flags;   // EMail flags
    };

    //
    // Enumeration of command codes (not used at present)
    //

    enum Commands {
        STARTTLS = 0,   // Supported
        AUTHENTICATE,   // Supported
        LOGIN,          // Supported
        CAPABILITY,     // Supported
        SELECT,         // Supported
        EXAMINE,        // Supported
        CREATE,         // Supported
        DELETE,         // Supported
        RENAME,         // Supported
        SUBSCRIBE,      // Supported
        UNSUBSCRIBE,    // Supported
        LIST,           // Supported
        LSUB,           // Supported
        STATUS,         // Supported
        APPEND,         // <-- Special handling (come back)
        CHECK,          // Supported
        CLOSE,          // Supported
        EXPUNGE,        // Supported
        SEARCH,         // Supported
        FETCH,          // Supported
        STORE,          // Supported
        COPY,           // Supported
        UID,            // <-- Special handling (come back)
        NOOP,           // <-- Special handling (come back)
        LOGOUT,         // <-- Special handling (come back)
        IDLE            // Supported (waitOnIdle)
    };

    //
    // Command response code enumeration.
    //
    
    enum class RespCode {
        OK = 0,
        NO,
        BAD
    };

    //
    // Command string constants
    //
    
    static const std::string kSEARCHStr;
    static const std::string kSELECTStr;
    static const std::string kEXAMINEStr;
    static const std::string kCREATEStr;
    static const std::string kDELETEStr;
    static const std::string kRENAMEStr;
    static const std::string kLOGINStr;
    static const std::string kSUBSCRIBEStr;
    static const std::string kUNSUBSCRIBEStr;
    static const std::string kLISTStr;
    static const std::string kLSUBStr;
    static const std::string kSTATUSStr;
    static const std::string kAPPENDStr;
    static const std::string kCHECKStr;
    static const std::string kCLOSEStr;
    static const std::string kEXPUNGEStr;
    static const std::string kFETCHStr;
    static const std::string kSTOREStr;
    static const std::string kCOPYStr;
    static const std::string kNOOPStr;
    static const std::string kLOGOUTStr;
    static const std::string kIDLEStr;
    static const std::string kCAPABILITYStr;

    //
    // Decoded command structures.
    // 
    
    struct BResponse {
        std::string command;
        RespCode status;
        std::string errorMessage;
    };

    struct SearchResponse : public BResponse {
        std::vector<uint64_t> indexes;
    };

    struct SelectResponse : public BResponse {
        std::string mailBoxName;
        std::string mailBoxAccess;
        std::unordered_map<std::string, std::string> responseMap;
    };

    struct ExamineResponse : public SelectResponse {
    };

    struct ListResponse : public BResponse {
        std::vector<ListRespData> mailBoxList;
    };
    
    struct LSubResponse : public ListResponse {
    };
    
    struct StatusResponse : public BResponse {
        std::string mailBoxName;
        std::unordered_map<std::string, std::string> responseMap;
    };
    
    struct ExpungeResponse : public BResponse {
        std::vector<uint64_t> exists;
        std::vector<uint64_t> expunged;
    };
   
    struct StoreResponse : public BResponse {
        std::vector<StoreRespData> storeList;
    };
    
    struct CapabilityResponse : public BResponse {
        std::string capabilityList;
    };
    
    struct FetchResponse : public BResponse {
        std::vector<FetchRespData> fetchList;
    };
    
    //
    // Command response structure shared pointer wrapper.
    //
    
    typedef  std::shared_ptr<BResponse> BASERESPONSE; 
    typedef  std::shared_ptr<SearchResponse> SEARCHRESPONSE;
    typedef  std::shared_ptr<SelectResponse> SELECTRESPONSE;
    typedef  std::shared_ptr<ExamineResponse> EXAMINERESPONSE;
    typedef  std::shared_ptr<ListResponse> LISTRESPONSE;
    typedef  std::shared_ptr<LSubResponse> LSUBRESPONSE;
    typedef  std::shared_ptr<StatusResponse> STATUSRESPONSE;
    typedef  std::shared_ptr<ExpungeResponse> EXPUNGERESPONSE;
    typedef  std::shared_ptr<StoreResponse> STORERESPONSE;
    typedef  std::shared_ptr<CapabilityResponse> CAPABILITYRESPONSE;
    typedef  std::shared_ptr<FetchResponse> FETCHRESPONSE;
    
    // ============
    // CONSTRUCTORS
    // ============

    //
    // Main constructor
    //

    CMailIMAP();

    // ==========
    // DESTRUCTOR
    // ==========

    virtual ~CMailIMAP();

    // ==============
    // PUBLIC METHODS
    // ==============

    //
    // Set email server account details
    //

    void setServer(const std::string& serverURL);
    void setUserAndPassword(const std::string& userName, const std::string& userPassword);

    //
    // IMAP connect, send command and disconnect
    //

    void connect(void);
    CMailIMAP::BASERESPONSE sendCommand(const std::string& commandLine);
    void disconnect(void);

    // IMAP initialization and closedown processing

    static void init();
    static void closedown();


    // ================
    // PUBLIC VARIABLES
    // ================

private:

    // ===========================
    // PRIVATE TYPES AND CONSTANTS
    // ===========================

    static const std::string kEOLStr;       // End of line
    
    //
    // Response strings
    //
    
    static const std::string kUntaggedStr;
    static const std::string kOKStr;
    static const std::string kBADStr;
    static const std::string kNOStr;
    static const std::string kFLAGSStr;
    static const std::string kPERMANENTFLAGSStr;
    static const std::string kUIDVALIDITYStr;
    static const std::string kUIDNEXTStr;
    static const std::string kHIGHESTMODSEQStr;
    static const std::string kUNSEENStr;
    static const std::string kEXISTSStr;
    static const std::string kRECENTStr;
    static const std::string kDONEStr;
    static const std::string kContinuationStr;
    static const std::string kENVELOPEStr;
    static const std::string kBODYSTRUCTUREStr;
    static const std::string kBODYStr;
    static const std::string kRFC822Str;
    static const std::string kINTERNALDATEStr;
    static const std::string kRFC822HEADERStr;
    static const std::string kRFC822SIZEStr;
    static const std::string kRFC822TEXTStr;
    static const std::string kUIDStr;
      
    //
    // Decode function pointer
    //
    
    typedef std::function<BASERESPONSE  (CommandData& commandData, std::istringstream&)> DecodeFunction;
    
    // =====================
    // DISABLED CONSTRUCTORS
    // =====================

    // ===============
    // PRIVATE METHODS
    // ===============

    //
    // Send IDLE command (requires a special handler).
    //
    
    void sendCommandIDLE(void);
       
    //
    // Talks to server using curl_easy_send/recv()
    //

    void sendCommandDirect(const std::string& command);
    void waitForCommandResponse(const std::string& commandTag, std::string& commandResponse);

    //
    // Generate next command tag
    //
    
    void generateTag(void);
    
    //
    // Command response decode utility methods
    //
    
    static std::string contentsBetween(const std::string& line, const char first, const char last);
    static std::string extractBetweenDelimeter(const std::string& line, const char delimeter);
    static std::string extractTag(const std::string& line);
    static std::string extractCommand(const std::string& line);
    static std::string extractList(const std::string& line);
    
    static void decodeStatus(const std::string& tag, const std::string& line, BASERESPONSE resp);
    static void decodeOctets(const std::string& commandStr, FetchRespData& fetchData, std::string& line, std::istringstream& responseStream);
    static void decodeList(const std::string& commandStr, FetchRespData& fetchData, std::string& line);
    static void decodeString(const std::string& commandStr, FetchRespData& fetchData, std::string& line);
    static void decodeNumber(const std::string& commandStr, FetchRespData& fetchData, std::string& line);

    //
    // Command response decode methods
    //
    
    static BASERESPONSE decodeFETCH(CommandData& commandData, std::istringstream& responseStream);
    static BASERESPONSE decodeLIST(CommandData& commandData, std::istringstream& responseStream);
    static BASERESPONSE decodeSEARCH(CommandData& commandData, std::istringstream& responseStream);
    static BASERESPONSE decodeSELECT(CommandData& commandData, std::istringstream& responseStream);
    static BASERESPONSE decodeSTATUS(CommandData& commandData, std::istringstream& responseStream);
    static BASERESPONSE decodeEXPUNGE(CommandData& commandData, std::istringstream& responseStream);
    static BASERESPONSE decodeSTORE(CommandData& commandData, std::istringstream& responseStream);
    static BASERESPONSE decodeCAPABILITY(CommandData& commandData, std::istringstream& responseStream);
    static BASERESPONSE decodeDefault(CommandData& commandData, std::istringstream& responseStream);
    static BASERESPONSE decodeResponse(const std::string& commandLine, const std::string& commandResponse);

    // =================
    // PRIVATE VARIABLES
    // =================

    std::string userName = ""; // Email account user name
    std::string userPassword = ""; // Email account user name password
    std::string serverURL = ""; // IMAP server URL

    CURL *curl = nullptr; // curl handle
    CURLcode res = CURLE_OK; // curl status

    std::string commandResponse; // IMAP command response

    char rxBuffer[CURL_MAX_WRITE_SIZE]; // IMAP rx buffer
    char errMsgBuffer[CURL_ERROR_SIZE]; // IMAP error string buffer
    
    uint64_t tagCount=1;        // Current command tag count
    std::string currentTag;     // Current command tag
    
    //
    // Command to decode response function mapping table
    //
    
    static std::unordered_map<std::string, DecodeFunction> decodeCommmandMap;

};

#endif /* CMAILIMAP_HPP */

