""" eventor """
from eventor import Eventor

def handler(event, context):
    # pylint: disable=W0612,W0613
    """ Lambda function handler.
        Arguments:
            event - event from AWS
            context - execution context
    """
    Eventor().execute()

    return {
        'message': 'OK'
    }
