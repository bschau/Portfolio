""" skakblad """
import filecmp
import os.path
import shutil
import urllib.request
from vpsutils import VpsUtils


def save_remote_file(url, destination):
    """ Get remote file from url and save to disk.
        Args:
            url: url to get.
            destination: destination.
    """
    remote_file = urllib.request.urlopen(url)
    data = remote_file.read()
    with open(destination, "wb") as code:
        code.write(data)

    return data


def main():
    """ Main entry. """
    vps = VpsUtils('Skakblad')
    url = 'http://www.skak.dk/images/skakbladet/seneste.pdf'
    remote_magazine = os.path.join(vps.tmp, "skakblad-remote.pdf")
    current_magazine_name = "{0}{1}".format(vps.prefix, "skakblad-current.pdf")
    current_magazine = os.path.join(vps.home, current_magazine_name)

    data = save_remote_file(url, remote_magazine)

    if os.path.isfile(current_magazine):
        if filecmp.cmp(remote_magazine, current_magazine):
            try:
                os.remove(remote_magazine)
            except:
                pass

            return

    shutil.move(remote_magazine, current_magazine)
    vps.deliver(data, "skakblad.pdf")


if __name__ == '__main__':
    main()
