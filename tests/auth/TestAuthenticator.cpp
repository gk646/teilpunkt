#define CATCH_CONFIG_MAIN // Include somewhere once

#include <catch_amalgamated.hpp>
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

    return;
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
        remove(token);
    }

    // Test for removal
    {
        AuthToken token{};
        auto status = login(userName, password, token);
        REQUIRE(status == AuthStatus::ERR_UNSUCCESSFUL);
    }
}

TEST_CASE("User Story Fuzzy")
{
}