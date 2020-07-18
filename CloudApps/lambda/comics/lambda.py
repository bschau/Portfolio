""" comics """
from comics import Comics

def handler(event, context):
    # pylint: disable=W0612,W0613
    """ Lambda function handler.
        Arguments:
            event - event from AWS
            context - execution context
    """
    Comics().execute()

    return {
        'message': 'OK'
    }
