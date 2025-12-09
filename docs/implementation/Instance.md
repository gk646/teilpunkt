### Security Assumptions

Some assumptions :

- When installing teilpunkt the machine is friendly and in the correct state

Requirements:

- Safe against physical tampering
    - While running
        - Reading memory => Encrypt memory with outside key (TPM)
    - While stopped
        - All data files are encrypted already with the general crypto scheme and instance files with instance secret
    - Modifying firmware
        - Startup is only possible with the instance secret and then the integrity of all needed outside sources is
          checked
            - TPM remote attestation (if possible and configured)
            - Integrity check of static files and self binary

Intended usage scenario:

- Self-hosted
- Physically in a (locked) server room
- Created and started by a trusted source
- Used by many users with different roles
    - From both inside and outside the local network

Attack vectors:

Local malicious user

- Access other peoples credentials
    - Social engineering
    - Attack other peoples hardware (frontend)
        - Keylogger,Email, XSS, Infected USB-sticks
- Physically tamper with the instance _such that it allows unauthorized access_
    - Dump its running memory
    - Replace system parts
    - 
Outside malicious attacker

- Send the instance information _such that it allows unauthorized access_

### Instance secret

The instant secret is some sort of entropy (passphrase) supplied by the creator and physical owner of the instance.
It is used to encrypt all instance related information on the disk such as:

- Virtual File System
- Instance configuration
- Instance statistics

As such losing the instance secret makes starting, operating the instance and accessing any files impossible.

This is necessary as encrypting above things needs plaintext access to some secret and storing that alongside results in
infinite regress. (storing a plaintext key used to encrypt a key which encrypts another key...)

Without such an outside secret, ownership of the physical hardware/software is equal to ownership of the instance. But
its one of the key design principles to uncouple this relationship.

### Role of a TPM

The role of the TPM is to supplied secrets that are

### Security

The security of the instance has several requirements:

- An instance (truly) belongs to the person with the instance secret
- 