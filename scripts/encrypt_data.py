# -----------------------------------------------------------------------------
# File: encrypt_data.py
# Author: PiniponSelvagem (Rodrigo Silva)
# Description: Encrypts sensitive data using AES-128-CBC and outputs base64-encoded ciphertext
# Created: 2025-05-06
# Usage: python encrypt_data.py
# Dependencies: pycryptodome
# -----------------------------------------------------------------------------

"""
This python script is intented to be run once, and its output copied to the expected place
in the controllers code.
It encrypts the MQTT user/password, etc, and prints it to the terminal.
Then that encrypted data should be used in the controller during compile time in 'defines'.
During run time, the controller should decrypt that 'data' and use it as usual.

This little dance is to avoid the MQTT user/password and other sensitive data to be in
plain text in the firmware binary.
Adds a layer to the difficulty of getting this sensitive data from someone that get their
hands on the firmware binary and looks for the .rodata section of the assembly code.
"""

from Crypto.Cipher import AES
from Crypto.Util.Padding import pad
import hashlib

#################################################################################
# TODO: place this in a separate file and add it to .gitignore
#
passphrase = "PiniponSelvagem"  # User-provided secret used to derive the AES key
iv = b"xptoxptoxptoxpto"        # IV, must be 16 bytes
#
# --- DATA TO ENCRYPT ---
plainData = {
    "MQTT_USER": b"user",
    "MQTT_PASS": b"pass",
    "OTA_USER":  b"user",
    "OTA_PASS":  b"pass"
}
#################################################################################

def print_c_array(name, size, data: bytes):
    print(f"const uint8_t {name}[{size}] = {{ " + ', '.join(f'0x{b:02X}' for b in data) + " };")


# --- DERIVE AES-128 KEY FROM PASSPHRASE ---
# Hash the passphrase using SHA-256, then take first 16 bytes for AES-128
key = hashlib.sha256(passphrase.encode()).digest()[:16]

# --- ENCRYPT ---
print(" --- C arrays with KEY and IV ---")
print_c_array("AES_KEY", len(key), key)
print_c_array("AES_IV", len(iv), iv)
print()

print(" --- C defines with encrypted data ---")
for label, value in plainData.items():
    cipher = AES.new(key, AES.MODE_CBC, iv)
    ct_bytes = cipher.encrypt(pad(value, AES.block_size))
    label_size = f"{label}_SIZE"
    print(f"#define {label}_SIZE {len(ct_bytes)}")
    print_c_array(label, label_size, ct_bytes)
    print()
