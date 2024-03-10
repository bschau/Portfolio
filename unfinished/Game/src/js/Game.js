class Game {
    create() {
        this.gameLoaded = false;

        this.input.addPointer(1);
        this.arrow = this.input.keyboard.createCursorKeys();

        var level = 1; 
        var levelData = LvlMgrGetLevelData(level);
        LvlMgrLoad(this, levelData, () => {
            PlayerInit(this, levelData.playerStartX, levelData.playerStartY);

            if (!this.sys.game.device.os.desktop) {
                this.addMobileInputs(); 
      
                this.rotateLabel = this.add.text(250, 170, '', { font: '30px Arial', fill: '#fff', backgroundColor: '#000' });
                this.rotateLabel.setOrigin(0.5, 0.5);

                this.scale.on('orientationchange', this.orientationChange, this);
                this.orientationChange();
            }

            this.gameLoaded = true;
        });



    /*
    this.coin = this.physics.add.sprite(60, 130, 'coin');

    this.scoreLabel = this.add.text(30, 25, 'score: 0', { font: '18px Arial', fill: '#fff' });
    this.score = 0; 

    this.enemies = this.physics.add.group();
    this.nextEnemy = 0;

    this.jumpSound = this.sound.add('jump');
    this.coinSound = this.sound.add('coin');
    this.deadSound = this.sound.add('dead'); 

    let particles = this.add.particles('pixel');
    this.emitter = particles.createEmitter({
      quantity: 15,
      speed: { min: -150, max: 150 },
      scale: { start: 2, end: 0.1 },
      lifespan: 800,
      on: false,
    });

    */
    }

    update(frameTime) {
        if (!this.gameLoaded) {
            return;
        }

       // this.physics.collide(player, this.walls);
        PlayerHandleCollisions(this);

        if (!PlayerIsActive()) {
            return;
        }

        PlayerMove(this, frameTime);


        /*
    this.physics.collide(this.player, this.walls);
    this.physics.collide(this.enemies, this.walls);

    if (!this.player.active) {
      return;
    }

    this.movePlayer(); 

    if (this.physics.overlap(this.player, this.coin)) {
      this.takeCoin();
    }

    if (this.player.y > 340 || this.player.y < 0) {
      this.playerDie();
    }

    if (this.physics.overlap(this.player, this.enemies)) {
      this.playerDie();
    }

    let now = Date.now();
    if (this.nextEnemy < now) {
      let startDifficulty = 4000;
      let endDifficulty = 1000;
      let scoreToReachEndDifficulty = 100;

      let progress = Math.min(this.score / scoreToReachEndDifficulty, 1);
      let delay = startDifficulty - (startDifficulty - endDifficulty) * progress;

      this.addEnemy();
      this.nextEnemy = now + delay;
    }
    */
    }

    takeCoin() {
        /*
    this.score += 5;
    this.scoreLabel.setText('score: ' + this.score);

    this.updateCoinPosition();

    this.coinSound.play();

    this.coin.setScale(0);
    this.tweens.add({
      targets: this.coin, 
      scale: 1, 
      duration: 300, 
    });

    this.tweens.add({
      targets: this.player, 
      scale: 1.3, 
      duration: 100, 
      yoyo: true,
    });
    */
    }

    updateCoinPosition() {
        /*
    let positions = [
      { x: 140, y: 60 }, 
      { x: 360, y: 60 }, 
      { x: 60, y: 140 }, 
      { x: 440, y: 140 }, 
      { x: 130, y: 300 }, 
      { x: 370, y: 300 },
    ];

    positions = positions.filter(coin => coin.x !== this.coin.x);

    let newPosition = Phaser.Math.RND.pick(positions);

    this.coin.setPosition(newPosition.x, newPosition.y);
    */
    }

    addEnemy() {
        /*
    let enemy = this.enemies.create(250, -10, 'enemy');

    enemy.body.gravity.y = 500;
    enemy.body.velocity.x = Phaser.Math.RND.pick([-100, 100]);
    enemy.body.bounce.x = 1;

    this.time.addEvent({
      delay: 10000,  
      callback: () => enemy.destroy(),
    });
        */
    }

    playerDie() {
        /*
    this.player.destroy();
    
    this.deadSound.play();
    this.emitter.setPosition(this.player.x, this.player.y);
    this.emitter.explode();

    this.cameras.main.shake(300, 0.02);

    this.time.addEvent({
      delay: 1000,  
      callback: () => this.scene.start('menu', { score: this.score }),
    });
        */
    }

    addMobileInputs() {
        this.moveLeft = false;
        this.moveRight = false;

        let jumpButton = this.add.sprite(1084, 610, 'jumpButton');
        jumpButton.setInteractive();
        //jumpButton.on('pointerdown', PlayerJump, this);
        jumpButton.on('pointerover', () => this.jump = true, this);
        jumpButton.on('pointerout', () => this.jump = false, this);
        jumpButton.alpha = 0.5;
    
        let leftButton = this.add.sprite(100, 610, 'leftButton');
        leftButton.setInteractive();
        leftButton.on('pointerover', () => this.moveLeft = true, this);
        leftButton.on('pointerout', () => this.moveLeft = false, this);
        leftButton.alpha = 0.5;

        let rightButton = this.add.sprite(240, 610, 'rightButton');
        rightButton.setInteractive();
        rightButton.on('pointerover', () => this.moveRight = true, this);
        rightButton.on('pointerout', () => this.moveRight = false, this);
        rightButton.alpha = 0.5;
    }

    orientationChange() {
        if (this.scale.orientation === Phaser.Scale.PORTRAIT) {
            this.rotateLabel.setText(' rotate your device in landscape '); 
            this.scene.pause();
        } else if (this.scale.orientation === Phaser.Scale.LANDSCAPE) {
            this.rotateLabel.setText('');
            this.scene.resume();
        }
    }
}
