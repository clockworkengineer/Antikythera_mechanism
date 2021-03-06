//
// Class: CSSHSession
//
// Description: A class for connecting to a SSH server, verifying the server and authorizing
// the client and managing the sessions created. If a client wishes to override one of the
// three main client authorization methods then it can be used as a base class. It is very much
// a wrapper class for libssh session functionality but it also wraps the main data structures
// in unique pointers with there own custom deleters. It tries to hide as much of its
// implementation using libssh as possible and use/return C20++ data structure/exceptions.
// It is not complete by any means but may be updated to future to use more libssh features.
//
// Note: The libssh documentation says that sessions may be reused after they are disconnected;
// I have not found this to be the case and so after a disconnect sessions need to be destroyed
// and recreated (the same seems to apply to channels).
//
// Dependencies:
//
// C20++        - Language standard features used.
// libssh       - Used to talk to SSH server (https://www.libssh.org/) (0.7.5)
//
// =================
// CLASS DEFINITIONS
// =================
#include "CSSHSession.hpp"
// ====================
// CLASS IMPLEMENTATION
// ====================
//
// C++ STL
//
#include <cstring>
// =========
// NAMESPACE
// =========
namespace Antik::SSH
{
    // ===========================
    // PRIVATE TYPES AND CONSTANTS
    // ===========================
    // ==========================
    // PUBLIC TYPES AND CONSTANTS
    // ==========================
    // ========================
    // PRIVATE STATIC VARIABLES
    // ========================
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    //
    // Intialise libssh for threading (currently pthread only).
    //
    void CSSHSession::initialise()
    {
        static bool intialised{false};
        if (!intialised)
        {
            ssh_threads_set_callbacks(ssh_threads_get_pthread());
            ssh_init();
            intialised = true;
        }
    }
    // ==============
    // PUBLIC METHODS
    // ==============
    //
    // Main CSSHSession object constructor.
    //
    CSSHSession::CSSHSession()
    {
        initialise();
        if ((m_session = ssh_new()) == NULL)
        {
            throw Exception("Could not allocate new session.", __func__);
        }
    }
    //
    // Main CSSHSession object constructor.
    //
    CSSHSession::CSSHSession(CSSHSession &session) : CSSHSession()
    {
        copyOptions(session);
    }
    //
    // CSSHSession Destructor (free session resources).
    //
    CSSHSession::~CSSHSession()
    {
        if (m_session)
        {
            ssh_free(m_session);
            m_session = NULL;
        }
    }
    //
    // Set SSH server name.
    //
    void CSSHSession::setServer(const std::string &server)
    {
        m_server = server;
        setOption(SSH_OPTIONS_HOST, m_server.c_str());
    }
    //
    // Set SSH server port.
    //
    void CSSHSession::setPort(unsigned int port)
    {
        m_port = port;
        setOption(SSH_OPTIONS_PORT, &m_port);
    }
    //
    // Set user.
    //
    void CSSHSession::setUser(const std::string &user)
    {
        m_user = user;
        setOption(SSH_OPTIONS_USER, m_user.c_str());
    }
    //
    // Set user password.
    //
    void CSSHSession::setUserPassword(const std::string &password)
    {
        m_password = password;
    }
    //
    // Connect to SSH server.
    //
    void CSSHSession::connect()
    {
        if (ssh_connect(m_session) != SSH_OK)
        {
            throw Exception(*this, __func__);
        }
    }
    //
    // Disconnect SSH session.
    //
    void CSSHSession::disconnect(bool silent)
    {
        if (m_session)
        {
            if (silent)
            {
                ssh_silent_disconnect(m_session);
            }
            else
            {
                ssh_disconnect(m_session);
            }
        }
        m_authorized = false;
    }
    //
    // Get available server user(client) authorization options.
    //
    int CSSHSession::userAuthorizationList()
    {
        return (ssh_userauth_list(m_session, NULL));
    }
    //
    // No user authorization.
    //
    int CSSHSession::userAuthorizationNone()
    {
        int returnCode = ssh_userauth_none(m_session, NULL);
        if (returnCode == SSH_AUTH_ERROR)
        {
            throw Exception(*this, __func__);
        }
        m_authorized = true;
        this->m_authorizarionType = UserAuthorizationType::None;
        return (returnCode);
    }
    //
    // Password user authorization. (Virtual: May be overridden)
    //
    int CSSHSession::userAuthorizationWithPassword()
    {
        int returnCode = ssh_userauth_password(m_session, NULL, m_password.c_str());
        if (returnCode == SSH_AUTH_ERROR)
        {
            throw Exception(*this, __func__);
        }
        m_authorized = true;
        this->m_authorizarionType = UserAuthorizationType::Password;
        return (returnCode);
    }
    //
    // Automatic public key user authorization.
    //
    int CSSHSession::userAuthorizationWithPublicKeyAuto()
    {
        int returnCode = ssh_userauth_publickey_auto(m_session, NULL, NULL);
        if (returnCode == SSH_AUTH_ERROR)
        {
            throw Exception(*this, __func__);
        }
        m_authorized = true;
        this->m_authorizarionType = UserAuthorizationType::PublicKey;
        return (returnCode);
    }
    //
    // Public key user authorization. (Virtual: May be overridden)
    //
    int CSSHSession::userAuthorizationWithPublicKey()
    {
        int returnCode = userAuthorizationWithPublicKeyAuto();
        if (returnCode == SSH_AUTH_ERROR)
        {
            throw Exception(*this, __func__);
        }
        m_authorized = true;
        this->m_authorizarionType = UserAuthorizationType::PublicKey;
        return (returnCode);
    }
    //
    // Keyboard interactive  user authorization. (Virtual: May be overridden)
    //
    int CSSHSession::userAuthorizationWithKeyboardInteractive()
    {
        return (SSH_AUTH_DENIED);
    }
    //
    // Check whether the server is known.
    //
    int CSSHSession::isServerKnown()
    {
        return (ssh_session_is_known_server(m_session) == SSH_KNOWN_HOSTS_OK);
    }
    //
    // Return sessions server public key.
    //
    Antik::SSH::CSSHSession::Key CSSHSession::getPublicKey()
    {
        Key serverPublicKey;
        ssh_key key;
        if (ssh_get_server_publickey(m_session, &key) != SSH_OK)
        {
            return (serverPublicKey);
        }
        serverPublicKey.reset(key);
        return (serverPublicKey);
    }
    //
    // Generate hash for passed in server public key/
    //
    void CSSHSession::getPublicKeyHash(Antik::SSH::CSSHSession::Key &serverPublicKey, std::vector<unsigned char> &keyHash)
    {
        unsigned char *hash = NULL;
        size_t hlen;
        if (ssh_get_publickey_hash(serverPublicKey.get(), SSH_PUBLICKEY_HASH_SHA1, &hash, &hlen) != SSH_OK)
        {
            throw Exception(*this, __func__);
        }
        keyHash.assign(&hash[0], &hash[hlen]);
        ssh_clean_pubkey_hash(&hash);
    }
    //
    // Convert hash value to vector of hex characters.
    //
    std::string CSSHSession::convertKeyHashToHex(std::vector<unsigned char> &keyHash)
    {
        char *hexa = NULL;
        std::string convertedHash;
        hexa = ssh_get_hexa(&keyHash[0], keyHash.size());
        if (hexa != NULL)
        {
            convertedHash.assign(&hexa[0], &hexa[std::strlen(hexa)]);
            ssh_string_free_char(hexa);
        }
        return (convertedHash);
    }
    //
    // Write current server key/hash away to local configuration file for later sessions.
    //
    void CSSHSession::writeKnownHost()
    {
        if (ssh_session_update_known_hosts(m_session) != SSH_OK)
        {
            throw Exception(*this, __func__);
        }
    }
    //
    // Get server issue banner.
    //
    std::string CSSHSession::getBanner() const
    {
        std::string sessionBanner;
        char *banner = ssh_get_issue_banner(m_session);
        if (banner)
        {
            sessionBanner.assign(&banner[0], &banner[std::strlen(banner)]);
            ssh_string_free_char(banner);
        }
        return (sessionBanner);
    }
    //
    // Get client banner.
    //
    std::string CSSHSession::getClientBanner() const
    {
        std::string clientBanner;
        const char *banner = ssh_get_clientbanner(m_session);
        if (banner)
        {
            clientBanner.assign(&banner[0], &banner[std::strlen(banner)]);
        }
        return (clientBanner);
    }
    //
    // Get server banner.
    //
    std::string CSSHSession::getServerBanner() const
    {
        std::string serverBanner;
        const char *banner = ssh_get_serverbanner(m_session);
        if (banner)
        {
            serverBanner.assign(&banner[0], &banner[std::strlen(banner)]);
        }
        return (serverBanner);
    }
    //
    // Get server disconnect message.
    //
    std::string CSSHSession::getDisconnectMessage() const
    {
        std::string disconnectMessage;
        const char *message = ssh_get_disconnect_message(m_session);
        if (message)
        {
            disconnectMessage.assign(&message[0], &message[std::strlen(message)]);
        }
        else
        {
            disconnectMessage = getError();
        }
        return (disconnectMessage);
    }
    //
    // Return string representing name of current session input cipher.
    //
    std::string CSSHSession::getCipherIn()
    {
        std::string cipherIn;
        const char *cipher = ssh_get_cipher_in(m_session);
        if (cipher == NULL)
        {
            throw Exception(*this, __func__);
        }
        cipherIn.assign(&cipher[0], &cipher[std::strlen(cipher)]);
        return (cipherIn);
    }
    //
    // Return string representing name of current session output cipher.
    //
    std::string CSSHSession::getCipherOut()
    {
        std::string cipherOut;
        const char *cipher = ssh_get_cipher_in(m_session);
        if (cipher == NULL)
        {
            throw Exception(*this, __func__);
        }
        cipherOut.assign(&cipher[0], &cipher[std::strlen(cipher)]);
        return (cipherOut);
    }
    //
    // Return string representing name of current session input HMAC algorithm.
    //
    std::string CSSHSession::getHMACIn()
    {
        std::string hmacIn;
        const char *hmac = ssh_get_hmac_in(m_session);
        if (hmac == NULL)
        {
            throw Exception(*this, __func__);
        }
        hmacIn.assign(&hmac[0], &hmac[std::strlen(hmac)]);
        return (hmacIn);
    }
    //
    // Return string representing name of current session output HMAC algorithm.
    //
    std::string CSSHSession::getHMACOut()
    {
        std::string hmacOut;
        const char *hmac = ssh_get_hmac_out(m_session);
        if (hmac == NULL)
        {
            throw Exception(*this, __func__);
        }
        hmacOut.assign(&hmac[0], &hmac[std::strlen(hmac)]);
        return (hmacOut);
    }
    //
    // Return string representing name of current session key exchange algorithm.
    //
    std::string CSSHSession::getKeyExchangeAlgorithm()
    {
        std::string keyExchangeAlg;
        const char *keyexchange = ssh_get_kex_algo(m_session);
        if (keyexchange == NULL)
        {
            throw Exception(*this, __func__);
        }
        keyExchangeAlg.assign(&keyexchange[0], &keyexchange[std::strlen(keyexchange)]);
        return (keyExchangeAlg);
    }
    //
    // Set session option value
    //
    void CSSHSession::setOption(CSSHSession::Option sessionOption, const void *optionValue)
    {
        if (ssh_options_set(m_session, sessionOption, optionValue) != SSH_OK)
        {
            throw Exception(*this, __func__);
        }
    }
    //
    // Copy a sessions options to a destination session
    //
    void CSSHSession::copyOptions(CSSHSession &source)
    {
        if (ssh_options_copy(source.getSession(), &m_session) != SSH_OK)
        {
            throw Exception(*this, __func__);
        }
    }
    //
    // Get session option value
    //
    void CSSHSession::getOption(CSSHSession::Option sessionOption, std::string &optionValue)
    {
        char *value;
        if (ssh_options_get(m_session, sessionOption, &value) != SSH_OK)
        {
            throw Exception(*this, __func__);
        }
        optionValue.assign(&value[0], &value[std::strlen(value)]);
        ssh_string_free_char(value);
    }
    //
    // Get SSH version.
    //
    int CSSHSession::getSSHVersion() const
    {
        return (ssh_get_version(m_session));
    }
    //
    // Get OpenSSH version.
    //
    int CSSHSession::getOpenSSHVersion() const
    {
        return (ssh_get_openssh_version(m_session));
    }
    //
    // Return current status of session.
    //
    int CSSHSession::getStatus() const
    {
        return (ssh_get_status(m_session));
    }
    //
    // Return true if session connected.
    //
    bool CSSHSession::isConnected() const
    {
        return (ssh_is_connected(m_session));
    }
    //
    // Has session been authorised
    //
    bool CSSHSession::isAuthorized() const
    {
        return m_authorized;
    }
    //
    // Return last SSH error message.
    //
    std::string CSSHSession::getError() const
    {
        return (ssh_get_error(m_session));
    }
    //
    // Return last SSH error code.
    //
    int CSSHSession::getErrorCode() const
    {
        return (ssh_get_error_code(m_session));
    }
    //
    // Return internal libssh session reference,
    //
    ssh_session CSSHSession::getSession() const
    {
        return m_session;
    }
    //
    // Set libssh logging verbosity
    //
    void CSSHSession::setLogging(int logging)
    {
        m_logging = logging;
        ssh_options_set(m_session, SSH_OPTIONS_LOG_VERBOSITY, &m_logging);
    }
    std::uint32_t CSSHSession::getAuthorizarionType() const
    {
        return m_authorizarionType;
    }
} // namespace Antik::SSH
