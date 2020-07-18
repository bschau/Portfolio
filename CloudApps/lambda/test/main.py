""" Main - so we can run from command line. """
from test import Test

if __name__ == '__main__':
    RESULT = Test().execute()
    print(RESULT)
