import Foundation
import SpriteKit

class Ball: SKShapeNode {
    var radius: Double {
        didSet {
            self.path = Ball.path(radius: self.radius)
        }
    }

    var state: BallState = BallState.dead
    var speedX: Int = 0
    var speedY: Int = 0
    var parentBall: Ball?
    var chainDepth: Int = 0
    var idle: Int = 0
    
    override init() {
        self.radius = 0
        super.init()

        self.lineWidth = 1
        self.path = Ball.path(radius: self.radius)
        self.state = BallState.move
        self.parentBall = nil
        self.chainDepth = 0
        self.strokeColor = cDarkGray
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    class func path(radius: Double) -> CGMutablePath {
        let path: CGMutablePath = CGMutablePath()
        path.addArc(center: CGPoint.zero, radius: CGFloat(radius), startAngle: 0.0, endAngle: CGFloat(2.0*Double.pi), clockwise: false)
        return path
    }
}
