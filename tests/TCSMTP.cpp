#include "HOST.hpp"
/*
 * File:   TCSMTP.cpp
 * 
 * Author: Robert Tizzard
 *
 * Created on October 24, 2016, 2:34 PM
 * 
 * Description: Google unit tests for class CSMTP.
 *
 * Copyright 2016.
 *
 */

// =============
// INCLUDE FILES
// =============

// Google test

#include "gtest/gtest.h"

// C++ STL

#include <stdexcept>

// CSMTP class

#include "CSMTP.hpp" 

using namespace Antik::SMTP;

// =======================
// UNIT TEST FIXTURE CLASS
// =======================

class TCSMTP : public ::testing::Test {
protected:

    // Empty constructor

    TCSMTP() {
    }

    // Empty destructor

    ~TCSMTP() override {
    }

    void SetUp() override {
    }

    void TearDown() override {
    }

    CSMTP smtp {};

};

// =================
// FIXTURE CONSTANTS
// =================

// ===============
// FIXTURE METHODS
// ===============

// =====================
// TASK CLASS UNIT TESTS
// =====================

TEST_F(TCSMTP, SetServerURL) {

    std::string serverURL;
    smtp.setServer("smtp://smtp.gmail.com:25");
    ASSERT_STREQ("smtp://smtp.gmail.com:25", smtp.getServer().c_str());

}

TEST_F(TCSMTP, SetUser) {

    smtp.setUserAndPassword("user01", "password01");
    ASSERT_STREQ("user01", smtp.getUser().c_str());

}

TEST_F(TCSMTP, SetFromAddress) {

    smtp.setFromAddress("<user01@gmail.com>");
    ASSERT_STREQ("<user01@gmail.com>", smtp.getFromAddress().c_str());

}

TEST_F(TCSMTP, SetToAddress) {

    smtp.setToAddress("<user02@gmail.com>");
    ASSERT_STREQ("<user02@gmail.com>", smtp.getToAddress().c_str());

}

TEST_F(TCSMTP, SetCCAddress) {

    smtp.setCCAddress("<user03@gmail.com>,<user04@gmail.com>,<user05@gmail.com>,<user06@gmail.com>");
    ASSERT_STREQ("<user03@gmail.com>,<user04@gmail.com>,<user05@gmail.com>,<user06@gmail.com>", smtp.getCCAddress().c_str());

}

TEST_F(TCSMTP, SetMailSubject) {

    smtp.setMailSubject("Message From The Grave");
    ASSERT_STREQ("Message From The Grave", smtp.getMailSubject().c_str());

}

TEST_F(TCSMTP, SetMailMessage) {

    smtp.setMailMessage({"Man is distinguished, not only by his reason, but by this singular passion from ",
        "other animals, which is a lust of the mind, that by a perseverance of delight ",
        "in the continued and indefatigable generation of knowledge, exceeds the short ",
        "vehemence of any carnal pleasure."});

    ASSERT_STREQ("Man is distinguished, not only by his reason, but by this singular passion from other animals,"
            " which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable"
            " generation of knowledge, exceeds the short vehemence of any carnal pleasure.", smtp.getMailMessage().c_str());

}

TEST_F(TCSMTP, Base64EncodeDecode) {

    std::string deocdedString;
    std::string encodedString;
    std::string redecodedString;

    deocdedString = "a";
    CSMTP::encodeToBase64(deocdedString, encodedString, deocdedString.length());
    ASSERT_STREQ("YQ==", encodedString.c_str());

    deocdedString = "ab";
    CSMTP::encodeToBase64(deocdedString, encodedString, deocdedString.length());
    ASSERT_STREQ("YWI=", encodedString.c_str());

    deocdedString = "abc";
    CSMTP::encodeToBase64(deocdedString, encodedString, deocdedString.length());
    ASSERT_STREQ("YWJj", encodedString.c_str());

    deocdedString = "abcd";
    CSMTP::encodeToBase64(deocdedString, encodedString, deocdedString.length());
    ASSERT_STREQ("YWJjZA==", encodedString.c_str());

    deocdedString = "a";
    CSMTP::encodeToBase64(deocdedString, encodedString, deocdedString.length());
    CSMTP::decodeFromBase64(encodedString, redecodedString, encodedString.length());
    ASSERT_STREQ(deocdedString.c_str(), redecodedString.c_str());

    deocdedString = "ab";
    CSMTP::encodeToBase64(deocdedString, encodedString, deocdedString.length());
    CSMTP::decodeFromBase64(encodedString, redecodedString, encodedString.length());
    ASSERT_STREQ(deocdedString.c_str(), redecodedString.c_str());

    deocdedString = "abc";
    CSMTP::encodeToBase64(deocdedString, encodedString, deocdedString.length());
    CSMTP::decodeFromBase64(encodedString, redecodedString, encodedString.length());
    ASSERT_STREQ(deocdedString.c_str(), redecodedString.c_str());

    deocdedString = "abcd";
    CSMTP::encodeToBase64(deocdedString, encodedString, deocdedString.length());
    CSMTP::decodeFromBase64(encodedString, redecodedString, encodedString.length());
    ASSERT_STREQ(deocdedString.c_str(), redecodedString.c_str());

    deocdedString = "Man is distinguished, not only by his reason, but by this singular passion from other animals,"
            " which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable"
            " generation of knowledge, exceeds the short vehemence of any carnal pleasure.";

    CSMTP::encodeToBase64(deocdedString, encodedString, deocdedString.length());
    CSMTP::decodeFromBase64(encodedString, redecodedString, encodedString.length());
    ASSERT_STREQ(deocdedString.c_str(), redecodedString.c_str());


}

TEST_F(TCSMTP, CheckForNulls) {

    CSMTP smtp;
    std::string mailMessage;

    smtp.setServer("smtp://smtp.gmail.com:25");
    smtp.setUserAndPassword("user01@gmail.com", "user001password");

    smtp.setFromAddress("<user01@gmail.com>");
    smtp.setToAddress("<usesr02@hotmail.com>");
    smtp.setCCAddress("<users03@gmail.com>");

    smtp.setMailSubject("Message From The Grave");

    smtp.setMailMessage({"Man is distinguished, not only by his reason, but by this singular passion from",
        "other animals, which is a lust of the mind, that by a perseverance of delight",
        "in the continued and indefatigable generation of knowledge, exceeds the short",
        "vehemence of any carnal pleasure."});

    mailMessage = smtp.getMailFull();

    EXPECT_TRUE(mailMessage.find('\0') == std::string::npos);

}

// =====================
// RUN GOOGLE UNIT TESTS
// =====================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}