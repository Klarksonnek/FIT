#!/usr/bin/python3

import argparse
from z3 import *

SUB = [0, 1, 1, 0, 1, 0, 1, 0]
N_B = 32
N = 8 * N_B


# get key as integer from SAT model
def get_key(model, variables):
    key = 0

    for i in range(0, N):
        if model[variables[i]]:
            key |= 1 << i

    first_bit = (key & 1)
    return (first_bit << (N - 1)) | (key >> 1)


# reversed step function
def reverse_step(keystream):
    # create array of boolean variables
    variables = []
    for i in range(0, N):
        variables.append(Bool('var%s' % i))
    variables += variables[:2]

    # create solver
    solver = Solver()
    for i in range(0, N):
        # get leftmost bit
        bit = (keystream >> i) & 1
        var0 = variables[i]
        var1 = variables[i + 1]
        var2 = variables[i + 2]

        if bit:
            res = Or(And(Not(var2), Not(var1), var0), And(Not(var0), Or(var1, var2)))

        else:
            res = Or(And(Not(var2), Not(var1), Not(var0)), And(var0, Or(var1, var2)))
        # store list of CNF clauses corresponding with bit '0' or bit '1'
        solver.add(res)

    # check if solution exists
    if solver.check() == z3.sat:
        return get_key(solver.model(), variables)
    else:
        raise ValueError("solution was not found")


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

    keystream = int.from_bytes(keystream, 'little')

    # get key
    for i in range(0, N // 2):
        keystream = reverse_step(keystream)

    # print key
    key = keystream.to_bytes(N_B, 'little').decode()
    for i in key:
        if i.isprintable():
            print(i, end='')
