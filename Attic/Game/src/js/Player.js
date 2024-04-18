"use strict";

var plyX, plyY;
var player;
var jumpTimer;

function PlayerInit(phaser, playerStartX, playerStartY) {
    plyX = playerStartX * TILESIZE;
    plyY = (playerStartY * TILESIZE) - 1;

    player = phaser.physics.add.sprite(plyX, plyY, 'player');
    player.name = "Player";
    player.body.gravity.y = 1000;
    player.body.gravity.x = 0;
    jumpTimer = 0;
}

function PlayerIsActive() {
    return player.active;
}

function PlayerHandleCollisions(phaser) {
    if (jumpTimer === 0) {
        phaser.physics.collide(player, phaser.jumpThru);
    }
    phaser.physics.collide(player, phaser.walls);
}

function PlayerMove(phaser, frameTime) {
    if (phaser.arrow.left.isDown || phaser.moveLeft) {
        player.body.velocity.x = -300;
        player.anims.play('left', true);
    } else if (phaser.arrow.right.isDown || phaser.moveRight) {
        player.body.velocity.x = 300;
        player.anims.play('right', true);
    } else {
        player.body.velocity.x = 0;
        player.setFrame(0);
    }

    if (phaser.arrow.up.isDown || phaser.jump) {
        if (player.body.onFloor()) {
//            jumpTimer = frameTime;
            player.body.velocity.y = -500;
            return;
//        } else if (jumpTimer !== 0 && (frameTime - jumpTimer < 250)) {
//            player.body.velocity.y = -500;
//            return;
        }
    }

    jumpTimer = 0;
}


    /*
    if (phaser.arrow.up.isDown) {
        PlayerJump();
    }

function PlayerJump() {
    if (player.body.onFloor()) {
        player.body.velocity.y = -220;
//        this.jumpSound.play(); 
    }
}
    */