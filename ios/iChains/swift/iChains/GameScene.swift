//
//  GameScene.swift
//  iChains
//
//  Created by Brian Schau on 29/06/2023.
//

import SpriteKit
import GameplayKit

class GameScene: SKScene {
    override func didMove(to view: SKView) {
        SoundManager.start(view: self)
        
        if let stars = SKEmitterNode(fileNamed: "Stars.sks") {
            stars.name = "stars"
            stars.position = CGPoint(x: frame.midX, y: frame.midY)
            stars.zPosition = Layer.stars.rawValue
            addChild(stars)
        }
    
        IntroView.buildGradient()
        
        if gdRound > 0 {
            PlayView.continueGame()
            PlayScreens.psRoundX()
        } else {
            IntroView.run()
        }
    }
    
    func touchDown(atPoint pos : CGPoint) {
    }
    
    func touchMoved(toPoint pos : CGPoint) {
    }
    
    func touchUp(atPoint pos : CGPoint) {
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
    }
    
    override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
    }
    
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        if appMode == Mode.IntroMode {
            IntroView.touchesEnded(touches: touches, view: self)
        } else if appMode == Mode.PlayMode {
            PlayView.touchesEnded(touches: touches, view: self)
        }
    }
    
    override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
    }
    
    
    override func update(_ currentTime: TimeInterval) {
        if appMode == Mode.PlayMode {
            PlayView.update(view: self)
        } else if appMode == Mode.IntroMode {
            IntroView.update(view: self)
        }
    }
}
