/*
 * SPDX-License-Identifier: GPL-3.0-only
 */

export function getIconHTML(type) {
    if (type === 'folder') {
        return `<svg viewBox="0 0 24 24" width="20" height="20" fill="var(--primary)">
              <path d="M10 4H4a2 2 0 00-2 2v12a2 2 0 002 2h16a2 2 0 002-2V8a2 2 0 00-2-2h-8l-2-2z"/>
            </svg>`;
    } else if (type === 'file') {
        return `<svg viewBox="0 0 24 24" width="20" height="20" fill="var(--primary)">
              <path d="M14 2H6a2 2 0 00-2 2v16a2 2 0 002 2h12a2 2 0 002-2V8z"/>
              <polyline points="14 2 14 8 20 8" fill="none" stroke="var(--primary)" stroke-width="2"/>
            </svg>`;
    }
}