document.addEventListener('DOMContentLoaded', () => {
    const form = document.querySelector('.login-form');
    const passkeyButton = document.getElementById('passkey-signup-btn');

    const hashLength = 32;

    // Initialize libsodium once
    let sodiumReady = (async () => {
        await window.sodium.ready;
        return window.sodium;
    })();

    const uint8ArrayToBase64 = (array) => window.sodium.to_base64(array);

    const fetchWithErrorHandling = async (url, options) => {
        try {
            const response = await fetch(url, options);
            if (!response.ok) {
                const error = await response.text();
                throw new Error(error);
            }
            return response;
        } catch (error) {
            console.error(`Error with fetch to ${url}:`, error);
            throw error;
        }
    };

    form.addEventListener('submit', async (event) => {
        event.preventDefault();

        const username = document.getElementById('username').value.trim();
        const password = document.getElementById('password').value.trim();
        const confirmPassword = document.getElementById('confirm-password').value.trim();

        if (!username || !password || !confirmPassword) {
            alert('Please fill out all fields.');
            return;
        }

        if (password !== confirmPassword) {
            alert('Passwords do not match.');
            return;
        }

        const sodium = await sodiumReady;

        const hashedPassword = sodium.crypto_generichash(hashLength, sodium.from_string(password));
        const hashedPasswordBase64 = uint8ArrayToBase64(hashedPassword);

        const body = `username=${username}\npassword=${hashedPasswordBase64}`;

        try {
            const response = await fetchWithErrorHandling('/api/signup', {
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
        }
    });

    // Handle Passkey Signup
    passkeyButton.addEventListener('click', async () => {
        const username = document.getElementById('username').value.trim();

        if (!username) {
            alert('Please enter a username before signing up with a passkey.');
            return;
        }

        try {
            const sodium = await sodiumReady;

            // Step 1: Request passkey challenge from the backend
            const challengeResponse = await fetchWithErrorHandling('/api/signup', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({username}),
            });

            const {challenge} = await challengeResponse.json();

            // Step 2: Use WebAuthn API to generate passkey credentials
            const publicKey = {
                challenge: Uint8Array.from(challenge, (c) => c.charCodeAt(0)),
                rp: {name: 'teilpunkt'},
                user: {
                    id: Uint8Array.from(username, (c) => c.charCodeAt(0)),
                    name: username,
                    displayName: username,
                },
                pubKeyCredParams: [{type: 'public-key', alg: -7}],
            };

            const credential = await navigator.credentials.create({publicKey});

            // Step 3: Send the credential back to the backend
            const verificationResponse = await fetchWithErrorHandling('/api/signup/passkey/verify', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(credential),
            });

            alert('Passkey signup successful! Redirecting to dashboard...');
            window.location.href = '/dashboard';
        } catch (error) {
            console.error('Error during passkey signup:', error);
        }
    });
});