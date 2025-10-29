import SpriteKit
//import GameplayKit
import Foundation

var introViewMode: IntroViewMode = IntroViewMode.intro

class IntroView {
    static let logoY = 200
    static var muteButton: SKSpriteNode = SKSpriteNode()
    static var helpButton: SKSpriteNode = SKSpriteNode()
    static let audioOnButton: SKTexture = SKTexture(imageNamed: "audioon")
    static let audioOffButton: SKTexture = SKTexture(imageNamed: "audiooff")
    static let helpTexture: SKTexture = SKTexture(imageNamed: "help")
    static let gradientTexture: SKTexture = SKTexture(imageNamed: "gradient")
    static var gradient: [SKColor] = []
    static var colorIndex: [Int] = [ 0, 0, 0, 0, 0, 0, 0 ]
    static var colorDelay: Int = 0
    static var blingBling: Bool = false
    static var letters: [SKLabelNode] = []
    
    static func run() {
        let ys: [Int] = [ pfTop + 48, pfTop + 48, 50, 10, -18, -68, -108, -168, logoY, logoY, logoY, logoY, logoY, logoY, logoY ]
        var lbls: [SKNode] = []

        appMode = Mode.IntroMode
        introViewMode = IntroViewMode.intro
        removeAllComponents(view: parentScene)
        lbls.removeAll()
        letters.removeAll()

        var cd = -24
        for i in 0..<7 {
            colorIndex[Int(i)] = cd
            cd += 3
        }
        blingBling = false
        buildMuteButton(y: ys[0])
        buildHelpButton(y: ys[1])
        lbls.append(muteButton)
        lbls.append(helpButton)
        lbls.append(ivAddLabel(color: cOrange, text: "By Schau / Usher", y: ys[2]))
        lbls.append(ivAddLabel(color: cPurple, text: "Sounds: Pixabay", y: ys[3]))
        lbls.append(ivAddLabel(color: cPurple, text: "Font: Speed Rush", y: ys[4]))
        lbls.append(ivAddLabel(color: cLightGray, text: "Hi score", y: ys[5]))
        lbls.append(ivAddLabel(color: cCyan, text: "\(gdHiScore)", y: ys[6]))
        lbls.append(ivAddLabel(color: cGreen, text: "Tap to play", y: ys[7]))

        letters.append(addLetter(text: "I"))
        letters.append(addLetter(text: "C"))
        letters.append(addLetter(text: "H"))
        letters.append(addLetter(text: "A"))
        letters.append(addLetter(text: "I"))
        letters.append(addLetter(text: "N"))
        letters.append(addLetter(text: "S"))

        lbls += letters

        var width = 0.0
        for i in 0..<7 {
            let lbl = letters[Int(i)]
            width += lbl.frame.width
        }

        width += 6 * 8.0    // space between letters, 8 pixels
        var xPos = (Double(pfWidth) - width) / 2
        xPos += Double(pfLeft)
        for i in 0..<7 {
            let lbl = letters[Int(i)]
            lbl.position.x = xPos
            xPos += lbl.frame.width + 8.0
        }

        let endNode = SKSpriteNode()
        endNode.position = CGPoint(x: 0, y: logoY + pfHeight)
        endNode.size = CGSize(width: 0, height: 0)
        let endNodeAction = SKAction.moveTo(y: CGFloat(logoY), duration: 2.0)
        endNodeAction.timingMode = SKActionTimingMode.easeInEaseOut
        parentScene.addChild(endNode)
        
        for i in 0..<lbls.count {
            let lbl = lbls[Int(i)]
            parentScene.addChild(lbl)
            let action = SKAction.moveTo(y: CGFloat(ys[Int(i)]), duration: 0.6 + (CGFloat(i) * 0.07))
            action.timingMode = SKActionTimingMode.easeInEaseOut
            lbl.run(action)
        }
        
        endNode.run(endNodeAction, completion: {
            () -> Void in animDone()
        })
    }
    
    static func buildMuteButton(y: Int) {
        let texture = getMuteButtonImage()
        muteButton.name = "mute"
        muteButton.size = texture.size()
        muteButton.texture = texture
        muteButton.anchorPoint = CGPoint(x: 0.5, y: 0.5)
        muteButton.position = CGPoint(x: CGFloat(pfLeft) + muteButton.size.width, y: CGFloat(y - pfHeight))
        muteButton.zPosition = Layer.controls.rawValue
        muteButton.alpha = 1.0
    }
         
    static func getMuteButtonImage() -> SKTexture {
        return SoundManager.isMuted() ? audioOffButton : audioOnButton;
    }

    static func buildHelpButton(y: Int) {
        helpButton.size = helpTexture.size()
        helpButton.name = "help"
        helpButton.texture = helpTexture
        helpButton.anchorPoint = CGPoint(x: 0.5, y: 0.5)
        helpButton.position = CGPoint(x: CGFloat(pfRight) - helpButton.size.width, y: CGFloat(y - pfHeight))
        helpButton.zPosition = Layer.controls.rawValue
        helpButton.alpha = 1.0
    }

    static func ivAddLabel(color: SKColor, text: String, y: Int) -> SKLabelNode {
        return addLabel(color: color, text: text, size: 28, x: 0, y: y - pfHeight, addToParent: false)
    }

    static func addLetter(text: String /*, x: Int, y: Int */) -> SKLabelNode {
        let lbl = SKLabelNode()
        lbl.fontName = GameFont
        lbl.fontColor = cLightGray
        lbl.fontSize = 60
        lbl.zPosition = Layer.text.rawValue
        lbl.text = text
        lbl.horizontalAlignmentMode = .left
        lbl.verticalAlignmentMode = .top
        lbl.position = CGPoint(x: 0, y: logoY - pfHeight)
        lbl.alpha = 1.0
        return lbl
    }

    static func buildGradient() {
        let cgImage = gradientTexture.cgImage()
        let width = cgImage.width
        let pixelData = cgImage.dataProvider!.data
        let data: UnsafePointer<UInt8> = CFDataGetBytePtr(pixelData)
 
        for x in 0..<width {
            let pixelInfo: Int = Int(x) * 4
            let r = CGFloat(data[pixelInfo]) / CGFloat(255.0)
            let g = CGFloat(data[pixelInfo+1]) / CGFloat(255.0)
            let b = CGFloat(data[pixelInfo+2]) / CGFloat(255.0)
            let a = CGFloat(data[pixelInfo+3]) / CGFloat(255.0)
            let rgba = SKColor(red: r, green: g, blue: b, alpha: a)
            gradient.append(rgba)
        }
    }

    static func animDone() {
        blingBling = true
    }
    
    static func touchesEnded(touches: Set<UITouch>, view: SKScene) {
        SoundManager.playSfx(sfx: Sfx.buttonPress)

        if introViewMode == IntroViewMode.help {
            IntroView.run()
            return
        }

        if let touch = touches.first {
            let location = touch.location(in: view)
            let tappedNodes = view.nodes(at: location)
            for node in tappedNodes {
                if node.name == "mute" {
                    SoundManager.toggleMute()
                    muteButton.texture = getMuteButtonImage()
                    
                    if SoundManager.isMuted() {
                        SoundManager.fadeOut()
                    } else {
                        SoundManager.fadeIn()
                    }
                    
                    return
                }
                
                if node.name == "help" {
                    HelpView.run()
                    return
                }
            }

            removeAllComponents(view: view)
            PlayView.newGame()
        }
    }

    static func update(view: SKScene) {
        if introViewMode == IntroViewMode.help {
            return
        }

        if !blingBling {
            return
        }

        colorDelay -= 1
        if colorDelay > 0 {
            return
        }
        colorDelay = 1

        let step = (Double.pi * 2.0) / Double(gradient.count / 4)
        for i in 0..<7 {
            let idx = Int(i)
            let letter = letters[idx]

            colorIndex[idx] += 1
            if colorIndex[idx] >= gradient.count {
                colorIndex[idx] = 0
            }
            
            if colorIndex[idx] < 0 {
                continue
            }

            let colorIdx = colorIndex[idx]
            letter.fontColor = gradient[colorIdx]

            letter.position.y = (sin(Double(colorIdx) * step) * 60.0) + Double(logoY)
        }
    }
}
