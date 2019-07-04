#!/usr/bin/python3

import argparse
import math
import os

SUB = [0, 1, 1, 0, 1, 0, 1, 0]
N_B = 32
N = 8 * N_B


# reversed step function
def reverse_step(keystream):
    # indices of bits '1' in SUB array
    one_indices = [1, 2, 4, 6]
    # indices of bits '0' in SUB array
    zero_indices = [0, 3, 5, 7]

    # store possibilities for rightmost bit of keystream
    res = zero_indices
    if keystream & 1:
        res = one_indices

    for i in range(1, N):
        res_tmp = []
        # store possibilities for following bit of keystream
        possible_indices = zero_indices
        if (keystream >> i) & 1:
            possible_indices = one_indices

        for res_index in res:
            for possible_index in possible_indices:
                # store four possible key parts
                if possible_index & 3 == (res_index >> i) & 3:
                    res_tmp.append(res_index | (possible_index << i))
        # store possible four keys
        res = res_tmp

    return res


# function defined in super_cipher.py.enc file
def step(x):
    x = (x & 1) << N+1 | x << 1 | x >> N-1
    y = 0
    for i in range(N):
        y |= SUB[(x >> i) & 7] << i
    return y


# decipher gif file using the first part of keystream
def decipher_gif(dir, key):
    # read hint.gif.enc file
    hint = open(os.path.join(dir, 'hint.gif.enc'), 'rb')
    hint_content = bytearray(hint.read())

    # get whole key using block key and step function that generates following block keys
    whole_key = b''
    block_key = int.from_bytes(key, 'little')
    for i in range(0, math.ceil(len(hint_content) / N_B)):
        whole_key += block_key.to_bytes(N_B, byteorder='little')
        block_key = step(block_key)

    # create hint.gif file and write to it deciphered file hint.git.enc
    gif_file = open('hint.gif', 'wb')
    for i in range(0, len(hint_content)):
        gif_file.write(bytes([hint_content[i] ^ whole_key[i]]))
    gif_file.close()


# decipher py file using the first part of keystream
def decipher_py(dir, key):
    # read super_cipher.py.enc file
    super_cipher = open(os.path.join(dir, 'super_cipher.py.enc'), 'rb')
    super_cipher_content = bytearray(super_cipher.read())

    # get whole key using block key and step function that generates following block keys
    whole_key = b''
    block_key = int.from_bytes(key, 'little')
    for i in range(0, math.ceil(len(super_cipher_content) / N_B)):
        whole_key += block_key.to_bytes(N_B, byteorder='little')
        block_key = step(block_key)

    # print whole deciphered file super_cipher.py.enc
    for i in range(0, len(super_cipher_content)):
        print(chr(super_cipher_content[i] ^ whole_key[i]), end='')


if __name__ == '__main__':
    # process arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("dir_path")
    args = parser.parse_args()

    if not os.path.isdir(args.dir_path):
        raise ValueError("path to directory does not exist")

    # read bis.txt file
    plain = open(os.path.join(args.dir_path, 'bis.txt'), 'rb')
    plaintext = bytearray(plain.read())
    # print(len(plaintext))

    # read bis.txt.enc file
    cipher = open(os.path.join(args.dir_path, 'bis.txt.enc'), 'rb')
    ciphertext = bytearray(cipher.read())
    # print(len(ciphertext))

    # get keystream using XOR of plaintext and ciphertext
    keystream = bytearray(len(plaintext))
    for i in range(0, len(plaintext)):
        keystream[i] = plaintext[i] ^ ciphertext[i]

    # decipher_py(args.dir_path, keystream[:32])
    # decipher_gif(args.dir_path, keystream[:32])

    keystream = int.from_bytes(keystream, 'little')

    # select one valid key from four possible keys
    for i in range(0, N // 2):
        for res in reverse_step(keystream):
            if res & (3 << N) == (res & 3) << N:
                keystream = (res >> 1) & ~(1 << N)
                break

    # print key
    key = keystream.to_bytes(N_B, 'little').decode()
    for i in key:
        if i.isprintable():
            print(i, end='')
