import {
    clearAuthError,
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

    const sodiumReady = (async () => {
        await window.sodium.ready;
        return window.sodium;
    })();

    // Attach real-time validation
    userNameInput.addEventListener('input', () => {
        validateUsername(userNameInput);
    });

    passwordInput.addEventListener('input', () => {
        validatePassword(passwordInput);
    });

    // Handle form submission for password signup
    form.addEventListener('submit', async (event) => {
        event.preventDefault();
        clearAuthError(authError);

        const inputs = [userNameInput, passwordInput];
        const validators = [
            validateUsername,
            validatePassword,
        ];

        let isValid = true;
        inputs.forEach((elem, idx) => {
            if (!elem.value.trim()) {
                showError(elem, 'This field is required.');
                isValid = false;
            } else {
                if (!validators[idx](elem)) {
                    isValid = false;
                }
            }
        });
        if (!isValid) {
            return;
        }

        await sodiumReady;

        let usernameValue = userNameInput.value.trim();
        let passwordValue = passwordInput.value.trim();
        let hashedPassword = hashPassword(passwordValue);
        let requestBody = JSON.stringify({
            name: usernameValue,
            password: hashedPassword
        });
        passwordValue = null;
        usernameValue = null;
        try {
            let result = fetchWithErrorHandling('/api/auth/password', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: requestBody
            });

            userNameInput.value = '';
            passwordInput.value = '';
            hashedPassword = null;
            requestBody = null;

            await result;

            window.location.href = '/dashboard';
        } catch (error) {
            displayAuthError(authError, "The login has failed. Please check your login details and try again.");
            console.error('Error during login:', error);
        }
    });

    if (isPasskeyAvailable()) {
        // Handle passkey signup
        passkeyButton.addEventListener('click', async () => {
            clearAuthError(authError);

            if (!validateUsername(userNameInput)) return;
            const usernameValue = userNameInput.value.trim();

            try {
                await sodiumReady;

                const challengeResponse = await fetchWithErrorHandling('/api/auth/passkey-get', {
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

                await fetchWithErrorHandling('/api/auth/passkey', {
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
    } else {
        console.error("Passkey not supported")
        const passkeyButton = document.getElementById("passkey-btn");
        passkeyButton.disabled = true;
        passkeyButton.textContent = "Use Passkey";
        passkeyButton.style.opacity = "0.5";
        passkeyButton.style.cursor = "not-allowed";
    }

});