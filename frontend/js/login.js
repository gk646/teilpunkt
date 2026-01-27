import {
    elementMakeInvisible,
    elementSetText,
    fetchWithErrorHandling,
    isPasskeyAvailable,
    setErrorAttached,
    validatePassword,
    validateUsername
} from './util.js';

import {BackendLoginUserPassword} from "./backend.js";

const form = document.querySelector('.login-form');
const errorField = document.getElementById('auth-error');
const passkeyButton = document.getElementById('passkey-btn');
const userNameInput = document.getElementById('username');
const passwordInput = document.getElementById('password');
const totpPopup = document.getElementById('totpPopup');
const totpInput = document.getElementById('totpInput');
const totpSubmit = document.getElementById('submitTOTP');

async function handlePasswordLogin() {
    try {
        const result = BackendLoginUserPassword(userNameInput.value.trim(), passwordInput.value.trim(), totpInput.value.trim());
        userNameInput.value = '';
        passwordInput.value = '';
        totpInput.value = '';
        await result;
        window.location.href = '/dashboard';
    } catch (error) {
        elementSetText(errorField, "The login has failed. Please check your login details and try again.");
        console.error('Error during login:', error);
    }
}

document.addEventListener('DOMContentLoaded', () => {
    totpInput.addEventListener('input', async function (e) {
            let value = e.target.value.replace(/\D/g, ''); // Remove non-numeric characters
            if (value.length > 6) {
                value = value.substring(0, 6);
            }
            e.target.value = value;
            if (value.length >= 6) {
                totpPopup.style.display = 'none';
                await handlePasswordLogin();
            }
        }
    );

    totpSubmit.addEventListener('click', async function () {
        const totpCode = document.getElementById('totpInput').value;
        if (totpCode.length === 6) {
            await handlePasswordLogin();
        } else {
            elementSetText(errorField, 'Please enter a valid 6-digit TOTP code.');
        }
        totpPopup.style.display = 'none';
    });

    userNameInput.addEventListener('input', () => {
        elementMakeInvisible(errorField);
        validateUsername(userNameInput);
    });
    passwordInput.addEventListener('input', () => {
        elementMakeInvisible(errorField);
        validatePassword(passwordInput);
    });

    form.addEventListener('submit', async (event) => {
        event.preventDefault();

        const inputs = [userNameInput, passwordInput];
        const validators = [
            validateUsername,
            validatePassword,
        ];

        let isValid = true;
        inputs.forEach((elem, idx) => {
            if (!elem.value.trim()) {
                setErrorAttached(elem, 'This field is required.');
                isValid = false;
            } else {
                if (!validators[idx](elem)) {
                    isValid = false;
                }
            }
        });

        if (isValid) { // Open totp code
            totpPopup.style.display = 'block';
            totpPopup.focus();
            totpInput.focus();
            totpInput.text = '';
        }
    });

    if (isPasskeyAvailable()) {
        // Handle passkey signup
        passkeyButton.addEventListener('click', async () => {
            elementMakeInvisible(errorField);

            if (!validateUsername(userNameInput)) return;
            const usernameValue = userNameInput.value.trim();

            try {
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
                elementSetText(errorField, "Passkey signup failed. Please try again.");
                console.error('Error during passkey signup:', error);
            }
        });
    } else {
        console.error("Passkey not supported")
        const passkeyButton = document.getElementById("passkey-btn");
        passkeyButton.disabled = true;
        passkeyButton.textContent = "Not supported";
        passkeyButton.style.opacity = "0.5";
        passkeyButton.style.cursor = "not-allowed";
    }

});