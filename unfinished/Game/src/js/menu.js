class Menu {
  create(data) {
    let score = data.score ? data.score : 0;

    this.add.image(250, 170, 'background');

    if (localStorage.getItem('bestScore') === null) {
      localStorage.setItem('bestScore', 0);
    }

    if (score > localStorage.getItem('bestScore')) {
      localStorage.setItem('bestScore', score);
    }

    let nameLabel = this.add.text(250, -50, 'Super Coin Box', { font: '70px Geo', fill: '#fff' });
    nameLabel.setOrigin(0.5, 0.5);
    this.tweens.add({
      targets: nameLabel, 
      y: 80,
      duration: 1000, 
      ease: 'bounce.out',
    });

    let scoreText = 'score: ' + score + '\nbest score: ' + localStorage.getItem('bestScore');
    let scoreLabel = this.add.text(250, 170, scoreText, { font: '25px Arial', fill: '#fff', align: 'center' });
    scoreLabel.setOrigin(0.5, 0.5);

    let startText;
    if (this.sys.game.device.os.desktop) {
      startText = 'press the up arrow key to start';
    } else {
      startText = 'touch the screen to start';
    }
    let startLabel = this.add.text(250, 260, startText, { font: '25px Arial', fill: '#fff' });
    startLabel.setOrigin(0.5, 0.5);
    startLabel.angle = -2;
    this.tweens.add({
      targets: startLabel, 
      angle: 4,
      duration: 1000, 
      yoyo: true,
      repeat: -1,
    });

    this.upKey = this.input.keyboard.addKey('up');

    this.muteButton = this.add.sprite(30, 30, 'mute');
    this.muteButton.setInteractive({ useHandCursor: true });
    this.muteButton.on('pointerdown', this.toggleSound, this);
    this.muteButton.setFrame(this.sound.mute ? 1 : 0);
  }

  update() {
    if (!this.sys.game.device.os.desktop && this.input.activePointer.y < 60) {
      return;
    }

    if (this.upKey.isDown || (!this.sys.game.device.os.desktop && this.input.activePointer.isDown)) {
      this.scene.start('play');
    }
  }

  toggleSound() {
    if (this.sound.mute) {
      this.sound.mute = false;
      this.muteButton.setFrame(0);
    } else {
      this.sound.mute = true;
      this.muteButton.setFrame(1);
    }
  }
}
