import {
    clearAuthError,
    clearError,
    displayAuthError,
    fetchWithErrorHandling,
    hashPassword,
    isPasskeyAvailable,
    redirectDashBoardLoggedIn,
    showError,
    validatePassword,
    validateUsername
} from './util.js';

document.addEventListener('DOMContentLoaded', () => {
    redirectDashBoardLoggedIn()

    // Cache DOM elements
    const form = document.querySelector('.login-form');
    const passkeyButton = document.getElementById('passkey-btn');
    const authError = document.getElementById('auth-error');
    const userNameInput = document.getElementById('username');
    const passwordInput = document.getElementById('password');
    const confirmPasswordInput = document.getElementById('confirm-password');

    // Initialize sodium once
    const sodiumReady = (async () => {
        await window.sodium.ready;
        return window.sodium;
    })();

    const validateConfirmPassword = (confirmElem) => {
        const passwordStr = passwordInput.value.trim();
        const confirmStr = confirmElem.value.trim();
        if (confirmStr !== passwordStr) {
            showError(confirmElem, 'Passwords do not match.');
            return false;
        }
        clearError(confirmElem);
        return true;
    };

    // Attach real-time validation
    userNameInput.addEventListener('input', () => {
        validateUsername(userNameInput);
    });

    passwordInput.addEventListener('input', () => {
        validatePassword(passwordInput);
        validateConfirmPassword(confirmPasswordInput);
    });

    confirmPasswordInput.addEventListener('input', () => {
        validateConfirmPassword(confirmPasswordInput);
    });

    async function registerUser(username, password) {
        await sodiumReady;
        let hashedPassword = hashPassword(password);
        const requestBody = JSON.stringify({
            name: username.trim(),
            password: hashedPassword,
        });

        const result = await fetchWithErrorHandling('/api/register/password', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Auth-Method': 'password'
            },
            body: requestBody
        });

        // Clear sensitive data
        hashedPassword = null;
        return result;
    }

    // Handle form submission for password signup
    form.addEventListener('submit', async (event) => {
        event.preventDefault();
        clearAuthError(authError);

        const inputs = [userNameInput, passwordInput, confirmPasswordInput];
        const validators = [
            validateUsername,
            validatePassword,
            validateConfirmPassword
        ];

        let isValid = true;
        // Validate all fields
        inputs.forEach((elem, idx) => {
            if (!elem.value.trim()) {
                showError(elem, 'This field is required.');
                isValid = false;
            } else {
                // For confirm password, pass the element explicitly.
                if (validators[idx] === validateConfirmPassword) {
                    if (!validateConfirmPassword(elem)) isValid = false;
                } else {
                    if (!validators[idx](elem)) isValid = false;
                }
            }
        });

        if (!isValid) {
            return;
        }

        let usernameValue = userNameInput.value.trim();
        let passwordValue = passwordInput.value.trim();

        try {
            let result = registerUser(usernameValue, passwordValue);
            userNameInput.value = '';
            passwordInput.value = '';
            confirmPasswordInput.value = '';
            await result;
            window.location.href = '/login';
        } catch (error) {
            displayAuthError(authError, "The sign-up has failed. Please check your details and try again.");
            console.error('Error during registration:', error);
        }
    });

    //TODO ADD DEBUG USER
    registerUser("hey", "123123123")

    if (isPasskeyAvailable()) {
        passkeyButton.addEventListener('click', async () => {
            clearAuthError(authError);

            if (!validateUsername(userNameInput)) {
                return;
            }

            const usernameValue = userNameInput.value.trim();
            await sodiumReady;

            try {

                // Generate a secure challenge (e.g. 32 bytes)
                const challenge = sodium.randombytes_buf(32);
                // Generate a random 16-byte user ID instead of using the username directly
                const userId = sodium.randombytes_buf(16);

                const publicKeyCredentialCreationOptions = {
                    challenge: Uint8Array.from(
                        "hello", c => c.charCodeAt(0)),
                    rp: {
                        name: "Duo Security",
                        id: "localhost"
                    },
                    user: {
                        id: Uint8Array.from(
                            "UZSL85T9AFC", c => c.charCodeAt(0)),
                        name: "lee@webauthn.guide",
                        displayName: "Lee",
                    },
                    pubKeyCredParams: [{alg: -8, type: "public-key"}],
                    authenticatorSelection: {
                        authenticatorAttachment: "cross-platform",
                        requireResidentKey: true,
                        residentKey: "preferred",
                        userVerification: "discouraged"
                    },
                    timeout: 15,
                    attestation: "indirect"
                };

                const credential = await navigator.credentials.create({
                    publicKey: publicKeyCredentialCreationOptions
                });

                console.log(credential)
                navigator.credentials.get({
                    publicKey: {
                        rpId: "localhost",
                        challenge: Uint8Array.from(
                            "hello", c => c.charCodeAt(0)), // The challenge must be produced by the server
                        allowCredentials: [
                            {
                                type: 'public-key',
                                id: Uint8Array.from(
                                    "UZSL85T9AFC", c => c.charCodeAt(0)) // The credential_id may be provided by the server
                            }
                        ],
                        timeout: 60000
                    }
                }).then(function (assertion) {
                    console.log(assertion)
                    // Send new credential info to server for verification and registration.
                }).catch(function (err) {
                    console.error(err)
                    // No acceptable authenticator or user refused consent. Handle appropriately.
                });

                // Note: You'll need to convert ArrayBuffers (like credential.response.attestationObject)
                // into a string (e.g. base64) for sending via JSON.

                //window.location.href = '/home';
            } catch (error) {
                displayAuthError(authError, "Passkey signup failed. Please try again.");
                console.error('Error during passkey signup:', error);
            }
        });
    } else {
        console.error("Passkey not supported")
        const passkeyButton = document.getElementById("passkey-btn");
        passkeyButton.disabled = true;
        passkeyButton.textContent = "Sign up with Passkey";
        passkeyButton.style.opacity = "0.5";
        passkeyButton.style.cursor = "not-allowed";
    }
});