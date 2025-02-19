#
#  Title:  generate_aes_key_hex_string.py
#  Author: Shuichiro Endo
#

from Crypto.Random import get_random_bytes


def generate_key():
    return get_random_bytes(32)

def generate_iv():
    return get_random_bytes(16)

key = generate_key()
iv = generate_iv()

print("aes-256-cbc")
print("key(32 bit): " + key.hex())
print("iv (16 bit): " + iv.hex())
