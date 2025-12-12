# Speedtest CLI

A lightweight command-line internet speed test tool written in C. Provides accurate speed measurements using multi-threaded downloads and modern CDN servers.

## Features

- Multi-threaded download testing (parallel connections)
- Upload speed testing via Cloudflare
- Automatic server selection based on latency
- Real-time progress display
- ISP and IP geolocation information
- HTTP/2 support for better performance
- Works globally with CDN-based test servers
- Colorful terminal output
- Distro-independent installation
- Minimal dependencies

## How It Works

The tool measures your internet speed similar to how Ookla's Speedtest works:

1. **Server Selection**: Tests multiple servers and picks the one with lowest latency
2. **Download Test**: Opens 8 parallel TCP connections and measures throughput over 12 seconds
3. **Upload Test**: Sends 25MB of data to Cloudflare's speed test endpoint
4. **Speed Calculation**: Uses 75th percentile of samples (top 25% average) for accurate results

## Installation

### Automatic (Recommended)

```bash
# 1. Install dependencies
sudo make install-deps

# 2. Build
make

# 3. Test locally
./speedtest

# 4. Install system-wide (optional)
sudo make install
```

### Manual Dependency Installation

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
# Run full speed test
speedtest

# Quick mode (download only)
speedtest -q

# Show help
speedtest --help

# Show version
speedtest --version
```

## Example Output

```
Connection Information:
-----------------------------------------
  IP Address:  203.0.113.42
  ISP:         Example Internet Provider
  Location:    Mumbai, Maharashtra, India
  Timezone:    Asia/Kolkata

Running Speed Tests:
-------------------------------------------------------------------------------------
   Finding best server..... Server 1 (45ms)
   Testing latency... 12.34 ms

   Testing download (8 connections)...
   Download:  31.25 Mbps [100%] [==================================================] DONE

   Testing upload (25 MB)...
   Upload:    28.50 Mbps [100%] [==================================================] DONE
-------------------------------------------------------------------------------------

Final Results:
=========================================
   Latency:     12.34 ms
   Download:    31.25 Mbps
   Upload:      28.50 Mbps
=========================================
```

## Project Structure

```
speedtest/
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
├── requirements.txt  # Manual dependency info
└── README.md
```

## Test Servers

The tool automatically selects the best server from:

- Cloudflare Speed Test (global CDN, recommended)
- Leaseweb Singapore
- Tele2 Sweden (fallback)
- OVH France (fallback)

## Technical Details

- **Language**: C (C11 standard)
- **Threading**: POSIX threads (pthread)
- **HTTP Client**: libcurl with HTTP/2 support
- **JSON Parsing**: json-c library
- **Speed Calculation**: 75th percentile method (matches Ookla methodology)
- **TCP Slow Start**: First 2 seconds of data discarded for accuracy

## Supported Distributions

- Arch Linux / Manjaro / EndeavourOS
- Debian / Ubuntu / Linux Mint / Pop!_OS
- Fedora / RHEL / CentOS / Rocky Linux / AlmaLinux
- openSUSE / SLES
- Alpine Linux

## Makefile Commands

```bash
make                # Build the project
make install-deps   # Install system dependencies (requires sudo)
make install        # Install to /usr/local/bin (requires sudo)
make uninstall      # Remove from system (requires sudo)
make clean          # Remove build files
make help           # Show all available commands
```

## Troubleshooting

**Low download speeds compared to browser-based tests:**
- The tool uses public test servers; your ISP may prioritize traffic to Ookla's servers
- Try running the test multiple times
- Check if your ISP throttles non-standard speed test traffic

**Connection errors:**
- Ensure you have a working internet connection
- Check if your firewall allows outbound HTTP/HTTPS connections
- Some corporate networks may block speed test servers

**Build errors:**
- Make sure all dependencies are installed: `sudo make install-deps`
- Verify pkg-config can find libraries: `pkg-config --exists libcurl json-c`

## License

MIT License

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## Acknowledgments

- Cloudflare for providing free speed test endpoints
- ip-api.com for IP geolocation services