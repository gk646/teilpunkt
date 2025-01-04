## teilpunkt

> [!IMPORTANT]
> Until version 1.0.0 this is not production ready. Use at your own risk (see License).


`teilpunkt`™ _(ger. sharepoint)_ is a modern

## Authentication

- Passkeys
- Password + any 2FA method
    - Email
    - Authenticator App
    - Biometrics

## Deployment

`teilpunkt` chooses its default settings favoring security over convenience in order to avoid mishaps. Tradeoffs for
more convenience have to be made explicitly by the admin that deploys the instance.

### Customization

Nearly all aspects of `teilpunkt` can be customized to accommodate for various deployment situations. It chooses more
secure defaults to avoid mishaps. Tradeoffs for more convenience have to be made explicitly by the admin that deploys
the instance.

## Implementation

### Memory

A `teilpunkt` instance runs entirely in RAM. It uses sophisticated techniques to manage memory such as page locking,
memory cleaning and randomized layout to protect against attacks on its running memory.

### Storage

You can configure multiple storage locations (storage endpoints) 