## Antikythera Mechanism C ++ Class Repository ##

# Introduction #

This repository contains the master copies of the C++ based utility classes that I use in my projects.  Copies of these classes in other repositories while working will not usually be up to date. The classes currently come wrapped in a namespace called Antik though this may change in future.

# [CTask](https://github.com/clockworkengineer/Antikythera_mechanism/blob/master/classes/CTask.cpp) #

The core for the file processing engine is provided by the CTask class whose constructor takes five arguments, 

- **taskName:** The task name (std::string).
- **watchFolder:** The folder to be watched (std::string).
- **taskActFcn:** A pointer to a task action function that is called for each file that is copied/moved into the watch folder hierarchy.
- **fnData:** A pointer to data that may be needed by the action function.
- **watchDepth:** An integer specifying the watch depth (-1=all,0=just watch folder,1=next level down etc.)
- **options:**(optional) This structure passes in values used in any low level functionality (ie. killCount) and can be implementation specific such as providing pointers to the generic coutsr/coutstr trace functions.

To start watching/processing files call this classes monitor function; the code within FPE.cpp creates a separate thread for this but it can be run in the main programs thread by just calling task.monitor() without any thread creation wrappper code (--single  option).

At the center of the class is an event loop which waits for CApprise events and calls the passed task action function to process any file name passed through as part of an add event. The CApprise class is new and is basically an encapsulation of all of the inotify file event handling  code that used to reside in the Task class with an abstraction layer to hide any platform specifics. In the future new platform specific versions of this class may be implemented for say MacOS or Windows; in creating this class the one last non portable component of the FPE has been isolated thus aiding porting in future. As a result of this new class the task class is a lot simpler and smaller with all of the functionality being offloaded. The idea of making CTask a child of the CApprise base class has been thought about but for present a task just creates and uses an private CApprise watcher object.

It should be noted that a basic shutdown protocol is provided to close down any threads that the task class uses by calling task.stop(). This now in turn calls the CApprise objects stop method which stops its internal event reading loop and performs any closedown of the CApprise object and thread.  This is just to give some control over thread termination which the C++ STL doesn't really provide; well in a subtle manner anyways. The shutdown can be actuated as well by either deleting the watch folder or by specifying a kill count in the optional task options structure parameter that can be passed in the classes constructor.

The task options structure parameter also has two other members which are pointers to functions that handle all cout/cerr output from the class. These take as a parameter a vector of strings to output and if the option parameter is omitted or the pointers are nullptr then no output occurs. The FPE provides these two functions in the form of coutstr/coutstr which are passed in if --quiet is not specified nullptrs otherwise. All output is modeled this way was it enables the two functions in the FPE to use a mutex to control access to the output streams which are not thread safe and also to provide a --quiet mode and when it is implemented a output to log file option.

# [CApprise](https://github.com/clockworkengineer/Antikythera_mechanism/blob/master/classes/CApprise.cpp) #

This is class was created to be a standalone class / abstraction of the inotify file event handling code that used to be contained in CTask. 

Its constructor has 3 parameters:

- **watchFolder:** Folder to watch for files created or moved into.
- **watchDepth:**  The watch depth is how far down the directory hierarchy that will be watched (-1 the whole tree, 0 just the watcher folder, 1 the next level down etc).
- **options:**(optional) This structure passes in values used in any low level functionality and can be implementation specific such as pointers to the generic coutsr/coutstr trace functions.

Once the object is created then its core method CApprise::watch() is run on a separate thread that is used to generate events from actions on files using inotify. While this is happening the main application loops  waiting on events returned by method CApprise::getEvent().

The current supported event types being

    enum EventId { 
    	Event_none=0,       // None
    	Event_add,          // File added to watched folder hierachy
    	Event_change,       // File changed
    	Event_unlink,       // File deleted from watched folder hierachy
    	Event_addir,        // Directory added to watched folder hierachy
    	Event_unlinkdir,    // Directory deleted from watched folder hierachy
    	Event_error         // Exception error
    };

and they are contained within a structure of form

    struct Event {
    	EventId id;             // Event id
    	std::string message;    // Event file name / error message string
    };
    
Notes: 

- Events *addir*/unlinkdir will result in new watch folders being added/removed from the internal watch table maps (depending on the value of watchDepth).

# *Exceptions* #

Both the CTask and CApprise classes are designed to run in a separate thread although the former can run in the main thread quite happily. As such any exceptions thrown by them could be lost and so that they are not a copy is taken inside each objects main catch clause and stored away in a std::exception_ptr. This value can then by retrieved with method getThrownException() and either re-thrown if the end of the chain has been reached or stored away again to retrieved be another getThrownException() when the enclosing object closes down (as in the case CTask and CApprise class having thrown the exception).

# [CRedirect](https://github.com/clockworkengineer/Antikythera_mechanism/blob/master/classes/CRedirect.cpp) #

This is a small self contained utility class designed for FPE logging output. Its prime functionality is to provide a wrapper for pretty generic code that saves away an output streams read buffer, creates a file stream and redirects the output stream to it. The code to restore the original output streams is called from the objects destructor thus providing convenient for restoring the original stream. Its primary use within the FPE is to redirect std::cout to a log file.
 
# [CSMTP](https://github.com/clockworkengineer/Antikythera_mechanism/blob/master/classes/CSMTP.cpp) #

CSMTP provides the ability to create an email, add file attachments (encoded either as 7-bit or base64) and then send the created email to a given recipient(s). It provides methods for setting various parameters required to send the email, attach files and post the resulting email. It is state based so it is quite possible to create an email and send but then just change say the recipients and re-post. Library [libcurl](https://curl.haxx.se/libcurl/) is used to provide the SMTP server connect and message sending transport.

# [CIMAP](https://github.com/clockworkengineer/Antikythera_mechanism/blob/master/classes/CIMAP.cpp) #

CIMAP provides a way to connect to an IMAP server, send commands and receive responses. It supports most of [rfc3501](https://tools.ietf.org/html/rfc3501) IMAP standard including the ability the ability to FETCH messages and also APPEND them to a mailbox. Antik class CSocket is used to provide the IMAP server connect and command send / receive transport. The string returned has responses containing status and other such possible values can either be parsed by third party code or by use of the class CIMAPParse.

# [CIMAPParse](https://github.com/clockworkengineer/Antikythera_mechanism/blob/master/classes/CIMAPParse.cpp) #

CIMAPParse is used to take any responses returned from IMAP commands, parse them and a return 
pointer to a suitable structure representation of the response. This structure includes a return status and also an error message field for when an error occurs.

# [CIMAPBodyStruct](https://github.com/clockworkengineer/Antikythera_mechanism/blob/master/classes/CIMAPBodyStruct.cpp) #

CIMAPBodyStruct is used to parse any bodystructures returned by CIMAPParse and convert them into a tree structure that may then be traversed by a user supplied function for each body part found. This may be used to extract information or perform searches within the tree for say the finding of any file attachments; this example is provided as an built in for the class.

# [CMIME](https://github.com/clockworkengineer/Antikythera_mechanism/blob/master/classes/CMIME.cpp) #

CMIME contains any MIME processing functionality/utilities used on projects. It is still quite small with just a file extension to MIME type mapping function, a parser for MIME word encoded strings and a function to convert such a string to a best possible 7-bit ASCII mapping.

# [CZIP](https://github.com/clockworkengineer/Antikythera_mechanism/blob/master/classes/CZIP.cpp) #

CFIleZIP is a class that enables the creation and manipulation of ZIP file archives. It supports 2.0 compatible archives at present; either storing or retrieving files in deflate compressed format or a simple stored copy of a file (ZIP64 extesions are also supported for larger format archives). The current supported compression format inflate/deflate  functionality is provided through the use of library [zlib](http://www.zlib.net/).

# [CZIPIO](https://github.com/clockworkengineer/Antikythera_mechanism/blob/master/classes/CZIPIO.cpp) #

CZIPIO provides functionality to open an ZIP archive and read/write its records and raw data. It
is the base class of CZIP but may be used standalone as with example program ZIPArchiveInfo.

# [CLogger](https://github.com/clockworkengineer/Antikythera_mechanism/blob/master/classes/CLogger.cpp) #

Generic log trace class that will take a list of strings and output them either to cout or cerr with an optional time and date stamp. It also includes a template method for converting an arbitrary value to a string to be placed in the list of strings to be output. This class is very much a work in progress and will probably change until I find a solution that I like for my logging needs.

#  [CSocket](https://github.com/clockworkengineer/Antikythera_mechanism/blob/master/classes/CSocket.cpp) #

Class for connecting to / listening for connections from remote peers and the reading/writing of data using sockets. It supports both plain and TLS/SSL connections and  is implemented using [BOOST:ASIO](http://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio.html) synchronous API calls. At present it only has basic TLS/SSL support and is geared more towards client support but this may change in future.

#  [CFTP](https://github.com/clockworkengineer/Antikythera_mechanism/blob/master/classes/CFTP.cpp) #

A class to connect to an FTP server using provided credentials and enable the uploading/downloading of files along with assorted other commands. It uses CSocket to provide the connection to the FTP server and may be plain or TLS/SSL.

# To do list #

1. Increase list of example programs.
2. CIMAPEnvelope to parse envelope response ( need to find a use for this before i start).
4. Extend existing unit tests and add more for classes which there are none.