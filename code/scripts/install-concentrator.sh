#!/bin/bash

# Make the RPI get the hostname from the DHCP server
echo localhost | sudo tee /etc/hostname

# Update the system
sudo apt-get update
sudo apt-get upgrade -y

# Install additional software
sudo apt-get install vim tshark tcpdump git build-essential python3-pip -y

# Configure Vim editor
echo "set background=dark" >> ~/.vimrc
echo "syntax on" >> ~/.vimrc
echo "set number" >> ~/.vimrc

# Install PIP3 packages
pip3 install pymodbustcp numpy