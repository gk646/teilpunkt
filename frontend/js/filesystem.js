// SPDX-License-Identifier: GPL-3.0-only

import {fetchWithErrorHandling} from './util.js';

// Returns an array of directory DTOs.
export async function fetchRoots() {
    const response = await fetchWithErrorHandling('/api/filesystem/roots');
    return response.json();
}

// Returns an array of directory DTOs.
export async function fetchFavourites() {
    const response = await fetchWithErrorHandling('/api/filesystem/favourites');
    return response.json();
}

//@param {string} directoryId - Identifier for the directory.
//Returns a directory object with an array of entries.
export async function fetchDirectory(directoryId) {
    const response = fetchWithErrorHandling('/api/filesystem/directory', {
        method: 'GET',
        headers: {
            'Content-Type': 'application/json',
        },
        body: directoryId
    });
    return response.json();
}