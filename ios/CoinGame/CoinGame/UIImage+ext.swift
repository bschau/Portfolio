import UIKit

extension UIImage {
    func slice(tileSize: Int) -> [UIImage] {
        var array: [UIImage] = []
        guard let cgImage = cgImage else { return [] }
        
        let cols = Int(CGFloat(cgImage.width) / CGFloat(tileSize))
        let rows = Int(CGFloat(cgImage.height) / CGFloat(tileSize))
        
        for row in 0..<rows {
            for col in 0..<cols {
                    if let cropped = cgImage.cropping(to: CGRect(x: col * tileSize, y: row * tileSize, width: tileSize, height: tileSize)) {
                    let img = UIImage(cgImage: cropped, scale: scale, orientation: imageOrientation)
                    array.append(img)
                }
            }
        }
        return array
    }
}
