import SpriteKit
import GameplayKit

let colors: [SKColor] = [
    cRed,
    cGreen,
    cBlue,
    cYellow,
    cCyan,
    cPurple,
    cBrown,
    cMagenta,
    cOrange
]
let chainBonus = 10000
let ballRadius = 12.0
let ballMaxRadius = 50.0
let ballGrowSpeed = 3.0
let ballShrinkSpeed = 5.0
let ballSize = (2 * ballRadius)
let ballAlive = 45
let bfLeft: Int = pfLeft + Int(ballRadius)
let bfRight: Int = pfRight - Int(ballRadius)
let bfTop: Int = pfTop + Int(ballRadius)
let bfBottom: Int = pfBottom - Int(ballRadius)
let levels: [Level] = [
    Level(balls: 5, goal: 2),
    Level(balls: 10, goal: 4),
    Level(balls: 15, goal: 6),
    Level(balls: 20, goal: 8),
    Level(balls: 25, goal: 12),
    Level(balls: 30, goal: 17),
    Level(balls: 35, goal: 20),
    Level(balls: 40, goal: 24),
    Level(balls: 43, goal: 30),
    Level(balls: 47, goal: 37),
    Level(balls: 52, goal: 45),
    Level(balls: 55, goal: 51),
]
var balls: [Ball] = [Ball](repeating: Ball(), count: 70)
var ballsCount: Int = 0
var collisionDetectionEnabled: Bool = false
var longestChain: Int = 0
var goal: Int = 0
var roundScore: Int = 0
var roundChainBonus: Int = 0
var penalty: Int = 0
var roundCompleted: Bool = true
var missionAccomplished: Bool = false
var newHiScore: Bool = false

class PlayView {
    static func newGame() {
        gdRound = 1
        gdTotal = 0
        continueGame()
        PlayScreens.psFirstLevel()
    }
    
    static func continueGame() {
        missionAccomplished = false
        roundCompleted = true
        appMode = Mode.PlayMode
    }

    static func startGame() {
        removeAllComponents(view: parentScene)
        let player = Ball()
        player.fillColor = cWhite
        player.zPosition = Layer.player.rawValue
        balls[0] = player
        roundScore = 0
        longestChain = 0
        collisionDetectionEnabled = false

        ballsCount = levels[gdRound - 1].balls
        goal = levels[gdRound - 1].goal

        var colorIndex = 0
        for i in 1...ballsCount {
            let ball = Ball()
            ball.radius = Double(ballRadius)
            ball.fillColor = colors[colorIndex]
            let x = Int.random(in: bfLeft..<bfRight)
            let y = Int.random(in: bfTop..<bfBottom)
            ball.position = CGPoint(x: x, y: y)

            ball.speedX = getSpeed()
            ball.speedY = getSpeed()
            ball.zPosition = Layer.ball.rawValue
            balls[Int(i)] = ball
            parentScene.addChild(ball)

            colorIndex += 1
            if colorIndex >= colors.count {
                colorIndex = 0
            }
        }

        Hud.build()
        Hud.update()
        SoundManager.playSfx(sfx: Sfx.startNewRound)
    }
    
    static func getSpeed() -> Int {
        let speed = Int.random(in: 1..<4)
        if Int.random(in: 0..<50) > 25 {
            return -speed
        }
        
        return speed
    }
    
    static func touchesEnded(touches: Set<UITouch>, view: SKScene) {
        if let touch = touches.first {
            if missionAccomplished {
                SoundManager.playSfx(sfx: Sfx.buttonPress)
                IntroView.run()
                return
            }

            if roundCompleted {
                SoundManager.playSfx(sfx: Sfx.buttonPress)
                roundCompleted = false
                startGame()
                return
            }
            
            if collisionDetectionEnabled {
                return
            }

            SoundManager.playSfx(sfx: Sfx.fire)
            let location = touch.location(in: view)
            let player = balls[0]
            player.position = location
            parentScene.addChild(player)
            player.state = BallState.grow
            collisionDetectionEnabled = true
        }
    }

    static func update(view: SKScene) {
        if roundCompleted {
            return
        }
        
        playerAction()
        moveBalls()
        detectCollisions(view: view)
        detectGameFinished()
        Hud.blink()
    }
    
    static func playerAction() {
        let player = balls[0]
        if player.state == BallState.grow {
            growBall(ball: player)
        } else if player.state == BallState.alive {
            idleBall(ball: player)
        } else if player.state == BallState.shrink {
            shrinkBall(ball: player)
        }
    }
    
    static func growBall(ball: Ball) {
        if ball.radius < ballMaxRadius {
            ball.radius += ballGrowSpeed
            return
        }
        ball.radius = ballMaxRadius
        ball.idle = ballAlive
        ball.state = BallState.alive
    }
    
    static func idleBall(ball: Ball) {
        if ball.idle > 0 {
            ball.idle -= 1
            return
        }
        ball.state = BallState.shrink
    }
    
    static func shrinkBall(ball: Ball) {
        let r = ball.radius - ballShrinkSpeed
        
        ball.radius = r
        if r > 0.0 {
            return
        }
        ball.state = BallState.dead
    }
    
    static func moveBalls() {
        for i in 1...ballsCount {
            let ball = balls[Int(i)]
            if ball.state == BallState.move {
                moveBall(ball: ball)
            } else if ball.state == BallState.grow {
                growBall(ball: ball)
            } else if ball.state == BallState.alive {
                idleBall(ball: ball)
            } else if ball.state == BallState.shrink {
                shrinkBall(ball: ball)
            }
        }
    }

    static func moveBall(ball: Ball) {
        var x = Int(ball.position.x)
        var y = Int(ball.position.y)
        
        x += ball.speedX
        if x <= bfLeft {
            x = bfLeft
            ball.speedX = -ball.speedX
        } else if x >= bfRight {
            x = bfRight
            ball.speedX = -ball.speedX
        }
        
        y += ball.speedY
        if y <= bfTop {
            y = bfTop
            ball.speedY = -ball.speedY
        } else if y >= bfBottom {
            y = bfBottom
            ball.speedY = -ball.speedY
        }
        
        ball.position = CGPoint(x: x, y: y)
    }

    static func detectCollisions(view: SKScene) {
        if !collisionDetectionEnabled {
            return
        }
         
        for idx in 1...ballsCount {
            let ball = balls[Int(idx)]
            if ball.state != BallState.move || ball.parentBall != nil {
                continue
            }

            for i in 0...ballsCount {
                if i == idx {
                    continue
                }
                
                let tgt = balls[Int(i)]
                if tgt.state == BallState.dead || tgt.state == BallState.move {
                    continue
                }
                
                if !isInRange(lhs: tgt, rhs: ball) {
                    continue
                }

                ball.state = BallState.grow
                ball.parentBall = tgt
                
                ball.chainDepth = 1
                var tmp = ball.parentBall
                while tmp?.parentBall != nil {
                    ball.chainDepth += 1
                    tmp = tmp?.parentBall
                }

                if ball.chainDepth > longestChain {
                    longestChain = ball.chainDepth
                }
                let z = ball.chainDepth + 1
                var sc = (z * z * z) * 100
                if balls[0].state == BallState.dead {
                    sc += sc / 2
                }
                
                goal -= 1
                roundScore += sc
                let lbl = addLabel(color: cWhite,
                                   text: String(sc),
                                   size: 24,
                                   x: Int(ball.position.x),
                                   y: Int(ball.position.y))
                
                let up = SKAction.sequence([
                    SKAction.moveTo(y: ball.position.y + 70, duration: 0.5),
                    SKAction.fadeOut(withDuration: 0.5)
                ])
                up.timingMode = SKActionTimingMode.easeOut
                let blink = SKAction.customAction(withDuration: 1.0) {
                    node, elapsedTime in
                    
                    if Int(elapsedTime) % 3 == 0 {
                        if let node = node as? SKLabelNode {
                            let c = Int.random(in: 0..<colors.count)
                            node.fontColor = colors[c]
                        }
                    }
                }
                let group = SKAction.group([up, blink])
                let all = SKAction.sequence([
                    group,
                    SKAction.removeFromParent()
                ])
                lbl.run(all)
                Hud.update()
                SoundManager.playSfx(sfx: Sfx.explosion)
            }
        }
    }

    static func isInRange(lhs: Ball, rhs: Ball) -> Bool {
        let x = lhs.position.x - rhs.position.x
        let y = lhs.position.y - rhs.position.y
        let l = sqrt((x * x) + (y * y))
        return (l - lhs.radius - rhs.radius) <= 0.0
    }
        
    static func detectGameFinished() {
        if balls[0].state != BallState.dead {
            return
        }
        
        for idx in 1...ballsCount {
            let ball = balls[Int(idx)]
            if ball.state == BallState.alive ||
                ball.state == BallState.grow ||
                ball.state == BallState.shrink {
                return
            }
        }

        sessionSave()
        removeAllComponents(view: parentScene)
        roundCompleted = true
        roundChainBonus = chainBonus * longestChain * gdRound
        
        if goal > 0 {
            SoundManager.playSfx(sfx: Sfx.nay)
            penalty = (gdTotal + roundScore + roundChainBonus) / 2
            if penalty < gdTotal {
                gdTotal -= penalty
            } else {
                gdTotal = 0
            }
            sessionSave()
            PlayScreens.psLevelFailed()
            return
        }
        
        gdTotal += roundScore
        gdTotal += roundChainBonus

        if gdRound < levels.count {
            SoundManager.playSfx(sfx: Sfx.yay)
            gdRound += 1
            sessionSave()
            PlayScreens.psLevelCompleted()
            return
        }
        
        SoundManager.playSfx(sfx: Sfx.missionAccomplished)
        newHiScore = gdTotal > gdHiScore
        if newHiScore {
            gdHiScore = gdTotal
            hiScoreSave()
        }
        
        missionAccomplished = true
        PlayScreens.psMissionAccomplished()
        sessionNew()
        sessionSave()
    }
}
