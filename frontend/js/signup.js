import {clearError, showError, uint8ArrayToBase64, validatePassword, validateUsername} from './util.js';


document.addEventListener('DOMContentLoaded', () => {
    const form = document.querySelector('.login-form');
    const passkeyButton = document.getElementById('passkey-signup-btn');
    const authError = document.getElementById('auth-error');

    const hashLength = 32;
    let sodiumReady = (async () => {
        await window.sodium.ready;
        return window.sodium;
    })();


    const fetchWithErrorHandling = async (url, options) => {
        const response = await fetch(url, options);
        if (!response.ok) {
            const errorText = await response.text();
            throw new Error(errorText);
        }
        return response;
    };

    const validateConfirmPassword = () => {
        const confirmPasswordInput = document.getElementById('confirm-password');
        const password = document.getElementById('password').value.trim();
        const confirmPassword = confirmPasswordInput.value.trim();
        if (confirmPassword !== password) {
            showError(confirmPasswordInput, 'Passwords do not match.');
            return false;
        } else {
            clearError(confirmPasswordInput);
            return true;
        }
    };

    const clearAuthError = () => {
        authError.style.display = 'none';
        authError.textContent = '';
    };

    // Real-time validation listeners
    ['username', 'password', 'confirm-password'].forEach(id => {
        document.getElementById(id).addEventListener('input', () => {
            clearAuthError();
            if (id === 'username') validateUsername();
            if (id === 'password') {
                validatePassword();
                validateConfirmPassword();
            }
            if (id === 'confirm-password') validateConfirmPassword();
        });
    });

    form.addEventListener('submit', async (event) => {
        event.preventDefault();
        clearAuthError();

        const inputs = ['username', 'password', 'confirm-password'];
        const validators = [validateUsername, validatePassword, validateConfirmPassword];

        let valid = true;
        inputs.forEach((id, idx) => {
            const inputEl = document.getElementById(id);
            if (!inputEl.value.trim()) {
                showError(inputEl, 'This field is required.');
                valid = false;
            } else {
                if (!validators[idx]()) valid = false;
            }
        });

        if (!valid) return;

        const username = document.getElementById('username').value.trim();
        const password = document.getElementById('password').value.trim();

        await sodiumReady;

        const hashedPassword = window.sodium.crypto_generichash(hashLength, window.sodium.from_string(password));
        const hashedPasswordBase64 = uint8ArrayToBase64(hashedPassword);

        const body = `username=${username}\npassword=${hashedPasswordBase64}`;

        try {
            await fetchWithErrorHandling('/api/signup', {
                method: 'POST',
                headers: {
                    'Content-Type': 'text/plain',
                    'Auth-Method': 'password',
                },
                body: body
            });
            window.location.href = '/';
        } catch (error) {
            console.error('Error during signup:', error);
            authError.style.display = 'block';
            authError.textContent = 'Invalid authentication. Please try again.';
        }
    });

    passkeyButton.addEventListener('click', async () => {
        clearAuthError();
        if (!validateUsername()) return;

        const username = document.getElementById('username').value.trim();

        try {
            await sodiumReady;

            const challengeResponse = await fetchWithErrorHandling('/api/signup', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({username}),
            });

            const {challenge} = await challengeResponse.json();

            const publicKey = {
                challenge: Uint8Array.from(challenge, c => c.charCodeAt(0)),
                rp: {name: 'teilpunkt'},
                user: {
                    id: Uint8Array.from(username, c => c.charCodeAt(0)),
                    name: username,
                    displayName: username,
                },
                pubKeyCredParams: [{type: 'public-key', alg: -7}],
            };

            const credential = await navigator.credentials.create({publicKey});

            await fetchWithErrorHandling('/api/signup/passkey/verify', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify(credential),
            });

            window.location.href = '/dashboard';
        } catch (error) {
            console.error('Error during passkey signup:', error);
            authError.style.display = 'block';
            authError.textContent = 'Passkey signup failed. Please try again.';
        }
    });
});