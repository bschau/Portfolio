package newsfeed

import (
	"encoding/gob"
	"log"
	"os"
	"time"
)

const cutOff int = 60 * 60 * 24 * 365 * 1_000_000_000

var cacheFile string
var timestamp time.Time
var cache map[string]time.Time

// CacheInit Initialize cache variables
func CacheInit(file string, now time.Time) {
	cacheFile = file
	timestamp = now
}

// CacheLoad Load or initialize cache
func CacheLoad() {
	decodeFile, err := os.Open(cacheFile)
	if err != nil {
		log.Println(err)
		cache = make(map[string]time.Time)
		return
	}
	defer decodeFile.Close()

	decoder := gob.NewDecoder(decodeFile)

	if err = decoder.Decode(&cache); err != nil {
		panic(err)
	}
}

// CacheSave Saves cache
func CacheSave() {
	encodeFile, err := os.Create(cacheFile)
	if err != nil {
		panic(err)
	}
	defer encodeFile.Close()

	encoder := gob.NewEncoder(encodeFile)
	if err := encoder.Encode(cache); err != nil {
		panic(err)
	}
}

// CacheHas Do key exist in cache?
func CacheHas(key string) bool {
	_, exists := cache[key]
	return exists
}

// CacheAdd Add key to cache
func CacheAdd(key string) {
	cache[key] = timestamp
}

// CachePrune Prune cache
func CachePrune() {
	duration := time.Duration(cutOff)
	for key, value := range cache {
		if value.Add(duration).After(timestamp) {
			continue
		}

		delete(cache, key)
	}
}
