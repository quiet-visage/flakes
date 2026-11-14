#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# Function to detect Linux distribution and install packages accordingly
{
    if [ -f /etc/arch-release ]; then
	echo "Detected Arch Linux or derivative"
	"$SCRIPT_DIR/distros/arch.sh"
    elif [ -f /etc/debian_version ]; then
	echo "Detected Debian/Ubuntu or derivative"
	"$SCRIPT_DIR/distros/debian.sh"
    elif [ -f /etc/redhat-release ]; then
	echo "Detected Fedora/CentOS"
	"$SCRIPT_DIR/distros/fedora.sh"
    elif [ -f /etc/gentoo-release ]; then
	echo "Detected Gentoo"
	"$SCRIPT_DIR/distros/gentoo.sh"
    elif [ -d /etc/xbps.d/ ]; then
	echo "Detected Void Linux"
	"$SCRIPT_DIR/distros/void.sh"
    else
	echo "Unsupported Linux distribution."
	exit 1
    fi
}
