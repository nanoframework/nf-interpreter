# NetXDuo
This folder contains a fork of NetXDuo that has been modified to utilize the [Inventek ISM43362](https://www.inventeksys.com/ism4336-m3g-l44-e-embedded-serial-to-wifi-module) WiFi module. The Inventek WiFi module has an integrated TCP/IP stack and communication with the device is through an AT command set.

Due to the integrated TCP/IP stack on the Inventek module, the TCP/IP stack has been removed from the NetXDuo layer and replaced with a redirection to the underlying AT command set. 

This is a demonstration of how the NetXDuo networking stack can be modified to work with radically different networking modules, while still providing a standardized interface layer.
