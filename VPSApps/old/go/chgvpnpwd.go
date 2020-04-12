package main

func chgvpnpwd(configuration RCFile) {
	Start("Change VPN Passwords")
	builder.WriteString("<p>Now is time to change the VPN passwords.</p>")
	builder.WriteString("<p>Also, change the Office 365 account password.</p>")
	End()
	Deliver(configuration, Title, ToString())
}
