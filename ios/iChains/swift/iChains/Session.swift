import Foundation

// Persist these
var gdRound: Int = 0
var gdTotal: Int = 0
var gdHiScore: Int = 0

func sessionLoad() {
    let defaults = UserDefaults.standard
    let sessionExists = defaults.bool(forKey: udSession)
    if !sessionExists {
        sessionNew()
        return
    }
    
    gdRound = defaults.integer(forKey: udRound)
    gdTotal = defaults.integer(forKey: udTotal)
}

func sessionNew() {
    gdRound = 0
    gdTotal = 0
}

func sessionSave() {
    let defaults = UserDefaults.standard
    defaults.set(gdRound, forKey: udRound)
    defaults.set(gdTotal, forKey: udTotal)
    defaults.set(true, forKey: udSession)
}

func hiScoreLoad() {
    let defaults = UserDefaults.standard
    gdHiScore = defaults.integer(forKey: udHiScore)
}

func hiScoreSave() {
    let defaults = UserDefaults.standard
    defaults.set(gdHiScore, forKey: udHiScore)
}
