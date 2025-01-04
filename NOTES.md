### Security
- Allow hosting towards internet with safeguards
  - DDos protection
    - WhiteList/Blacklist IP's
    - Rate limiting
- Randomize Memory Layout
- Computation Time:
  - Side Channel Attacks (find information about data by measure information about its processing)
- Wrong Authentication always takes 2 sec (max 5 per minute)

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
1. Listening for requests
2. Authenticate via any credentials (or with valid session cookie)

Upload:
3. 