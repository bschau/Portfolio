//
//  SoundManager.swift
//  iChains
//
//  Created by Brian Schau on 09/07/2023.
//

import SpriteKit

class SoundManager {
    static var backgroundMusic: SKAudioNode = SKAudioNode(fileNamed: "TranquilTranceExpress.mp3")
    static var inited: Bool = false
    static let buttonSound = SKAction.playSoundFileNamed("buttonpress.mp3", waitForCompletion: false)
    static let explosionSound = SKAction.playSoundFileNamed("explosion.mp3", waitForCompletion: false)
    static let fireSound = SKAction.playSoundFileNamed("fire.mp3", waitForCompletion: false)
    static let roundCompletedSound = SKAction.playSoundFileNamed("round-completed.mp3", waitForCompletion: false)
    static let startNewRoundSound = SKAction.playSoundFileNamed("start-new-round.mp3", waitForCompletion: false)
    static let yaySound = SKAction.playSoundFileNamed("yay.mp3", waitForCompletion: false)
    static let naySound = SKAction.playSoundFileNamed("nay.mp3", waitForCompletion: false)
    static let missionAccomplishedSound = SKAction.playSoundFileNamed("mission-accomplished.mp3", waitForCompletion: false)
    static let sfxs: [SKAction] = [
        buttonSound, explosionSound, fireSound, roundCompletedSound, startNewRoundSound,
        yaySound, naySound, missionAccomplishedSound
    ]

    static func isMuted() -> Bool {
        let defaults = UserDefaults.standard
        return defaults.bool(forKey: udMuted)
    }

    static func start(view: SKScene) {
        if inited {
            return
        }
        
        inited = true
        backgroundMusic.name = "music"
        backgroundMusic.autoplayLooped = true
        backgroundMusic.isPositional = false
        backgroundMusic.run(SKAction.changeVolume(to: 0, duration: 0))
        
        view.addChild(backgroundMusic)
    }

    static func fadeIn() {
        if isMuted() {
            return
        }

        backgroundMusic.run(SKAction.sequence([SKAction.play(), SKAction.changeVolume(to: 1, duration: 2)]))
    }

    static func fadeOut() {
        backgroundMusic.run(SKAction.sequence([SKAction.changeVolume(to: 0, duration: 0.5), SKAction.pause()]))
    }
    
    static func toggleMute() {
        let defaults = UserDefaults.standard
        defaults.set(!isMuted(), forKey: udMuted)
    }
    
    static func playSfx(sfx: Sfx) {
        if isMuted() {
            return
        }
        
        parentScene.run(sfxs[sfx.rawValue])
    }
}
