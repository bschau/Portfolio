import SpriteKit
import GameplayKit

let scoreLbl: SKLabelNode = SKLabelNode()
let ballsLbl: SKLabelNode = SKLabelNode()
let blinkingBalls: [SKLabelNode] = [
    SKLabelNode(),
    SKLabelNode(),
    SKLabelNode(),
    SKLabelNode(),
    SKLabelNode(),
    SKLabelNode()
]
let hudFontSize: CGFloat = 32
var blinkInited: Bool = false
var blinkIndex: Int = -1
var blinkDelay: Int = 0

class Hud {
    static func build() {
        blinkInited = false
        blinkIndex = -1
        blinkDelay = 0

        let hudY = Int(pfBottom) - Int(hudFontSize)
 
        ballsLbl.horizontalAlignmentMode = .left
        ballsLbl.text = "\u{2b24} " + String(goal)
        ballsLbl.position = CGPoint(x: pfLeft, y: hudY)

        scoreLbl.horizontalAlignmentMode = .right
        scoreLbl.text = "0"
        scoreLbl.position = CGPoint(x: pfRight, y: hudY)

        addLbl(lbl: ballsLbl)
        addLbl(lbl: scoreLbl)
        
        var x = pfLeft
        for i in 0..<blinkingBalls.count {
            let lbl = blinkingBalls[Int(i)]

            lbl.horizontalAlignmentMode = .left
            lbl.text = "\u{2b24}"
            lbl.position = CGPoint(x: x, y: hudY)
            lbl.fontName = GameFont
            lbl.fontColor = cLightGray
            lbl.fontSize = hudFontSize
            lbl.zPosition = Layer.text.rawValue
            lbl.verticalAlignmentMode = .baseline
            lbl.alpha = 0.0
            parentScene.addChild(lbl)
            x += Int(hudFontSize + 2)
        }
    }
    
    static func addLbl(lbl: SKLabelNode) {
        lbl.fontName = GameFont
        lbl.fontColor = cLightGray
        lbl.fontSize = hudFontSize
        lbl.zPosition = Layer.text.rawValue
        lbl.verticalAlignmentMode = .baseline
        lbl.alpha = 1.0
        parentScene.addChild(lbl)
    }
    
    static func update() {
        scoreLbl.text = String(gdTotal + roundScore)

        if goal > 0 {
            if goal == 1 {
                ballsLbl.text = "\u{2b24} Last"
                return
            }
            
            ballsLbl.text = "\u{2b24} " + String(goal)
            return
        }
        
        if blinkInited {
            return
        }
        
        ballsLbl.removeFromParent()
        SoundManager.playSfx(sfx: Sfx.roundCompleted)
        for i in 0..<blinkingBalls.count {
            let lbl = blinkingBalls[Int(i)]
            lbl.alpha = 1.0
        }
            
        blinkInited = true
        blinkDelay = 0
        blink()
    }
    
    static func blink() {
        if !blinkInited {
            return
        }
        
        blinkDelay -= 1
        if blinkDelay > 0 {
            return
        }
        blinkDelay = 8

        blinkIndex += 1
        if blinkIndex >= colors.count {
            blinkIndex = 0
        }

        var index = blinkIndex
        for i in 0..<blinkingBalls.count {
            let lbl = blinkingBalls[Int(i)]
            lbl.fontColor = colors[index]
            index += 1
            if index >= colors.count {
                index = 0
            }
        }
    }
}
