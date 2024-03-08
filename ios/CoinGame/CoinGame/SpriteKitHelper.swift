import Foundation
import SpriteKit

enum Layer: CGFloat {
    case tiles
    case player
    case controls
}

enum PhysicsCategory {
    static let none: UInt32 = 0
    static let player: UInt32 = 0b001
    static let platform: UInt32 = 0b010
    //static let coin: UInt32 = 0b100
}

extension SKSpriteNode {
    func loadTextures(atlas: String, prefix: String, startsAt: Int, stopsAt: Int) -> [SKTexture] {
        var textureArray = [SKTexture]()
        let textureAtlas = SKTextureAtlas(named: atlas)
        for i in startsAt...stopsAt {
            let textureName = "\(prefix)-\(i)"
            let temp = textureAtlas.textureNamed(textureName)
            textureArray.append(temp)
        }

        return textureArray
    }
    
    // Start the animation using a name and a count (0 = repeat forever)
    func startAnimation(textures: [SKTexture], speed: Double, name: String, count: Int, resize: Bool, restore: Bool) {
        if (action(forKey: name) == nil) {
            let animation = SKAction.animate(with: textures, timePerFrame: speed,
                                         resize: resize, restore: restore)
            if count == 0 {
                let repeatAction = SKAction.repeatForever(animation)
                run(repeatAction, withKey: name)
                return
            }
            
            if count == 1 {
                run(animation, withKey: name)
                return
            }
            
            let repeatAction = SKAction.repeat(animation, count: count)
            run(repeatAction, withKey: name)
        }
    }
    
    func stopAnimation(name: String) {
        if action(forKey: name) === nil {
            return
        }
        
        removeAction(forKey: name)
    }
}
