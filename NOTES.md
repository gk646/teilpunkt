### Security
- Allow hosting towards internet with safeguards
  - DDos protection
    - WhiteList/Blacklist IP's
    - Rate limiting
- Randomize Memory Layout
- Computation Time:
  - Side Channel Attacks (find information about data by measure information about its processing)


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
- Storage Location
  - Choose storage endpoint (maybe assigned by admin to user)

### Implementation

- Memory: 
  - Abstract Allocation
    - Allows any backend
    - Reduce fragmentation
    - Random layout

- File Storage
  - Virtual Filesystem
  - Multiple storage endpoints
  - Allow adding at runtime

### Deployment

- Deployment:
  - Disable core dumps
  - Disable swap partitions