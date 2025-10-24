import Foundation
import SpriteKit

let udMuted = "muted"
let udSession = "session"
let udRound = "round"
let udTotal = "total"
let udHiScore = "hiscore"

enum Mode: Int {
    case IntroMode
    case PlayMode
}

enum Layer: CGFloat {
    case stars
    case controls
    case player
    case ball
    case text = 2000
}

enum BallState: Int {
    case dead
    case alive
    case grow
    case shrink
    case move
}

enum Sfx: Int {
    case buttonPress
    case explosion
    case fire
    case roundCompleted
    case startNewRound
    case yay
    case nay
    case missionAccomplished
}

enum IntroViewMode: Int {
    case intro
    case help
}
