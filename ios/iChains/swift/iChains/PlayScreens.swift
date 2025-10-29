import Foundation
import SpriteKit

class PlayScreens {
    static func psAddLabel(color: SKColor, text: String, y: Int) -> SKLabelNode {
        return addLabel(color: color, text: text, size: 28, x: 0, y: y)
    }

    static func psFirstLevel() {
        let ys: [Int] = [ 64, 24, -36 ]
        var lbls: [SKLabelNode] = []
        
        lbls.append(psAddLabel(color: cGreen, text: "First round", y: ys[0]))
        lbls.append(getBalls(y: ys[1], round: gdRound))
        lbls.append(tapToContinue(y: ys[2]))

        addAnimations(labels: lbls, positions: ys)
    }
    
    static func psRoundX() {
        let ys: [Int] = [ 64, 24, -36 ]
        var lbls: [SKLabelNode] = []

        if gdRound == levels.count {
            lbls.append(psAddLabel(color: cBlue, text: "last round", y: ys[0]))
        } else {
            lbls.append(psAddLabel(color: cBlue, text: "Round \(gdRound)", y: ys[0]))
        }

        lbls.append(getBalls(y: ys[1], round: gdRound))
        lbls.append(tapToContinue(y: ys[2]))

        addAnimations(labels: lbls, positions: ys)
    }

    static func psLevelCompleted() {
        let ys: [Int] = [ 164, 104, 64, 24, -36, -76, -136 ]
        var lbls: [SKLabelNode] = []
        
        lbls.append(psAddLabel(color: cGreen, text: "Round Completed", y: ys[0]))
        lbls.append(psAddLabel(color: cLightGray, text: "Round score: \(roundScore)", y: ys[1]))
        if roundChainBonus > 0 {
            lbls.append(psAddLabel(color: cLightGray, text: "Chain bonus: \(roundChainBonus)", y: ys[2]))
        } else {
            lbls.append(psAddLabel(color: cRed, text: "No chain bonus", y: ys[2]))
        }
        lbls.append(psAddLabel(color: cLightGray, text: "Total score: \(gdTotal)", y: ys[3]))

        if gdRound == levels.count {
            lbls.append(psAddLabel(color: cBlue, text: "Now play last round", y: ys[4]))
        } else {
            lbls.append(psAddLabel(color: cBlue, text: "Now play round \(gdRound)", y: ys[4]))
        }

        lbls.append(getBalls(y: ys[5], round: gdRound))
        lbls.append(tapToContinue(y: ys[6]))

        addAnimations(labels: lbls, positions: ys)
    }

    static func addAnimations(labels: [SKLabelNode], positions: [Int]) {
        for i in 0..<labels.count {
            let action = SKAction.moveTo(y: CGFloat(positions[Int(i)]), duration: 0.4 + (CGFloat(i) * 0.07))
            action.timingMode = SKActionTimingMode.easeInEaseOut
            labels[Int(i)].run(action)
        }
    }

    static func psMissionAccomplished() {
        let ys: [Int] = [ 144, 84, 44, 4, -56, -116 ]
        var lbls: [SKLabelNode] = []
        
        lbls.append(psAddLabel(color: cGreen, text: "Mission Accomplished", y: ys[0]))
        lbls.append(psAddLabel(color: cLightGray, text: "Round score: \(roundScore)", y: ys[1]))
        if roundChainBonus > 0 {
            lbls.append(psAddLabel(color: cLightGray, text: "Chain bonus: \(roundChainBonus)", y: ys[2]))
        } else {
            lbls.append(psAddLabel(color: cRed, text: "No chain bonus", y: ys[2]))
        }
        lbls.append(psAddLabel(color: cLightGray, text: "Total score: \(gdTotal)", y: ys[3]))

        if newHiScore {
            lbls.append(psAddLabel(color: cGreen, text: "New high score!", y: ys[4]))
        } else {
            lbls.append(psAddLabel(color: cBlack, text: " ", y: ys[4]))
        }

        lbls.append(tapToContinue(y: ys[5]))

        addAnimations(labels: lbls, positions: ys)
    }
    
    static func psLevelFailed() {
        let ys: [Int] = [ 144, 84, 44, -16, -56, -116 ]
        var lbls: [SKLabelNode] = []
        
        lbls.append(psAddLabel(color: cRed, text: "Round Failed", y: ys[0]))
        lbls.append(psAddLabel(color: cRed, text: "Round penalty: \(penalty)", y: ys[1]))
        lbls.append(psAddLabel(color: cLightGray, text: "\(goal) more ball\(goal == 1 ? "" : "s")", y: ys[2]))
        
        if gdRound == levels.count {
            lbls.append(psAddLabel(color: cBlue, text: "Replay last round", y: ys[3]))
        } else {
            lbls.append(psAddLabel(color: cBlue, text: "Replay round \(gdRound)", y: ys[3]))
        }

        lbls.append(getBalls(y: ys[4], round: gdRound))
        lbls.append(tapToContinue(y: ys[5]))

        addAnimations(labels: lbls, positions: ys)
    }
    
    static func tapToContinue(y: Int) -> SKLabelNode {
        return psAddLabel(color: cGreen, text: "Tap to continue", y: y)
    }
    
    
    static func getBalls(y: Int, round: Int) -> SKLabelNode {
        let ballsCount = levels[round - 1].balls
        let goal = levels[round - 1].goal

        return psAddLabel(color: cBlue, text: "Get \(goal) of \(ballsCount) balls", y: y)
    }
}
