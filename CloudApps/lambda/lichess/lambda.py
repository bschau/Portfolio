""" lichessdigest """
from lichess import Lichess

def handler(event, context):
    # pylint: disable=W0612,W0613
    """ Lambda function handler.
        Arguments:
            event - event from AWS
            context - execution context
    """
    Lichess().execute()

    return {
        'message': 'OK'
    }
