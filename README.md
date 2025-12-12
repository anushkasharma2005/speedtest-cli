# Speedtest CLI

A lightweight command-line internet speed test tool written in C. Measures download/upload speeds using CDN servers with multi-threaded connections.

## Features

- Multi-threaded download testing (8 parallel connections)
- Upload speed testing via Cloudflare
- Automatic server selection based on latency
- Real-time progress display
- ISP and IP geolocation information
- HTTP/2 support for better performance
- Works globally with CDN-based test servers
- Colorful terminal output
- Cross-distribution support
- Minimal dependencies

## Quick Start

```bash
# Clone the repository
git clone https://github.com/anushkasharma2005/speedtest-cli.git
cd speedtest-cli

# Install dependencies (requires sudo)
sudo make install-deps

# Build and run
make run
```

That's it! The `make run` command will build the project if needed and run the speed test.

## Installation

### Option 1: Just Run It (Recommended)

```bash
# Install dependencies first
sudo make install-deps

# Build and run in one command
make run
```

### Option 2: Build Manually

```bash
# 1. Install dependencies
sudo make install-deps

# 2. Build
make

# 3. Run
./speedtest
```

### Option 3: Install System-Wide

```bash
# After building, install to /usr/local/bin
sudo make install

# Now you can run from anywhere
speedtest

# To uninstall later
sudo make uninstall
```

## Manual Dependency Installation

If `make install-deps` doesn't work for your distribution:

**Arch Linux / Manjaro:**
```bash
sudo pacman -S gcc make curl json-c
```

**Debian / Ubuntu / Linux Mint:**
```bash
sudo apt install gcc make libcurl4-openssl-dev libjson-c-dev
```

**Fedora / RHEL / CentOS:**
```bash
sudo dnf install gcc make libcurl-devel json-c-devel
```

**openSUSE:**
```bash
sudo zypper install gcc make libcurl-devel libjson-c-devel
```

**Alpine Linux:**
```bash
sudo apk add gcc make curl-dev json-c-dev musl-dev
```

## Usage

```bash
# Full speed test (download + upload)
./speedtest

# Quick test (download only, faster)
./speedtest -q
./speedtest --quick

# Show help
./speedtest --help

# Show version
./speedtest --version
```

## Example Output

```
╔════════════════════════════════════════╗
║        SPEEDTEST CLI v1.0              ║
║     Network Speed Test Tool            ║
╚════════════════════════════════════════╝

 Fetching connection information...

 Connection Information:
─────────────────────────────────────────
  IP Address:  203.0.113.42
  ISP:         Example Internet Provider
  Location:    Mumbai, Maharashtra, India
  Timezone:    Asia/Kolkata

 Running Speed Tests:
─────────────────────────────────────────────────────────────────────────────────────────────
   Finding best server..... Server 1 (45ms)
   Testing latency... 12.34 ms

   Testing download (8 connections)...
   Download:  31.25 Mbps [100%] [==================================================] DONE

   Testing upload (25 MB)...
   Upload:    28.50 Mbps [100%] [==================================================] DONE
─────────────────────────────────────────────────────────────────────────────────────────────

 Final Results:
═════════════════════════════════════════
   Latency:     12.34 ms
   Download:    31.25 Mbps
   Upload:      28.50 Mbps
═════════════════════════════════════════
```

## Makefile Commands

| Command | Description |
|---------|-------------|
| `make` | Build the project |
| `make run` | Build (if needed) and run the speed test |
| `make install-deps` | Install system dependencies (requires sudo) |
| `make install` | Install to /usr/local/bin (requires sudo) |
| `make uninstall` | Remove from system (requires sudo) |
| `make clean` | Remove build files |
| `make help` | Show all available commands |

## Project Structure

```
speedtest-cli/
├── src/
│   ├── main.c        # Entry point and argument parsing
│   ├── network.c     # Speed test logic (download/upload/latency)
│   ├── ip_info.c     # ISP and IP geolocation lookup
│   └── display.c     # Terminal output formatting
├── include/
│   ├── network.h
│   ├── ip_info.h
│   └── display.h
├── Makefile          # Build configuration
├── install-deps.sh   # Dependency installer script
└── README.md
```

## How It Works

1. **Server Selection**: Tests multiple CDN servers and picks the one with lowest latency
2. **Download Test**: Opens 8 parallel TCP connections and measures throughput over 12 seconds
3. **Upload Test**: Sends 25MB of data to Cloudflare's speed test endpoint
4. **Speed Calculation**: Uses 75th percentile of samples (similar to Ookla methodology)

## Test Servers

The tool automatically selects the best server from:

- Cloudflare Speed Test (global CDN)
- Leaseweb Singapore
- Tele2 Sweden (fallback)
- OVH France (fallback)

## Troubleshooting

**Build errors:**
```bash
# Make sure dependencies are installed
sudo make install-deps

# Clean and rebuild
make clean && make
```

**Low speeds compared to browser tests:**
- Your ISP may prioritize traffic to Ookla's servers
- Try running the test multiple times
- Network congestion varies throughout the day

**Connection errors:**
- Check your internet connection
- Some corporate firewalls may block speed test servers

## License

MIT License

## Contributing

Pull requests are welcome. For major changes, please open an issue first.