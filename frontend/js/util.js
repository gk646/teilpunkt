const USERNAME_LEN = 3
const PASSWORD_LEN = 7
const USER_ID_COOKIE = "tp-session-user"


export function setErrorAttached(inputEl, message) {
    inputEl.classList.add('invalid');
    const errorEl = document.getElementById(`${inputEl.id}-error`);
    if (errorEl) {
        errorEl.textContent = message;
        errorEl.style.visibility = 'visible';
    }
}

export function showQRCode(data, canvas) {
    const QRC = qrcodegen.QrCode;
    const qr = QRC.encodeText(data, QRC.Ecc.MEDIUM);

    function drawCanvas(qr, scale, border, lightColor, darkColor, canvas) {
        if (scale <= 0 || border < 0)
            throw new RangeError("Value out of range");

        const size = (qr.size + border * 2) * scale;
        canvas.width = size;
        canvas.height = size;
        let ctx = canvas.getContext("2d");
        for (let y = -border; y < qr.size + border; y++) {
            for (let x = -border; x < qr.size + border; x++) {
                ctx.fillStyle = qr.getModule(x, y) ? darkColor : lightColor;
                ctx.fillRect((x + border) * scale, (y + border) * scale, scale, scale);
            }
        }
    }

    drawCanvas(qr, 5, 4, '#FFFFFF', '#000000', canvas);
}

export function clearError(inputEl) {
    inputEl.classList.remove('invalid');
    const errorEl = document.getElementById(`${inputEl.id}-error`);
    if (errorEl) {
        errorEl.textContent = '';
        errorEl.style.visibility = 'hidden';
    }
}

export const elementMakeInvisible = (ele) => {
    ele.style.opacity = '0';
};

export const elementSetText = (ele, message) => {
    ele.style.opacity = '1';
    ele.textContent = message
    ele.style.visibility = 'visible';

};

export const validateUsername = (userNameElement) => {
    const username = userNameElement.value.trim();
    if (username.length < USERNAME_LEN) {
        setErrorAttached(userNameElement, 'Username must be at least 3 characters');
        return false;
    } else {
        clearError(userNameElement);
        return true;
    }
};

export const validatePassword = (passwordElement) => {
    const password = passwordElement.value.trim();
    if (password.length < PASSWORD_LEN) {
        setErrorAttached(passwordElement, 'Password must be at least 7 characters');
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

export const isPasskeyAvailable = () => {
    return window.PublicKeyCredential &&
        PublicKeyCredential.isUserVerifyingPlatformAuthenticatorAvailable &&
        PublicKeyCredential.isConditionalMediationAvailable;

}

export const redirectDashBoardLoggedIn = () => {
    if (document.cookie.split(';').some(cookie => cookie.trim().startsWith(USER_ID_COOKIE + '='))) {
        //window.location.replace('/dashboard.html');
    }
}