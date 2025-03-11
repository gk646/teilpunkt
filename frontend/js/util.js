
export const showError = (inputEl, message) => {
    inputEl.classList.add('invalid');
    const errorEl = document.getElementById(`${inputEl.id}-error`);
    if (errorEl) {
        errorEl.textContent = message;
        errorEl.style.visibility = 'visible';
    }
};

export const clearError = (inputEl) => {
    inputEl.classList.remove('invalid');
    const errorEl = document.getElementById(`${inputEl.id}-error`);
    if (errorEl) {
        errorEl.textContent = '';
        errorEl.style.visibility = 'hidden';
    }
};

export const clearAuthError = (authErrorEl) => {
    authErrorEl.style.display = 'none';
    authErrorEl.textContent = '';
};

export const displayAuthError = (authErrorEl, message) => {
    authErrorEl.textContent = message;
    authErrorEl.style.display = 'block';
};


export const uint8ArrayToBase64 = (array) => window.sodium.to_base64(array);


export const validateUsername = () => {
    const usernameInput = document.getElementById('username');
    const username = usernameInput.value.trim();
    if (username.length < 3) {
        showError(usernameInput, 'Username must be at least 3 characters.');
        return false;
    } else {
        clearError(usernameInput);
        return true;
    }
};

export const validatePassword = () => {
    const passwordInput = document.getElementById('password');
    const password = passwordInput.value.trim();
    if (password.length < 7) {
        showError(passwordInput, 'Password must be at least 7 characters.');
        return false;
    } else {
        clearError(passwordInput);
        return true;
    }
};