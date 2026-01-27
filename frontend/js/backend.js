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
    const headers = new Headers();
    headers.append('file-name', file.name);
    headers.append('directory', fid);

    try {
        await fetchWithErrorHandling('/api/filesystem/upload', {
            method: 'POST',
            body: file,
            headers: headers
        });
        console.log('File uploaded successfully');
    } catch (error) {
        console.error('Error uploading file:');
    }
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

export async function BackendFileDownload(fid) {

    const headers = new Headers();
    headers.append('file', fid);

    const response = await fetchWithErrorHandling('/api/filesystem/download', {
        method: 'POST',
        headers: headers
    });

    const contentDisposition = response.headers.get('Content-Disposition');
    let filename = 'download';
    if (contentDisposition) {
        const filenameMatch = contentDisposition.match(/filename="?([^"]+)"?/);
        if (filenameMatch && filenameMatch[1]) {
            filename = filenameMatch[1];
        }
    }

    const blob = await response.blob();
    const url = window.URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = filename;
    document.body.appendChild(a);
    a.click();

    window.URL.revokeObjectURL(url);
    document.body.removeChild(a);
}