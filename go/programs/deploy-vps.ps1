$env:GOOS="linux"
go.exe build -ldflags '-s'
pscp -P 22 -i $HOME\.ssh\id_rsa.ppk ComicsDigest "$($env:VPSAPPSACCOUNT):/home/bs/Apps"