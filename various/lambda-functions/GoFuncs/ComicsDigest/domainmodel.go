package main

// SendgridRc is the sendgrid configuration
type SendgridRc struct {
	From     string
	FromName string
	To       string
	ToName   string
	APIKey   string
}
