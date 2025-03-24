// SPDX-License-Identifier: GPL-3.0-only

#define CATCH_CONFIG_MAIN // Include somewhere once

#include <catch_amalgamated.hpp>
#include <thread>
#include <server/DTO.h>
#include <sodium/crypto_generichash.h>
#include "TestCommons.h"

using namespace tpunkt;

static AuthStatus signup(const char* userName, const char* password)
{
    const UserName name{userName};

    Credentials credentials{};
    credentials.type = CredentialsType::PASSWORD;
    credentials.password = password;

    const auto status = GetAuthenticator().userAddServer(name, credentials);

    Credentials emptyCredentials{};
    REQUIRE(credentials.password == emptyCredentials.password); // Zeroed out
    return status;
}

static AuthStatus login(const char* userName, const char* password, UserID& user)
{
    const UserName name{userName};

    Credentials credentials{};
    credentials.type = CredentialsType::PASSWORD;
    credentials.password = password;

    const auto status = GetAuthenticator().userLogin(name, credentials, user);

    Credentials emptyCredentials{};
    REQUIRE(credentials.password == emptyCredentials.password); // Zeroed out
    return status;
}

static AuthStatus changeCredentials(const UserID& user, const char* newUser, const char* newPw)
{
    const UserName name{newUser};

    Credentials credentials{};
    credentials.type = CredentialsType::PASSWORD;
    credentials.password = newPw;

    const auto status = GetAuthenticator().userChangeCredentials(user, name, credentials);

    Credentials emptyCredentials{};
    REQUIRE(credentials.password == emptyCredentials.password); // Zeroed out
    return status;
}

static AuthStatus remove(const UserID& user)
{
    return GetAuthenticator().userRemove(user, user);
}

UserPassword getPassword(const char* hint)
{
    UserPassword pw = hint;
    crypto_generichash(pw.udata(), pw.capacity(), pw.u_str(), pw.capacity(), nullptr, 0);
    return pw;
}

TEST_CASE("Add, Login, and Delete User")
{
    TEST_INIT();

    // Setup
    const char* userName = "MyUserName";
    UserPassword pw = getPassword("password123");
    const char* password = pw.c_str();

    // Signup
    {
        auto status = signup(userName, password);
        REQUIRE(status == AuthStatus::OK);
    }

    // Login
    {
        UserID user{};
        auto status = login(userName, password, user);
        REQUIRE(status == AuthStatus::OK);
    }

    // Remove
    {
        UserID user{};

        auto status = login(userName, password, user);
        REQUIRE(status == AuthStatus::OK);
        status = remove(user);
        REQUIRE(status == AuthStatus::OK);
    }

    // Test for removal
    {
        UserID user{};
        auto status = login(userName, password, user);
        REQUIRE(status == AuthStatus::ERR_UNSUCCESSFUL);
    }
}

TEST_CASE("Change Credentials")
{
    TEST_INIT();
    // Setup
    const char* userName = "MyUserName";
    const char* newUser = "MyNewUserName";
    UserPassword pw = getPassword("password123");
    const char* password = pw.c_str();

    UserPassword newPw = getPassword("newpassword123");
    const char* newPassword = newPw.c_str();

    // Signup
    {
        auto status = signup(userName, password);
        REQUIRE(status == AuthStatus::OK);
    }

    // Login
    {
        UserID token{};
        auto status = login(userName, password, token);
        REQUIRE(status == AuthStatus::OK);
        status = changeCredentials(token, newUser, newPassword);
        REQUIRE(status == AuthStatus::OK);
    }

    {
        UserID token{};
        auto status = login(newUser, newPassword, token);
        REQUIRE(status == AuthStatus::OK);
    }
}

TEST_CASE("Error Codes")
{
    TEST_INIT();
    const char* wrongPassowrd = "password123";
    const UserPassword correctPW = getPassword("password123");

    {
        // forbidden symbol
        auto status = signup("Hello*", correctPW.c_str());
        REQUIRE(status == AuthStatus::ERR_INVALID_ARGUMENTS);

        // Too short
        status = signup("He", correctPW.c_str());
        REQUIRE(status == AuthStatus::ERR_INVALID_ARGUMENTS);
    }

    // Invalid password
    {
        auto status = signup("MyUser", wrongPassowrd);
        REQUIRE(status == AuthStatus::ERR_INVALID_ARGUMENTS);
    }
}

TEST_CASE("Sessions Add, Remove and Auth")
{
    TEST_INIT();
    const UserID user = UserID{1};
    SessionMetaData original{.userAgent = "FireFox", .remoteAddress = "Adress"};

    // Invalid user id
    {
        SecureWrapper<SessionToken> token;
        auto status = GetAuthenticator().sessionAdd(UserID{}, original, token);
        REQUIRE(status == AuthStatus::ERR_INVALID_ARGUMENTS);
    }

    SessionToken token;
    {
        SecureWrapper<SessionToken> wrapper;
        auto status = GetAuthenticator().sessionAdd(user, original, wrapper);
        REQUIRE(status == AuthStatus::OK);
        auto wrappedToken = wrapper.get();
        token = wrappedToken.get();
    }

    {
        std::vector<DTOSessionInfo> collector;
        auto status = GetAuthenticator().sessionGetInfo(user, collector);
        REQUIRE(status == AuthStatus::OK);
        REQUIRE(collector.size() == 1);
    }

    // Correct auth
    {
        UserID newUser{};
        auto status = GetAuthenticator().sessionAuth(token, original, newUser);
        REQUIRE(status == AuthStatus::OK);
        REQUIRE(newUser == user);
    }

    // Wrong metadata
    {
        UserID newUser{};
        SessionMetaData newMeta{"NotFireFox", "FakeAddress"};
        auto status = GetAuthenticator().sessionAuth(token, newMeta, newUser);
        REQUIRE(status == AuthStatus::ERR_UNSUCCESSFUL);

        // Automatically deletes session
        std::vector<DTOSessionInfo> collector;
        status = GetAuthenticator().sessionGetInfo(user, collector);
        REQUIRE(status == AuthStatus::OK);
        REQUIRE(collector.empty());
    }

    // Create new one
    {
        SecureWrapper<SessionToken> wrapper;
        auto status = GetAuthenticator().sessionAdd(user, original, wrapper);
        REQUIRE(status == AuthStatus::OK);
    }

    Timestamp creation;
    {
        std::vector<DTOSessionInfo> collector;
        auto status = GetAuthenticator().sessionGetInfo(user, collector);
        REQUIRE(status == AuthStatus::OK);
        REQUIRE(collector.size() == 1);
        creation.zero();
        creation.addNanos(collector[ 0 ].creationUnix);
    }

    // Wrong timestamp
    {
        auto status = GetAuthenticator().sessionRemove(user, Timestamp{});
        REQUIRE(status == AuthStatus::ERR_UNSUCCESSFUL);
    }

    {
        auto status = GetAuthenticator().sessionRemove(user, creation);
        REQUIRE(status == AuthStatus::OK);
    }
}