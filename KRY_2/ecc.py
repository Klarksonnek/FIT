#!/usr/bin/python3

import argparse

F_P = 0x17
A = 0x01
B = 0x01
P_X = 0xD
P_Y = 0x10


# get inverse number
# source: https://www.johannes-bauer.com/compsci/ecc/, modified function eea()
def getInvNum(j):
    i = F_P
    j = j % F_P
    (s, t, u, v) = (1, 0, 0, 1)
    while j != 0:
        (q, r) = (i // j, i % j)
        (unew, vnew) = (s, t)
        s = u - (q * s)
        t = v - (q * t)
        (i, j) = (j, r)
        (u, v) = (unew, vnew)

    return u % F_P


# add two points
# source: Elipticke krivky a sifrovani (see web page of KRY)
def add(B_x, B_y, x, y):
    # if points are inverse, special processing is required
    if B_x == x and B_y == ((-y) % F_P):
        return 0, 0
    if B_x == x and B_y == y:
        s = ((3 * (B_x ** 2) + A) * getInvNum(2 * B_y)) % F_P
    else:
        s = ((y - B_y) * getInvNum(x - B_x)) % F_P

    x_r = s ** 2 - B_x - x
    y_r = s * (B_x - x_r) - B_y
    return x_r % F_P, y_r % F_P


def main():
    # process arguments
    argument_parser = argparse.ArgumentParser()
    argument_parser.add_argument('public_key')
    args = argument_parser.parse_args()

    # store public key (point)
    public_key = args.public_key.split(',')
    Q_x = int(str(public_key[0]).strip()[1:], 16)
    Q_y = int(str(public_key[1]).strip()[:-1], 16)

    # search for key
    k = 1
    R_x = P_X
    R_y = P_Y
    while True:
        # if coordinates of two points equal, print key
        if R_x == Q_x and R_y == Q_y:
            break

        R_x, R_y = add(P_X, P_Y, R_x, R_y)
        k += 1

    print(k, end='')


if __name__ == '__main__':
    main()
