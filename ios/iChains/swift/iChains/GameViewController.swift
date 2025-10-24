//
//  GameViewController.swift
//  iChains
//
//  Created by Brian Schau on 29/06/2023.
//

import UIKit
import SpriteKit
import GameplayKit

class GameViewController: UIViewController {
    override func viewWillLayoutSubviews() {
        super.viewWillLayoutSubviews()
        if #available(iOS 11.0, *), let view = self.view {
            view.frame = self.view.safeAreaLayoutGuide.layoutFrame
        }
        guard let view = self.view as! SKView? else { return }
        view.ignoresSiblingOrder = true
        
        pfWidth = Int(view.bounds.size.width)
        pfHeight = Int(view.bounds.size.height)
        pfLeft = -(pfWidth / 2)
        pfRight = pfWidth / 2
        pfTop = -(pfHeight / 2)
        pfBottom = pfHeight / 2
        
        if let view = self.view as! SKView? {
            if let scene = SKScene(fileNamed: "GameScene") {
                scene.size = view.bounds.size
                scene.scaleMode = .aspectFill
                parentScene = scene
                view.presentScene(scene)
            }            
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }

    override var supportedInterfaceOrientations: UIInterfaceOrientationMask {
        if UIDevice.current.userInterfaceIdiom == .phone {
            return .allButUpsideDown
        } else {
            return .all
        }
    }

    override var prefersStatusBarHidden: Bool {
        return true
    }

    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        
        SoundManager.fadeIn()
    }
}
