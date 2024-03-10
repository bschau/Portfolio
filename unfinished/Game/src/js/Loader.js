"use strict";

class Loader {
    preload() {
//    this.load.image('background', 'assets/background.png');
        this.load.spritesheet('player', 'gfx/player.png', { 
            frameWidth: PLAYERSIZE, 
            frameHeight: PLAYERSIZE, 
        });

//    this.load.image('coin', 'assets/coin.png');
//    this.load.image('enemy', 'assets/enemy.png');
//    this.load.audio('jump', ['assets/jump.ogg', 'assets/jump.mp3']);
//    this.load.audio('coin', ['assets/coin.ogg', 'assets/coin.mp3']);
//    this.load.audio('dead', ['assets/dead.ogg', 'assets/dead.mp3']);
//    this.load.image('pixel', 'assets/pixel.png');
//    this.load.spritesheet('mute', 'assets/muteButton.png', { 
//      frameWidth: 28, 
//      frameHeight: 22, 
//    }); 
        this.load.image('jumpButton', 'gfx/btn-jump.png');
        this.load.image('rightButton', 'gfx/btn-right.png');
        this.load.image('leftButton', 'gfx/btn-left.png');

        this.loadLabel = this.add.text(250, 170, 'loading\n0%', { font: '30px Arial', fill: '#fff', align: 'center' });
        this.loadLabel.setOrigin(0.5, 0.5);
        this.load.on('progress', this.progress, this);
    }

    progress(value) {
        let percentage = Math.round(value * 100) + '%';
        this.loadLabel.setText('loading\n' + percentage);
    }

    create() {
        this.anims.create({
            key: 'right',
            frames: this.anims.generateFrameNumbers('player', { frames: [1, 2] }),
            frameRate: 8,
            repeat: -1,
        });
        this.anims.create({
            key: 'left',
            frames: this.anims.generateFrameNumbers('player', { frames: [3, 4] }),
            frameRate: 8,
            repeat: -1,
        });

        this.scene.start('game');
    }
}
