// SPDX-License-Identifier: GPL-3.0-only

export function IconFile(isFile) {
    if (!isFile) {
        return `<svg viewBox="0 0 24 24" width="20" height="20" fill="var(--primary)">
              <path d="M10 4H4a2 2 0 00-2 2v12a2 2 0 002 2h16a2 2 0 002-2V8a2 2 0 00-2-2h-8l-2-2z"/>
            </svg>`;
    } else {
        return `<svg viewBox="0 0 24 24" width="20" height="20" fill="var(--primary)">
              <path d="M14 2H6a2 2 0 00-2 2v16a2 2 0 002 2h12a2 2 0 002-2V8z"/>
              <polyline points="14 2 14 8 20 8" fill="none" stroke="var(--primary)" stroke-width="2"/>
            </svg>`;
    }
}
export function IconOptions() {
    return `<svg viewBox="0 0 24 24" width="16" height="16" fill="var(--text)">
            <circle cx="5" cy="12" r="2"/>
            <circle cx="12" cy="12" r="2"/>
            <circle cx="19" cy="12" r="2"/>
          </svg>`;
}

export function IconShare() {
    return `<svg viewBox="0 0 24 24" width="16" height="16" fill="var(--text)">
            <path d="M3.9 12c0-1.1.9-2 2-2h3v2H5.9a.9.9 0 00-.9.9c0 .5.4.9.9.9H8v2H5.9c-1.1 0-2-.9-2-2zM15 10h-3V8h3c1.1 0 2 .9 2 2s-.9 2-2 2h-3v-2h3c.55 0 1-.45 1-1s-.45-1-1-1zM8.4 11.7l6.3-6.3 1.4 1.4-6.3 6.3-1.4-1.4zm7.2 0l-6.3 6.3-1.4-1.4 6.3-6.3 1.4 1.4z"/>
          </svg>`;
}
export function IconDelete() {
    return `<svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="var(--text)" stroke-width="2">
              <circle cx="12" cy="12" r="10"/>
              <path d="M15 9l-6 6M9 9l6 6"/>
            </svg>`;
}


export function IconDownload() {
    return `<svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="var(--text)" stroke-width="2">
              <path d="M12 16l-6 6h12l-6-6zM12 3v12"/>
            </svg>`;
}
