//
//  TileMap.swift
//  CoinGame
//
//  Created by Brian Schau on 13/07/2023.
//

import Foundation

struct TileMap: Codable {
    var tileset: String = ""
    var startX: Int = 0
    var startY: Int = 0
    var data: [Int]
}
