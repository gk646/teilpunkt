// SPDX-License-Identifier: GPL-3.0-only

import {fetchWithErrorHandling} from './util.js';
import {hashPassword} from "./crypto.js";

export async function BackendLoginUserPassword(username, password, totp) {
    let hashedPassword = await hashPassword(password);

    let requestBody = JSON.stringify({
        name: username,
        password: hashedPassword,
        totp: totp
    });

    return fetchWithErrorHandling('/api/auth/password', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: requestBody
    })
}

export async function BackendRegisterUserPassword(username, password) {
    let hashedPassword = await hashPassword(password);
    const requestBody = JSON.stringify({
        name: username.trim(),
        password: hashedPassword,
    });

    const result = await fetchWithErrorHandling('/api/register/password', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Auth-Method': 'password'
        },
        body: requestBody
    });

    hashedPassword = null
    password = null;
    return result.text();
}

// Returns an array of directory DTOs.
export async function BackendFetchUserRoots() {
    const response = await fetchWithErrorHandling('/api/filesystem/roots', {});
    return response.json();
}

//Returns a directory object with an array of entries.
export async function BackendDirLookup(directoryId) {
    const response = await fetchWithErrorHandling('/api/filesystem/dirLookup', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({directory: directoryId})
    });
    return response.json();
}

export async function BackendDirCreate(directory, name) {
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

export async function BackendDirDelete(directory) {
    const body = {
        file: directory,
    };

    await fetchWithErrorHandling('/api/filesystem/dir', {
        method: 'DELETE',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(body)
    });
}

export async function BackendFileCreate(directory, name) {
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

export async function BackendFileUpload(file, fid) {
    const chunkSize = 1024 * 512;

    async function uploadChunk(chunk) {
        const headers = new Headers();
        headers.append('file-name', file.name);
        headers.append('directory', fid);

       await fetchWithErrorHandling('/api/filesystem/upload', {
            method: 'POST',
            body: chunk,
            headers: headers
        });
    }

    async function uploadFile() {
        const totalChunks = Math.ceil(file.size / chunkSize);

        for (let i = 0; i < totalChunks; i++) {
            const start = i * chunkSize;
            const end = Math.min(start + chunkSize, file.size);
            const chunk = file.slice(start, end);

            try {
              await uploadChunk(chunk);
                console.log(`Chunk ${i} uploaded successfully`);
            } catch (error) {
                console.error(`Error uploading chunk ${i}:`);
                break;
            }
        }
    }

    await uploadFile();
}

export async function BackendFileDelete(fid) {
    const body = {
        file: fid,
    };

    await fetchWithErrorHandling('/api/filesystem/file', {
        method: 'DELETE',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(body)
    });
}