const USERNAME_LEN = 3
const PASSWORD_LEN = 7
const HASH_LENGTH = 24 // Produces 32 length base 64

// Hash the password
export const hashPassword = (plainPassword) => {
    let hashedPassword = window.sodium.crypto_generichash(
        HASH_LENGTH,
        window.sodium.from_string(plainPassword)
    );
    return window.sodium.to_base64(hashedPassword)
}

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
    authErrorEl.style.opacity = '0';
};

export const displayAuthError = (authErrorEl, message) => {
    authErrorEl.style.opacity = '1';
    authErrorEl.textContent = message
};

export const validateUsername = (userNameElement) => {
    const username = userNameElement.value.trim();
    if (username.length < USERNAME_LEN) {
        showError(userNameElement, 'Username must be at least 3 characters');
        return false;
    } else {
        clearError(userNameElement);
        return true;
    }
};

export const validatePassword = (passwordElement) => {
    const password = passwordElement.value.trim();
    if (password.length < PASSWORD_LEN) {
        showError(passwordElement, 'Password must be at least 7 characters');
        return false;
    } else {
        clearError(passwordElement);
        return true;
    }
};

export const fetchWithErrorHandling = async (url, options) => {
    const response = await fetch(url, options);
    if (!response.ok) {
        const errorText = await response.text();
        throw new Error(errorText);
    }
    return response;
};