package main

import (
	"image"
	_ "image/jpeg"
	_ "image/png"
	"log"
	"os"
)

// GetSize returns size of image
func GetSize(imagePath string) (int, int) {
	file, err := os.Open(imagePath)
	if err != nil {
		log.Panicln(err)
	}

	image, _, err := image.DecodeConfig(file)
	if err != nil {
		log.Panicln(imagePath, err)
	}
	return image.Width, image.Height
}
