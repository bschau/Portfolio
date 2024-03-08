module main

go 1.15

require internal/common v1.0.0
replace internal/common => ./internal/common
require internal/comics v1.0.0

replace internal/comics => ./internal/comics

require internal/lichess v1.0.0

replace internal/lichess => ./internal/lichess

require (
	github.com/go-gomail/gomail v0.0.0-20160411212932-81ebce5c23df
	github.com/sendgrid/rest v2.6.2+incompatible // indirect
	github.com/sendgrid/sendgrid-go v3.7.2+incompatible
	internal/newsfeed v1.0.0
)

replace internal/newsfeed => ./internal/newsfeed
