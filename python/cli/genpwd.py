#!/usr/bin/env python3
""" genpwd - generate a password. """
import random
import sys


def get_letters(letters):
    """ Get the letters in the password.
        Args:
            letters - number of letters in password.
    """
    consonants = "bcdfghjklmnpqrstvwxyz"
    vocals = "aeiou"
    alphas = int(letters / 2)
    destination = ''
    while alphas > 0:
        destination = destination + random.choice(consonants)
        destination = destination + random.choice(vocals)
        alphas = alphas - 1

    if letters % 2 == 1:
        destination = destination + random.choice(vocals)

    return destination


def get_digits(digits):
    """ Get the digits in the password.
        Args:
            digits - number of digits in password.
    """
    numbers = "0123456789"
    destination = ''
    while digits > 0:
        destination = destination + random.choice(numbers)
        digits = digits - 1

    return destination


def capitalize_random(letters):
    """ Uppercase one random character.
        Args:
            letters - source string.
    """
    index = random.randrange(len(letters))
    return letters[:index] + letters[index].upper() + letters[index + 1:]


def normalize(letters):
    """ Turn one or two letters to uppercase.
        Args:
            letters - source string.
    """
    if len(letters) > 4:
        letters = capitalize_random(letters)

    return capitalize_random(letters)


def runner(letters, digits):
    """ Create the password.
        Args:
            letters - number of letters in password.
            digits - number of digits in password.
    """
    lhs = normalize(get_letters(letters))
    rhs = get_digits(digits)
    special = random.choice("!#%&/()=?@£${[]}+-")
    structure = [ lhs, special, rhs ]
    random.shuffle(structure)
    return f"{structure[0]}{structure[1]}{structure[2]}"


if __name__ == "__main__":
    LETTERS = 8
    DIGITS = 3

    ARG_CNT = len(sys.argv)
    if ARG_CNT < 1 or ARG_CNT > 3:
        print("Usage: genpwd.py [letters [digits]]")
        print("Defaults:")
        print(f"  letters:    {LETTERS}")
        print(f"  digits:     {DIGITS}")
        sys.exit(0)

    if ARG_CNT == 2:
        LETTERS = int(sys.argv[1])
    elif ARG_CNT == 3:
        LETTERS = int(sys.argv[1])
        DIGITS = int(sys.argv[2])

    random.seed(None)
    print(runner(LETTERS, DIGITS))
