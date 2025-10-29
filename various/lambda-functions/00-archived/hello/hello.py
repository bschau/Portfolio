""" hello """
import time
from sendmail import SendMail


def handler(event, context):
    # pylint: disable=W0612,W0613
    """ Lambda function handler.
        Arguments:
            event - event from AWS
            context - execution context
    """
    Hello().execute()

    return {
        'message': 'OK'
    }


class Hello():
    """ Hello (world) backbone. """

    def execute(self):
        """ Hello runner. """
        now = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
        title = f"Hello {now}"
        page = self.head(title)
        page = f"{page}<html><body><p>Hello from AWS</p></body></html>"
        SendMail().deliver(title, page)


    @staticmethod
    def head(title):
        """ Get HTML head (of page).
            Arguments:
                title - title.
        """
        return f"""<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
    <title>{title}</title>
</head>"""


if __name__ == '__main__':
    Hello().execute()
