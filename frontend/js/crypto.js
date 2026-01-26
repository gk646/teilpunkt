/*
 * SPDX-License-Identifier: GPL-3.0-only
 */

const HASH_LENGTH = 24 // Produces 32 length base 64

export async function hashPassword(plainPassword) {
    await window.sodium.ready;
    let hashedPassword = window.sodium.crypto_generichash(
        HASH_LENGTH,
        window.sodium.from_string(plainPassword)
    );
    return window.sodium.to_base64(hashedPassword)
}

