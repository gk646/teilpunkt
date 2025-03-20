## teilpunkt

> [!IMPORTANT]
> Until version 1.0.0 it is not recommended for production use.
> Work in progress - certain features might not be complete.


`teilpunkt`™ _(ger. sharepoint)_ is a _C++23_ **self-hosted, security first, file server** implemented natively for Linux® using modern technologies.

Dependencies:

- [uWebSockets](https://github.com/uNetworking/uWebSockets) _(latest)_: HTTPS server and sockets
- [libsodium](https://github.com/jedisct1/libsodium) _(latest)_: Crypto Primitives
- [libsodium.js](https://github.com/jedisct1/libsodium.js) _(latest)_: Crypto Primitives for the Frontend
- [glaze](https://github.com/stephenberry/glaze) _(latest)_: JSON library
- [ankerl](https://github.com/martinus/unordered_dense) _(latest)_: Optimized Hashmap

_Note: This project is built only with/for GCC and the Linux® kernel._

**Feature Highlights:**

- **Security first** approach using modern technology
- [Websocket](https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API) enabled file transfers
- Supports **passkey and MFA** authentication
- **Fast and compliant multithreaded webserver** with uWebSockets
- **Linux native** filesystem and webserver
- Comprehensive **auditing and logging** accessible with various endpoints
- Modern and lightweight frontend accessible on all devices
- Many more to come...

## Deployment

Disclaimer: Currently `teilpunkt` is not ready to be deployed

`teilpunkt` chooses its default settings favoring security over convenience in order to avoid mishaps. Tradeoffs for
more convenience have to be made explicitly by the admin that deploys the instance.

### Customization

Nearly all aspects of `teilpunkt` can be customized to accommodate for various deployment situations. It chooses more
secure defaults to avoid mishaps. Tradeoffs for more convenience have to be made explicitly by the admin that deploys
the instance.

### Storage

You can configure multiple storage locations (storage endpoints)

## Security

These are the active (and passive) security measures used by `teilpunkt`.

### Storage

- Use of encryption for all persisted files

### Transfer

- Use of HTTPS for encrypted traffic
- Use of client side hashing of credentials

### Deployment

- Runs only on Linux kernel
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

## Implementation

### Threading

#### WebServer Threads _(2-x)_:

- Handle incoming HTTP requests
- Directly handle tasks that return immediately:
    - Modify virtual filesystem
    - Create, Login or Remove users
    - Change settings, etc.
- Also handles chunked file transfer:
    - Use of non-blocking sockets allows to have many connections even per thread

#### Worker Threads _(1-x):_

- Priority-based queue worker system
    - Only run tasks that are explicitly given to them

#### Background Thread _(1):_

- Runs the main event loop
    - Handles incoming events
    - Adds tasks to the worker thread

This means it runs hardware concurrent on devices with 4+ cores.

### Instance

### Files and Data

## Notice

Linux® is the registered trademark of Linus Torvalds in the U.S. and other countries.