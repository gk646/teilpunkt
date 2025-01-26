## teilpunkt

> [!IMPORTANT]
> Until version 1.0.0 it is not recommended for production use.


`teilpunkt`™ _(ger. sharepoint)_ is a highly secure, self-hosted file server.

Note: This made to compile with GCC and for a Linux kernel as open source is essential for security.

## Authentication

- Passkeys
- Password + any 2FA method
    - Email
    - Authenticator App
    - Biometrics
- Single Sign On (SSOA)

## Deployment

`teilpunkt` chooses its default settings favoring security over convenience in order to avoid mishaps. Tradeoffs for
more convenience have to be made explicitly by the admin that deploys the instance.

### Customization

Nearly all aspects of `teilpunkt` can be customized to accommodate for various deployment situations. It chooses more
secure defaults to avoid mishaps. Tradeoffs for more convenience have to be made explicitly by the admin that deploys
the instance.

### Storage

You can configure multiple storage locations (storage endpoints)

## Security

These are the actively used security measures used by `teilpunkt`.

### Storage

- Use of encryption for all persisted files

### Transfer

- Use of HTTPS for encrypted traffic
- Use of client side hashing of credentials

### Deployment

- Runs only on recent GNU/Linux
- All unneeded ports blocked (with setup.sh)
- Core dumps disabled (with setup.sh)
- Use of locking and blocking sensitive memory regions
- Use of encrypting sensitive data in memory with TPM support

### Frontend

- Use of file hashes for served static files to detect tampering
- No usage of CDN's, font providers or third parties services to avoid sharing user data (IP, ...)
- No frontend framework for simple and auditable code

### Source Code

- Use of clear code formatting
- Use of simple code style (_C + templates_)
- Use of only few dynamic memory

### Compilation

- Only compiles with recent GCC
- Use of safe optimization level -O2
- Use of symbol stripping (except relocation information to not hinder ASLR)
- Use of many compiler warning flags