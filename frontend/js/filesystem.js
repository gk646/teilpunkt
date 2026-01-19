// SPDX-License-Identifier: GPL-3.0-only

import {fetchWithErrorHandling} from './util.js';

// Returns an array of directory DTOs.
export async function fetchRoots() {
    const response = await fetchWithErrorHandling('/api/filesystem/roots', {});
    return response.json();
}

// Returns an array of directory DTOs.
export async function fetchFavourites() {
    const response = await fetchWithErrorHandling('/api/filesystem/favourites', {});
    return response.json();
}

//Returns a directory object with an array of entries.
export async function fetchDirectory(directoryId) {
    const response = await fetchWithErrorHandling('/api/filesystem/directoryLookup', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({directory: directoryId})
    });
    return response.json();
}