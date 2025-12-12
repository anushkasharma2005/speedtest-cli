#!/bin/bash

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}Speedtest CLI - Dependency Installer${NC}\n"

# Detect the distribution
if [ -f /etc/os-release ]; then
    . /etc/os-release
    DISTRO=$ID
    DISTRO_LIKE=$ID_LIKE
else
    echo -e "${RED}Cannot detect distribution${NC}"
    exit 1
fi

echo -e "${YELLOW}Detected distribution: $DISTRO${NC}\n"

# Function to check if running as root
check_root() {
    if [ "$EUID" -ne 0 ]; then
        echo -e "${RED}Please run with sudo or as root${NC}"
        exit 1
    fi
}

# Install dependencies based on distro
case "$DISTRO" in
    arch|manjaro|endeavouros)
        echo "Installing for Arch-based system..."
        check_root
        pacman -Sy --needed --noconfirm gcc make curl json-c
        ;;
    
    ubuntu|debian|linuxmint|pop)
        echo "Installing for Debian-based system..."
        check_root
        apt update
        apt install -y gcc make libcurl4-openssl-dev libjson-c-dev
        ;;
    
    fedora|rhel|centos|rocky|almalinux)
        echo "Installing for Fedora-based system..."
        check_root
        dnf install -y gcc make libcurl-devel json-c-devel
        ;;
    
    opensuse*|sles)
        echo "Installing for openSUSE-based system..."
        check_root
        zypper install -y gcc make libcurl-devel libjson-c-devel
        ;;
    
    alpine)
        echo "Installing for Alpine Linux..."
        check_root
        apk add --no-cache gcc make curl-dev json-c-dev musl-dev
        ;;
    
    *)
        # Check if it's based on known distros
        if [[ "$DISTRO_LIKE" == *"debian"* ]] || [[ "$DISTRO_LIKE" == *"ubuntu"* ]]; then
            echo "Installing for Debian-like system..."
            check_root
            apt update
            apt install -y gcc make libcurl4-openssl-dev libjson-c-dev
        elif [[ "$DISTRO_LIKE" == *"arch"* ]]; then
            echo "Installing for Arch-like system..."
            check_root
            pacman -Sy --needed --noconfirm gcc make curl json-c
        elif [[ "$DISTRO_LIKE" == *"fedora"* ]] || [[ "$DISTRO_LIKE" == *"rhel"* ]]; then
            echo "Installing for Fedora-like system..."
            check_root
            dnf install -y gcc make libcurl-devel json-c-devel
        else
            echo -e "${RED}Unsupported distribution: $DISTRO${NC}"
            echo "Please install manually: gcc, make, libcurl-dev, libjson-c-dev"
            exit 1
        fi
        ;;
esac

echo -e "\n${GREEN}✓ Dependencies installed successfully!${NC}"
echo -e "You can now run: ${YELLOW}make${NC}"