# Building

## Requirements

- C++ compiler
- xmake

You can install [xmake](https://xmake.io/) on your machine using these one-liners:

```bash
# Unix systems
bash <(curl -fsSL https://xmake.io/shget.text)

# Windows Powershell
Set-ExecutionPolicy RemoteSigned -scope CurrentUser
Invoke-Expression (Invoke-Webrequest 'https://xmake.io/psget.text' -UseBasicParsing).Content
```

## Usage

```bash
# Optionally: use debug mode
xmake config -m debug

# Build
xmake

# Run
xmake run
```
