document.addEventListener('DOMContentLoaded', () => {
    const form = document.querySelector('.login-form');
    const passwordSection = document.getElementById('password-section');
    const passkeyButton = document.getElementById('passkey-signup-btn');

    // Handle Password Signup
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
        const body = `username=${username}\npassword=${password}`;

        try {
            const response = await fetch('/api/signup', {
                method: 'POST',
                headers: {'Content-Type': 'application/plain-text'},
                body: body
            });

            if (response.ok) {
                alert('Signup successful! Redirecting to home...');
                window.location.href = '/home';
            } else {
                const error = await response.text();
                alert(`Signup failed: ${error}`);
            }
        } catch (error) {
            console.error('Error during signup:', error);
            alert('An error occurred. Please try again.');
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
            // Step 1: Request passkey challenge from the backend
            const challengeResponse = await fetch('/api/signup', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({username}),
            });

            if (!challengeResponse.ok) {
                alert('Failed to initiate passkey signup.');
                return;
            }

            const {challenge} = await challengeResponse.json();

            // Step 2: Use WebAuthn API to generate passkey credentials
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

            // Step 3: Send the credential back to the backend
            const verificationResponse = await fetch('/api/signup/passkey/verify', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify(credential),
            });

            if (verificationResponse.ok) {
                alert('Passkey signup successful! Redirecting to dashboard...');
                window.location.href = '/dashboard';
            } else {
                alert('Passkey signup failed.');
            }
        } catch (error) {
            console.error('Error during passkey signup:', error);
            alert('An error occurred. Please try again.');
        }
    });

});