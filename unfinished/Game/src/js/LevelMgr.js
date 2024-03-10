"use strict";

function LvlMgrGetLevelData(level) {
    return {
        tileset: "tiles01.png",
        tilesJumpThrough: [ 1 ],
        map: "map01.json",
        playerStartX: 18,
        playerStartY: 9,
    }
}

function LvlMgrLoad(phaser, levelData, callback) {
    let loader = new Phaser.Loader.LoaderPlugin(phaser);
    loader.image('tileset', 'lvl/' + levelData.tileset);
    loader.tilemapTiledJSON('map', 'lvl/' + levelData.map);
    loader.once(Phaser.Loader.Events.COMPLETE, () => {
        let map = phaser.add.tilemap('map');
        let tileset = map.addTilesetImage('tileset', 'tileset');
        phaser.walls = map.createLayer('Walls', tileset);
        phaser.jumpThru = map.createLayer('JumpThru', tileset);

        phaser.jumpThru.setCollision(1);
        phaser.jumpThru.layer.data.forEach(function (row) {
            row.forEach(function (tile) {
                if (levelData.tilesJumpThrough.includes(tile.index)) {
                    tile.collideDown = false; 
                    tile.collideLeft = false;
                    tile.collideRight = false;
                    tile.collideUp = true;
                }
            });
        });

        phaser.walls.setCollision(2);

        callback();
    });
    loader.start();
}

/*
const tileCollisions = [0, 1, 2, 3] // designate which tiles in the tileset you want to make collideable

level.layer.data.forEach(function (row) {
  row.forEach(function (tile) {
    if (tileCollisions.includes(tile.index)) {
      tile.collideDown = false
      tile.collideLeft = false
      tile.collideRight = false
      tile.collideUp = true
      // or less verbosely:
      // tile.setCollision(false, false, true, false)
    }
  })
})
*/