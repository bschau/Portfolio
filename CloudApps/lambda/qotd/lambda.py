""" qotd """
from qotd import Qotd

def handler(event, context):
    # pylint: disable=W0612,W0613
    """ Lambda function handler.
        Arguments:
            event - event from AWS
            context - execution context
    """
    Qotd().execute()

    return {
        'message': 'OK'
    }
