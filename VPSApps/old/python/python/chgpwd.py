""" chgpwd """
from vpsutils import VpsUtils


def main():
    """ Main entry method. """
    vps = VpsUtils('Change passwords')
    vps.html_add("<p>Now is time to change the VPN passwords.</p>")
    vps.html_add("<p>Also, change the Office 365 account password.</p>")
    vps.deliver()


if __name__ == '__main__':
    main()
