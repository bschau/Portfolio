//
//  GameScene.swift
//  CoinGame
//
//  Created by Brian Schau on 13/07/2023.
//

import SpriteKit
import GameplayKit

class GameScene: SKScene, SKPhysicsContactDelegate {
    let player: Player = Player()
    
    override func didMove(to view: SKView) {
        let tileMapData = loadJSONFile(fileName: "map1")
        let tiles = UIImage(named: tileMapData.tileset)!.slice(tileSize: 40)
        setTiles(tiles: tiles, tileMapData: tileMapData.data)
        
        player.position = CGPoint(x: (tileMapData.startX * 40) + 20, y: 730 - (tileMapData.startY * 40) + 20)
        print(player.position.x)
        print(player.position.y)
        addChild(player)
        
        
        physicsWorld.contactDelegate = self

        
        /*
        let tileSet = SKTileSet()
        let tileMap = SKTileMapNode(tileSet: <#T##SKTileSet#>, columns: 25, rows: 17, tileSize: CGSize(20, 20))
         */
/*
        let tile1Map = SKTileMapNode(tileSet: tilesetImage, size: CGSize(width: 100, height: 100))
        
        // Get label node from scene and store it for use later
        self.label = self.childNode(withName: "//helloLabel") as? SKLabelNode
        if let label = self.label {
            label.alpha = 0.0
            label.run(SKAction.fadeIn(withDuration: 2.0))
        }
        
        // Create shape node to use during mouse interaction
        let w = (self.size.width + self.size.height) * 0.05
        self.spinnyNode = SKShapeNode.init(rectOf: CGSize.init(width: w, height: w), cornerRadius: w * 0.3)
        
        if let spinnyNode = self.spinnyNode {
            spinnyNode.lineWidth = 2.5
            
            spinnyNode.run(SKAction.repeatForever(SKAction.rotate(byAngle: CGFloat(Double.pi), duration: 1)))
            spinnyNode.run(SKAction.sequence([SKAction.wait(forDuration: 0.5),
                                              SKAction.fadeOut(withDuration: 0.5),
                                              SKAction.removeFromParent()]))
        }
             */
    }
    
    func loadJSONFile(fileName: String) -> TileMap {
        if let url = Bundle.main.url(forResource: fileName, withExtension: "json") {
            if let content = try? String(contentsOf: url) {
                let decoder = JSONDecoder()
                do {
                    let tileMap = try decoder.decode(TileMap.self, from: content.data(using: .utf8)!)
                    return tileMap
                } catch {}
            }
        }

        return TileMap(data: [])
    }
    
    func setTiles(tiles: [UIImage], tileMapData: [Int]) {
        let offsetX = ((1334 - (32 * 40)) / 2) + 20  // (20 = half a tile)
        var index = 0
        var yPos = 750
        for _ in 1...15 {
            var xPos = offsetX
            for _ in 1...32 {
                let tile = tileMapData[index]
                let tileNode = SKSpriteNode(texture: SKTexture(image: tiles[tile]))
                tileNode.position = CGPoint(x: xPos, y: yPos)
                tileNode.anchorPoint = CGPoint(x: 1.0, y: 1.0)
                tileNode.zPosition = Layer.tiles.rawValue
                if tile == 1 {
                    let physicsBody = SKPhysicsBody(rectangleOf: CGSize(width: 40, height: 40))
                    physicsBody.categoryBitMask = PhysicsCategory.platform
                    physicsBody.contactTestBitMask = PhysicsCategory.player
                    physicsBody.collisionBitMask = PhysicsCategory.player
                    physicsBody.pinned = true
                    physicsBody.allowsRotation = false
                    tileNode.physicsBody = physicsBody
                }
                addChild(tileNode)
                index += 1
                xPos += 40
            }
            yPos -= 40
        }
    }
    
    func touchDown(atPoint pos : CGPoint) {
        let nodeAtPoint = atPoint(pos)
        if let touchedNode = nodeAtPoint as? SKSpriteNode {
            if touchedNode.name == "button-jump" {
                player.jump()
            } else if touchedNode.name?.starts(with: "button-") == true {
                let direction = touchedNode.name?.replacingOccurrences(of: "button-", with: "")
                player.move(direction: direction!)
            }
        }
    }
    
    func touchMoved(toPoint pos : CGPoint) {
        let nodeAtPoint = atPoint(pos)
        if let touchedNode = nodeAtPoint as? SKSpriteNode {
            if touchedNode.name == "button-jump" {
                player.jump()
            } else if touchedNode.name?.starts(with: "button-") == true {
                let direction = touchedNode.name?.replacingOccurrences(of: "button-", with: "")
                player.move(direction: direction!)
            }
        }
    }
    
    func touchUp(atPoint pos : CGPoint) {
        let nodeAtPoint = atPoint(pos)
        if let touchedNode = nodeAtPoint as? SKSpriteNode {
            if touchedNode.name?.starts(with: "button-") == true {
                player.stop()
            }
        }
    }

    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
      for t in touches { self.touchDown(atPoint: t.location(in: self)) }
    }
    
    override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
      for t in touches { self.touchMoved(toPoint: t.location(in: self)) }
    }
    
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
      for t in touches { self.touchUp(atPoint: t.location(in: self)) }
    }
    
    override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
      for t in touches { self.touchUp(atPoint: t.location(in: self)) }
    }
    
    
    override func update(_ currentTime: TimeInterval) {
        // Called before each frame is rendered
    }
    
    func didBegin(_ contact: SKPhysicsContact) {
        let collision = contact.bodyA.contactTestBitMask | contact.bodyB.contactTestBitMask
        if collision == (PhysicsCategory.player | PhysicsCategory.platform) {
            player.hitPlatform(contact: contact)
        }
    }
}
