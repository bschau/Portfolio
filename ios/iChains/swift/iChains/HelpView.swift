import SpriteKit
import Foundation

class HelpView {
    static func run() {
        let ys: [Int] = [ 200, 170, 140, 110, 80, 50, 20, -10, -40, -70, -140 ]
        var lbls: [SKNode] = []

        introViewMode = IntroViewMode.help
        removeAllComponents(view: parentScene)
        lbls.removeAll()

        lbls.append(ivAddLabel(color: cYellow, text: "score points by creating", y: ys[0]))
        lbls.append(ivAddLabel(color: cYellow, text: "longer and longer chains", y: ys[1]))
        lbls.append(ivAddLabel(color: cYellow, text: "of exploding balls.", y: ys[2]))
        lbls.append(ivAddLabel(color: cBlue, text: "tap anywhere to start", y: ys[3]))
        lbls.append(ivAddLabel(color: cBlue, text: "the initial chain.", y: ys[4]))
        lbls.append(ivAddLabel(color: cYellow, text: "each round has a number", y: ys[5]))
        lbls.append(ivAddLabel(color: cYellow, text: "of balls which must be", y: ys[6]))
        lbls.append(ivAddLabel(color: cYellow, text: "removed before going on", y: ys[7]))
        lbls.append(ivAddLabel(color: cYellow, text: "to the next round.", y: ys[8]))
        lbls.append(ivAddLabel(color: cBlue, text: "there are 12 rounds.", y: ys[9]))
        lbls.append(ivAddLabel(color: cGreen, text: "tap to go back.", y: ys[10]))

        for i in 0..<lbls.count {
            let lbl = lbls[Int(i)]
            parentScene.addChild(lbl)
            let action = SKAction.moveTo(y: CGFloat(ys[Int(i)]), duration: 0.6 + (CGFloat(i) * 0.07))
            action.timingMode = SKActionTimingMode.easeInEaseOut
            lbl.run(action)
        }
    }
    
    static func ivAddLabel(color: SKColor, text: String, y: Int) -> SKLabelNode {
        return addLabel(color: color, text: text, size: 28, x: 0, y: y - pfHeight, addToParent: false)
    }
}
