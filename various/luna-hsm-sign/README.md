# LunaHSMSign

A small demo to show how to sign a PDF using a Luna HSM from within C#.

The Luna Safenet HSM can be a bitch to address when using C#. You have to acknowledge, that you're keeping the public part of the certificate in your certificate store. The private key is stored on the HSM (as it must be!). Also, the drivers are 64-bits only.

Thus:

* Make sure that the user has access to the private key. Open mmc, import the Certificates snap-in for the local computer and add the access rights under Manage private key menu item.
* Make sure that you're compiling for 64 bits in Visual Studio.
* If you run your signing app under IIS make sure that the App-pool is enabled for 64 bits support.


Requirements

An old version (5.5.13) of iTextSharp is needed. If you updated this sample to a later version of iTextSharp then please send me a pull request.

