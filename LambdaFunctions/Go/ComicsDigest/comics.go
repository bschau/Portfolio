package main

func comics(title string) {
	HTMLInit(title)

	cOrigin := "class=\"fancybox\""
	cToken := "img src"
	ckOrigin := ""
	ckToken := "data-comic-image-url"

	Generic(cOrigin, cToken, "Andy Capp", "https://www.creators.com/read/andy-capp")
	Generic(cOrigin, cToken, "B.C.", "https://www.creators.com/read/bc")
	Dilbert()
	Generic(ckOrigin, ckToken, "Hagar", "https://www.comicskingdom.com/hagar-the-horrible/")
	Generic(ckOrigin, ckToken, "Zits", "https://www.comicskingdom.com/zits")
}
