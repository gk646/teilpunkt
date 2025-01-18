### Security

- Allow hosting towards internet with safeguards
    - DDos protection
        - WhiteList/Blacklist IP's
        - Rate limiting
- Randomize Memory Layout
- Computation Time:
    - Side Channel Attacks (find information about data by measure information about its processing)
- Wrong Authentication always takes 2 sec (max 5 per minute)
- The secret used is a hash derived from the users authentication method
    - Can only have 1 auth method active
    - Can be changed by unwrapping with the old and re-wrapping with the new secret
- Server persists as little as possible
    - Encrypted files
    - Encrypted file indices
        - Private-Mode: Generic-Name (User-SecretFile1,...),encrypted file size,

### Crypto

- Each user has a secret that is derived form their auth method
- Files are encrypted with a resource key (RK)
- RK is generated when the Files is uploaded
- RK is immediately encrypted (wrapped) with the uploaders secret
- There is a admin secret per instance given by the admin after instance startup
- Private-mode is when the RK is only wrapped with the uploaders secret (not the admins)
    - This means only the uploader can unwrap the RK and thus share access (and revoke it)
- For all files not created in private-mode the RK is additionally wrapped with the admin secret
    - This allows to share access with any user at any time (as you need to decrypt the RK and encrypt it with the
      target users secret)
- Only the wrapped keys are persisted to file

### User Features

- File Operations:
    - Upload
    - Download

- Authentication:
    - Passkeys
    - Password + any 2FA method
        - Email
        - Authenticator App
        - Biometrics
    - Cookie Sessions:
        - Session Token
        - HttpOnly, Secure, and SameSite flags
- User groups
    - Admin and normal user
    - Storage location with permissions
    - Subfolders which inherit permissions (like GitLab)
    - Permissions can be assigned individually or in groups (to either users or usergroups)
        - Write/Read
        - Delete, Rename, Move, Read, Change, ...
- Storage Location
    - Choose storage endpoint (maybe assigned by admin to user)

- Flag critical actions that always require 2FA

### API

- API endpoints to programmatically interact with the instance (save and retrieve files)
- Allows for easy syncing and local cloud features

### Monitoring

- Monitor requests (invalidate session cookie)
    - IP Change (within same session) is not allowed (fair even for VPN's or Tor)
    -
- DDOs protection
    - Without session only allow authentication request
    - Wrong auth takes 2 sec (all other requests from that IP blocked)
    - Limit to 50 requests per minute

### Implementation

- Memory:
    - Abstract Allocation
        - Allows any backend
        - Reduce fragmentation (or do it on purpose against memory dump attacks?)
        - Random layout

- File Storage
    - Virtual Filesystem
      - Server does not create local filesystem according to the uploaded folders
      - Each filesystem is stored in 2 encrypted files: index and filedata 
      - Immediate mode storage
        - Files are immediately added to virtual file system index
        - Files are immediately persisted to endpoint
      - Uploading files happens via transactions
        - Either upload and encryption and storage succeed or is refused
        - Transaction happens fully in memory and is deleted after either outcome
    - Multiple storage endpoints
      - Each has a virtual file system at its base
    - Can be added at runtime
    - Everything is stored encrypted
        - The endpoint has no knowledge of plain files

### Deployment

- Deployment:
    - Disable core dumps
    - Disable swap partitions

User Story:

1. Open in Web
2. Authenticate
3. Now can view and upload files from/to supported locations

Upload:

4. Select File and upload
5.

Manage Files:

4. Rename existing file

Server Story:
1. Startup
    - Prompt for admin key
2. Decrypt indices of virtual filesystems
    - Load them from disk
    - Check the file hashes for integrity
1. Listening for requests
2. Authenticate via any credentials (or with valid session cookie)
    - Keep user secret in memory until session expires (or logs off)

Upload:





// Finished Sec