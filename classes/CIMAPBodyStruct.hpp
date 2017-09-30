/*
 * File:   CIMAPBodyStruct.hpp
 * 
 * Author: Robert Tizzard
 * 
 * Created on January 24, 2017, 2:33 PM
 *
 * Copyright 2016.
 *
 */

#ifndef CIMAPBODYSTRUCT_HPP
#define CIMAPBODYSTRUCT_HPP

//
// C++ STL
//

#include <vector>
#include <memory>
#include <unordered_map>
#include <stdexcept>

// =========
// NAMESPACE
// =========

namespace Antik {
    namespace IMAP {

        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================

        //
        // BODYSTRUCTURE constants
        //

        constexpr const char *kNILStr{ "NIL"};
        constexpr const char *kTEXTStr{ "TEXT"};
        constexpr const char *kATTACHMENTStr{ "ATTACHMENT"};
        constexpr const char *kINLINEStr{ "INLINE"};
        constexpr const char *kCREATIONDATEStr{ "CREATION-DATE"};
        constexpr const char *kFILENAMEStr{ "FILENAME"};
        constexpr const char *kMODIFICATIONDATEStr{ "MODIFICATION-DATE"};
        constexpr const char *kSIZEStr{ "SIZE"};

        // ================
        // CLASS DEFINITION
        // ================

        class CIMAPBodyStruct {
        public:

            // ==========================
            // PUBLIC TYPES AND CONSTANTS
            // ==========================

            //
            // Class exception
            //

            struct Exception : public std::runtime_error {

                Exception(std::string const& messageStr)
                : std::runtime_error("CIMAPBodyStruct Failure: " + messageStr) {
                }

            };

            //
            // Parsed body part contents
            //

            struct BodyPartParsed {
                std::string typeStr; // Body type
                std::string subtypeStr; // Body subtype
                std::string parameterListStr; // Body parameter list
                std::string idStr; // Body id
                std::string descriptionStr; // Body Description
                std::string encodingStr; // Body encoding
                std::string sizeStr; // Body size
                std::string textLinesStr; // Body ("TEXT") extended no of text lines
                std::string md5Str; // Body MD5 value
                std::string dispositionStr; // Body disposition list
                std::string languageStr; // Body language
                std::string locationStr; // Body location
                std::string extendedStr; // Body extended data (should be empty)

            };

            //
            // Body structure tree
            //

            struct BodyPart;

            struct BodyNode {
                std::string partLevelStr; // Body part level
                std::vector<BodyPart> bodyParts; // Vector of body parts and child nodes
                std::string extendedStr; // Multi-part extended data for level
            };

            struct BodyPart {
                std::string partNoStr; // Body part no (ie. 1 or 1.2..)                 
                std::string partStr; // Body part contents
                std::unique_ptr<BodyPartParsed> parsedPart; // Parsed body part data
                std::unique_ptr<BodyNode> child; // Pointer to lower level node in tree
            };

            //
            // Body attachment details
            //

            struct Attachment {
                std::string indexStr;
                std::string partNoStr;
                std::string creationDateStr;
                std::string fileNameStr;
                std::string modifiactionDateStr;
                std::string sizeStr;
                std::string encodingStr;
            };

            struct AttachmentData {
                std::vector<Attachment> attachmentsList;
            };


            typedef std::function<void (std::unique_ptr<BodyNode>&, BodyPart&, std::shared_ptr<void>&) > BodyPartFn;

            // ============
            // CONSTRUCTORS
            // ============

            // ==========
            // DESTRUCTOR
            // ==========

            // ==============
            // PUBLIC METHODS
            // ==============

            //
            // Construct body structure tree
            //

            static void consructBodyStructTree(std::unique_ptr<BodyNode>& bodyNode, const std::string& bodyPartStr);

            //
            // Walk body structure tree calling use supplied function for each body part.
            //

            static void walkBodyStructTree(std::unique_ptr<BodyNode>& bodyNode, BodyPartFn walkFn, std::shared_ptr<void>& walkData);

            //
            // Walk function to extract file attachments.
            //

            static void attachmentFn(std::unique_ptr<BodyNode>& bodyNode, BodyPart& bodyPart, std::shared_ptr<void>& attachmentData);

            // ================
            // PUBLIC VARIABLES
            // ================

        private:

            // ===========================
            // PRIVATE TYPES AND CONSTANTS
            // ===========================


            // ===========================================
            // DISABLED CONSTRUCTORS/DESTRUCTORS/OPERATORS
            // ===========================================

            CIMAPBodyStruct() = delete;
            virtual ~CIMAPBodyStruct() = delete;
            CIMAPBodyStruct(const CIMAPBodyStruct & orig) = delete;
            CIMAPBodyStruct(const CIMAPBodyStruct && orig) = delete;
            CIMAPBodyStruct& operator=(CIMAPBodyStruct other) = delete;

            // ===============
            // PRIVATE METHODS
            // ===============

            //
            // Parse body structure tree filling in body part data
            //

            static void parseNext(std::string& partStr, std::string& valueStr);
            static void parseBodyPart(BodyPart& bodyPart);
            static void parseBodyStructTree(std::unique_ptr<BodyNode>& bodyNode);

            //
            // Create body structure tree from body part list
            //

            static void createBodyStructTree(std::unique_ptr<BodyNode>& bodyNode, const std::string& bodyPartStr);

            // =================
            // PRIVATE VARIABLES
            // =================

        };

    } // namespace IMAP
} // namespace Antik

#endif /* CIMAPBODYSTRUCT_HPP */
