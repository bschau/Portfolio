import Foundation
import SpriteKit

let GameFont: String = "Speed Rush"
let cLightGray: SKColor = SKColor(red: 0.83, green: 0.83, blue: 0.83, alpha: 1.0)
let cDarkGray: SKColor = SKColor(red: 0.25, green: 0.25, blue: 0.25, alpha: 1.0)
let cBlack: SKColor = SKColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.0)
let cWhite: SKColor = SKColor(red: 1.0, green: 1.0, blue: 1.0, alpha: 1.0)
let cGreen: SKColor = SKColor(red: 0.47, green: 0.87, blue: 0.47, alpha: 1.0)
let cYellow: SKColor = SKColor(red: 1.0, green: 0.98, blue: 0.63, alpha: 1.0)
let cBlue: SKColor = SKColor(red: 0.65, green: 0.78, blue: 0.91, alpha: 1.0)
let cRed: SKColor = SKColor(red: 0.98, green: 0.63, blue: 0.63, alpha: 1.0)
let cCyan: SKColor = SKColor(red: 0.64, green: 0.85, blue: 0.85, alpha: 1.0)
let cPurple: SKColor = SKColor(red: 0.76, green: 0.69, blue: 0.88, alpha: 1.0)
let cBrown: SKColor = SKColor(red: 0.51, green: 0.41, blue: 0.33, alpha: 1.0)
let cMagenta: SKColor = SKColor(red: 0.96, green: 0.6, blue: 0.76, alpha: 1.0)
let cOrange: SKColor = SKColor(red: 0.97, green: 0.78, blue: 0.6, alpha: 1.0)

func removeAllComponents(view: SKScene) {
    for child in view.children {
        if child.name == "stars" || child.name == "music" {
            continue
        }

        child.removeFromParent()
    }
}

func addLabel(color: SKColor, text: String, size: CGFloat, x: Int, y: Int, addToParent: Bool = true) -> SKLabelNode {
    let lbl = SKLabelNode()
    lbl.fontName = GameFont
    lbl.fontColor = color
    lbl.fontSize = size
    lbl.horizontalAlignmentMode = .center
    lbl.verticalAlignmentMode = .center
    lbl.zPosition = Layer.text.rawValue
    lbl.text = text
    lbl.position = CGPoint(x: x, y: y)
    lbl.alpha = 1.0
    if addToParent {
        parentScene.addChild(lbl)
    }
    return lbl
}
