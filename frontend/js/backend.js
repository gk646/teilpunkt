// SPDX-License-Identifier: GPL-3.0-only

import {fetchWithErrorHandling} from './util.js';

// Returns an array of directory DTOs.
export async function BackendFetchUserRoots() {
    const response = await fetchWithErrorHandling('/api/filesystem/roots', {});
    return response.json();
}

// Returns an array of directory DTOs.
export async function BackendFetchUserFavourites() {
    const response = await fetchWithErrorHandling('/api/filesystem/favourites', {});
    return response.json();
}

//Returns a directory object with an array of entries.
export async function BackendLookupDirectory(directoryId) {
    const response = await fetchWithErrorHandling('/api/filesystem/dirLookup', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({directory: directoryId})
    });
    return response.json();
}

export async function BackendCreateFile(directory, name) {
    const body = {
        name: name,
        directory: directory,
    };

    await fetchWithErrorHandling('/api/filesystem/file', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(body)
    });
}

export async function BackendCreateDir(directory, name) {
    const body = {
        name: name,
        directory: directory,
    };

    await fetchWithErrorHandling('/api/filesystem/dir', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(body)
    });
}
