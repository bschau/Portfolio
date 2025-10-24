import Foundation
import SpriteKit

enum PlayerAnimationType: String {
    case still
    case walk
}

class Player: SKSpriteNode {
    private var stillTexture: SKTexture = SKTexture(imageNamed: "player-still")
    private var walkLeftTextures: [SKTexture]?
    private var walkRightTextures: [SKTexture]?
    private var canJump: Bool = true

    init() {
        super.init(texture: stillTexture, color: .clear, size: stillTexture.size())
        
        self.walkLeftTextures = self.loadTextures(atlas: "player", prefix: "player-left", startsAt: 1, stopsAt: 2)
        self.walkRightTextures = self.loadTextures(atlas: "player", prefix: "player-right", startsAt: 1, stopsAt: 2)
        self.name = "player"
        self.anchorPoint = CGPoint(x: 1.0, y: 1.0)
        self.zPosition = Layer.player.rawValue
        let physicsBody = SKPhysicsBody(rectangleOf: CGSize(width: 40, height: 40))
        physicsBody.categoryBitMask = PhysicsCategory.player
        physicsBody.contactTestBitMask = PhysicsCategory.platform
        physicsBody.collisionBitMask = PhysicsCategory.platform
        physicsBody.allowsRotation = false
        physicsBody.restitution = 0
        physicsBody.friction = 0
        self.physicsBody = physicsBody
    }
    
    func move(direction: String) {
        if direction == "left" {
            if (action(forKey: PlayerAnimationType.walk.rawValue) != nil) {
                return
            }
            
            let animation = SKAction.animate(with: walkLeftTextures!, timePerFrame: 0.05, resize: true, restore: true)
            let movement = SKAction.moveBy(x: -40, y: 0, duration: 0.1)
            let group = SKAction.group([animation, movement])
            let repeatAction = SKAction.repeatForever(group)
            run(repeatAction, withKey: PlayerAnimationType.walk.rawValue)
            return
        }
        
        if (action(forKey: PlayerAnimationType.walk.rawValue) != nil) {
            return
        }
        
        let animation = SKAction.animate(with: walkRightTextures!, timePerFrame: 0.05, resize: true, restore: true)
        let movement = SKAction.moveBy(x: 40, y: 0, duration: 0.1)
        let group = SKAction.group([animation, movement])
        let repeatAction = SKAction.repeatForever(group)
        run(repeatAction, withKey: PlayerAnimationType.walk.rawValue)
        return
    }
    
    func jump() {
        if canJump {
            canJump = false
            physicsBody?.velocity = CGVectorMake(0, 0)
            physicsBody?.applyImpulse(CGVectorMake(0, 40.0))
        }
    }
    
    func stop() {
        if action(forKey: PlayerAnimationType.walk.rawValue) !== nil {
            removeAction(forKey: PlayerAnimationType.walk.rawValue)
        }
    }
    
    func hitPlatform(contact: SKPhysicsContact) {
        let (player, platform) = getPlayerAndPlatform(contact: contact)
        
        if Int(player!.position.y) < Int(platform!.position.y) {
            canJump = true
        }
    }
    
    func getPlayerAndPlatform(contact: SKPhysicsContact) -> (SKNode?, SKNode?) {
        if contact.bodyA.collisionBitMask == PhysicsCategory.player {
            return (contact.bodyA.node, contact.bodyB.node)
        }
        
        return (contact.bodyB.node, contact.bodyA.node)
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

