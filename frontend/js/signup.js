import {
    clearAuthError,
    clearError,
    displayAuthError,
    fetchWithErrorHandling,
    showError,
    uint8ArrayToBase64,
    validatePassword,
    validateUsername
} from './util.js';

document.addEventListener('DOMContentLoaded', () => {
    // Cache DOM elements
    const form = document.querySelector('.login-form');
    const passkeyButton = document.getElementById('passkey-signup-btn');
    const authError = document.getElementById('auth-error');
    const userNameInput = document.getElementById('username');
    const passwordInput = document.getElementById('password');
    const confirmPasswordInput = document.getElementById('confirm-password');

    const HASH_LENGTH = 32;

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
        if (!isValid) return;

        // Use cached values
        const usernameValue = userNameInput.value.trim();
        const passwordValue = passwordInput.value.trim();

        await sodiumReady;

        // Hash the password
        const hashedPassword = window.sodium.crypto_generichash(
            HASH_LENGTH,
            window.sodium.from_string(passwordValue)
        );
        const hashedPasswordBase64 = uint8ArrayToBase64(hashedPassword);

        const requestBody = `username=${usernameValue}\npassword=${hashedPasswordBase64}`;

        try {
            await fetchWithErrorHandling('/api/signup', {
                method: 'POST',
                headers: {
                    'Content-Type': 'text/plain',
                    'Auth-Method': 'password'
                },
                body: requestBody
            });
            window.location.href = '/';
        } catch (error) {
            displayAuthError(authError, "Invalid authentication. Please try again.");
            console.error('Error during signup:', error);
        }
    });

    // Handle passkey signup
    passkeyButton.addEventListener('click', async () => {
        clearAuthError(authError);

        if (!validateUsername(userNameInput)) return;
        const usernameValue = userNameInput.value.trim();

        try {
            await sodiumReady;

            const challengeResponse = await fetchWithErrorHandling('/api/signup', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({username: usernameValue})
            });

            const {challenge} = await challengeResponse.json();

            const publicKey = {
                challenge: Uint8Array.from(challenge, c => c.charCodeAt(0)),
                rp: {name: 'teilpunkt'},
                user: {
                    id: Uint8Array.from(usernameValue, c => c.charCodeAt(0)),
                    name: usernameValue,
                    displayName: usernameValue
                },
                pubKeyCredParams: [{type: 'public-key', alg: -7}]
            };

            const credential = await navigator.credentials.create({publicKey});

            await fetchWithErrorHandling('/api/signup/passkey/verify', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify(credential)
            });

            window.location.href = '/dashboard';
        } catch (error) {
            displayAuthError(authError, "Passkey signup failed. Please try again.");
            console.error('Error during passkey signup:', error);
        }
    });
});