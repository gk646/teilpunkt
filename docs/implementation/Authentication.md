### Timed One Time Password (TOTP)

The purpose of TOTP is to share an additional secret with the client at creation time. It is ephemeral, so its only
produced this one time and then never again (it is stored however). A random byte sequence is generated and shared with
the user. By also agreeing to a common algorithm both the user and server can produce the same TOTP (truncated hash of
secret mixed with the current time) at the same time.
Validation does not require to send the secret (as both parties already have it). Also, the TOTP cannot be used in any
way to infer the secret.

Another nice property is that TOTP's are only valid for a limited amount of time (30 seconds usually). An intercepted
code is thus quickly useless.
It also sets a hard limit to the bruteforce attempts (should the server even allow it), as the TOTP has to be found for
each interval separately.
Modern TOTP implementation additionally have forward secrecy, so future codes cannot be inferred by previous ones.