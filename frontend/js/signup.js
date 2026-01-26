import {
    clearError,
    elementMakeInvisible,
    elementSetText,
    isPasskeyAvailable,
    redirectDashBoardLoggedIn,
    setErrorAttached,
    showQRCode,
    validatePassword,
    validateUsername
} from './util.js';
import {BackendRegisterUserPassword} from "./backend.js";

const qrPopup = document.getElementById('qrPopup');


document.addEventListener('DOMContentLoaded', () => {
    redirectDashBoardLoggedIn()

    // Cache DOM elements
    const form = document.querySelector('.login-form');
    const passkeyButton = document.getElementById('passkey-btn');
    const authError = document.getElementById('auth-error');
    const userNameInput = document.getElementById('username');
    const passwordInput = document.getElementById('password');
    const confirmPasswordInput = document.getElementById('confirm-password');


    const validateConfirmPassword = (confirmElem) => {
        const passwordStr = passwordInput.value.trim();
        const confirmStr = confirmElem.value.trim();
        if (confirmStr !== passwordStr) {
            setErrorAttached(confirmElem, 'Passwords do not match.');
            return false;
        }
        clearError(confirmElem);
        return true;
    };

    qrPopup.addEventListener('click', () => {
        qrPopup.style.display = 'none';
        window.location.href = '/login';

    })

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


    // Handle form submission for password signup
    form.addEventListener('submit', async (event) => {
        event.preventDefault();
        elementMakeInvisible(authError);

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
                setErrorAttached(elem, 'This field is required.');
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
            let result = BackendRegisterUserPassword(usernameValue, passwordValue);
            userNameInput.value = '';
            passwordInput.value = '';
            confirmPasswordInput.value = '';

            await result.then(response => {
                qrPopup.style.display = 'block'
                showQRCode(response, document.getElementById("qrCanvas"))
            });
        } catch (error) {
            elementSetText(authError, "The sign-up has failed. Please check your details and try again.");
            console.error('Error during registration:', error);
        }
    });

    //TODO ADD DEBUG USER
    BackendRegisterUserPassword("hey", "123123123")

    if (isPasskeyAvailable()) {
        passkeyButton.addEventListener('click', async () => {
            elementMakeInvisible(authError);

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
                elementSetText(authError, "Passkey signup failed. Please try again.");
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