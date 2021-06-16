#!/bin/bash

sudo apt install -y qemu-kvm libvirt-daemon-system bridge-utils virtinst


if ! systemctl is-active --quiet libvirtd; then
	systemctl enable libvirtd
	systemctl start libvirtd
fi
exit 0
