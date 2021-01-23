Write-Host "Updating build-lambda-zip"
go get github.com/aws/aws-lambda-go/cmd/build-lambda-zip

Write-Host "Building lambda function"
$env:GOOS="linux"
$env:CGO_ENABLED="0"
$env:GOARC="amd64"
go build -o main -ldflags '-s'
Remove-Item Env:\GOOS
Remove-Item Env:\CGO_ENABLED
Remove-Item Env:\GOARC

Write-Host "Creating zip file for deployment"
copy $HOME/.sendgridrc sendgridrc
build-lambda-zip -output main.zip main sendgridrc

Write-Host "Deploying"
aws lambda update-function-code --function-name=GoSample --zip-file=fileb://main.zip

Write-Host "Cleaning up"
del main.zip
go clean
