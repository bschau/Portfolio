"use strict";

let game = new Phaser.Game({
    width: 1184,
    height: 680,
    backgroundColor: '#3498db', 
    physics: {
        default: 'arcade',
        arcade: {
           debug: true  // Enable collision boxes
        }
    },
    parent: 'game', 
    scale: {
        mode: Phaser.Scale.FIT,
        autoCenter: Phaser.Scale.CENTER_BOTH,
        min: {
            width: 296,
            height: 170,
        },
        max: {
            width: 2368,
            height: 1360,
        }
    }
});

game.scene.add('loader', Loader);
//game.scene.add('menu', Menu);
game.scene.add('game', Game);

game.scene.start('loader');
