#define CATCH_CONFIG_MAIN // Include somewhere once

#include <catch_amalgamated.hpp>
#include <thread>

#include "TestCommons.h"
#include "auth/AuthToken.h"

using namespace tpunkt;

static AuthStatus signup(const char* userName, const char* password)
{
    const UserName name{userName};

    Credentials credentials{};
    credentials.type = CredentialsType::PASSWORD;
    credentials.password = password;

    const auto status = GetAuthenticator().userAdd(name, credentials);

    Credentials emptyCredentials{};
    REQUIRE(credentials.password == emptyCredentials.password); // Zeroed out
    return status;
}

static AuthStatus login(const char* userName, const char* password, AuthToken& token)
{
    const UserName name{userName};

    Credentials credentials{};
    credentials.type = CredentialsType::PASSWORD;
    credentials.password = password;

    const auto status = GetAuthenticator().userLogin(name, credentials, token);

    Credentials emptyCredentials{};
    REQUIRE(credentials.password == emptyCredentials.password); // Zeroed out
    return status;
}

static AuthStatus changeCredentials(const AuthToken& token, const char* newUser, const char* newPw)
{
    const UserName name{newUser};

    Credentials credentials{};
    credentials.type = CredentialsType::PASSWORD;
    credentials.password = newPw;

    const auto status = GetAuthenticator().userChangeCredentials(token, name, credentials);

    Credentials emptyCredentials{};
    REQUIRE(credentials.password == emptyCredentials.password); // Zeroed out
    return status;
}

static AuthStatus remove(const AuthToken& token)
{
    return GetAuthenticator().userRemove(token);
}

TEST_CASE("Add, Login, and Delete User")
{
    TEST_INIT();

    // Setup
    const char* userName = "MyUserName";
    const char* password = "password123";

    // Signup
    {
        auto status = signup(userName, password);
        REQUIRE(status == AuthStatus::OK);
    }

    // Login
    {
        AuthToken token{};
        auto status = login(userName, password, token);
        REQUIRE(status == AuthStatus::OK);
    }

    // Remove
    {
        AuthToken token{};

        auto status = login(userName, password, token);
        REQUIRE(status == AuthStatus::OK);
        status = remove(token);
        REQUIRE(status == AuthStatus::OK);
    }

    // Test for removal
    {
        AuthToken token{};
        auto status = login(userName, password, token);
        REQUIRE(status == AuthStatus::ERR_UNSUCCESSFUL);
    }
}

TEST_CASE("Token copying")
{
    TEST_INIT();

    // Setup
    const char* userName = "MyUserName";
    const char* password = "password123";

    // Signup
    {
        auto status = signup(userName, password);
        REQUIRE(status == AuthStatus::OK);
    }

    AuthToken outerToken;
    // Login
    {
        AuthToken token{};
        auto status = login(userName, password, token);
        memcpy(&outerToken, &token, sizeof(AuthToken)); // Copy token forcefully
        REQUIRE(status == AuthStatus::OK);
    }
    // Invalid
    REQUIRE_FALSE(GetAuthenticator().tokenValid(outerToken));
    // Methods return invalid token
    REQUIRE(GetAuthenticator().userRemove(outerToken) == AuthStatus::ERR_INVALID_TOKEN);
}

TEST_CASE("Return value on invalid token")
{
    TEST_INIT();
    AuthToken token{};
    //
    Credentials c{};
    UserName n{};
    REQUIRE(GetAuthenticator().userRemove(token) == AuthStatus::ERR_INVALID_TOKEN);
    REQUIRE(GetAuthenticator().userChangeCredentials(token, n, c) == AuthStatus::ERR_INVALID_TOKEN);

    SessionMetaData data{};
    SecureWrapper<SessionID> id;
    REQUIRE(GetAuthenticator().sessionAdd(token, data, id) == AuthStatus::ERR_INVALID_TOKEN);
    REQUIRE(GetAuthenticator().sessionRemove(token) == AuthStatus::ERR_INVALID_TOKEN);
    REQUIRE(GetAuthenticator().sessionGet(token) == AuthStatus::ERR_INVALID_TOKEN);

    FileID handle;
    SecureWrapper<WrappedKey> key;
    REQUIRE(GetAuthenticator().getUserName(token, n) == AuthStatus::ERR_INVALID_TOKEN);
    REQUIRE(GetAuthenticator().getWrappedKey(token, handle, key) == AuthStatus::ERR_INVALID_TOKEN);
}


TEST_CASE("Change Credentials")
{
    TEST_INIT();
    // Setup
    const char* userName = "MyUserName";
    const char* newUser = "MyNewUserName";
    const char* password = "password123";
    const char* newPassword = "newpassword123";
    // Signup
    {
        auto status = signup(userName, password);
        REQUIRE(status == AuthStatus::OK);
    }

    // Login
    {
        AuthToken token{};
        auto status = login(userName, password, token);
        REQUIRE(status == AuthStatus::OK);
        status = changeCredentials(token, newUser, newPassword);
        REQUIRE(status == AuthStatus::OK);
    }

    {
        AuthToken token{};
        auto status = login(newUser, newPassword, token);
        REQUIRE(status == AuthStatus::OK);
    }
}