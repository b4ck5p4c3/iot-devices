# B4CKSP4CE Internet of ~~shit~~ things

Monorepo with firmwares of small IoT devices used across the space.  
Secrets encrypted using [envienc](https://github.com/imcatwhocode/envienc).

## Secrets usage
You need Node.js 16.x or newer installed.
```bash
# Install envienc dependency
npm ci

# Decrypt
npx envienc decrypt

# Encrypt (if you added or changed secrets)
npx envienc encrypt
```

Encryption password is stored in RØ keychain.
