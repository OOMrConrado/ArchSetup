#!/bin/bash

# Local installation script for ArchSetup
# Does not require administrator permissions

# Create bin directory in home if it doesn't exist
mkdir -p ~/bin

# Copy the executable
cp sysmon ~/bin/ArchSetup

# Make it executable
chmod +x ~/bin/ArchSetup

# Add ~/bin to PATH if not already there
if ! echo $PATH | grep -q "$HOME/bin"; then
    echo 'export PATH="$HOME/bin:$PATH"' >> ~/.bashrc
    echo "Added ~/bin to PATH in ~/.bashrc"
    echo "Run 'source ~/.bashrc' or restart the terminal to use the 'ArchSetup' command"
else
    echo "~/bin is already in your PATH"
fi

echo ""
echo "✅ ArchSetup System Monitor installed successfully!"
echo ""
echo "Usage:"
echo "  ArchSetup              - Show all information"
echo "  ArchSetup --watch      - Continuous monitor mode"
echo "  ArchSetup --cpu        - CPU information only"
echo "  ArchSetup --memory     - Memory information only"
echo "  ArchSetup --help       - Show complete help"
echo ""
echo "If this is the first installation, run: source ~/.bashrc"