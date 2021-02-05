""" Meteor game """
import random
import pgzrun

WIDTH = 800
HEIGHT = 600

player = Actor('player', anchor=(0, 0))
player.pos = 15, (HEIGHT - player.height) / 2

STJERNE_I = []
STJERNE_X = []
METEORER = []
METEORER_H = []
NY_METEOR = 1
game_over = False
score = 0

for i in range(100):
    meteor = Actor('meteor', anchor=(0, 0))
    METEORER.append(meteor)
    METEORER_H.append(-1)

HASTIGHEDER = [0.5, 1, 2]
FARVER = [(85, 85, 85), (136, 136, 136), (187, 187, 187)]
for y in range(int(HEIGHT / 2)):
    index = random.randint(0, 2)
    STJERNE_I.append(index)
    STJERNE_X.append(random.randint(0, WIDTH + 50))

def draw():
    screen.fill((0, 0, 0))
    y = 0
    for i in range(len(STJERNE_X)):
        rect = Rect(STJERNE_X[i], y, 1, 1)
        screen.draw.rect(rect, FARVER[STJERNE_I[i]])
        y = y + 2

    for i in range(len(METEORER_H)):
        if METEORER_H[i] > -1:
            METEORER[i].draw()

    player.draw()
    screen.draw.text(str(score), right=WIDTH, top=0, fontname="computerfont", fontsize=60, shadow=(1.0, 1.0), scolor='blue')

def update():
    for i in range(len(STJERNE_X)):
        x = STJERNE_X[i] - HASTIGHEDER[STJERNE_I[i]]
        if x < 0:
            x = WIDTH + random.randint(0, 50)
        STJERNE_X[i] = x

    if game_over:
        player.angle = player.angle + 8
        if player.angle > 359:
            player.angle = 0
        return

    for i in range(len(METEORER_H)):
        if METEORER_H[i] != -1:
            METEORER[i].x = METEORER[i].x - METEORER_H[i]
            if METEORER[i].x < -METEORER[i].width:
                METEORER_H[i] = -1

    dan_meteor()
    handle_input()
    kollisioner()

def handle_input():
    speed = 4
    if keyboard.left:
        if player.x > 0:
            player.x = player.x - speed
    if keyboard.right:
        if player.x < WIDTH - player.width:
            player.x = player.x + speed
    if keyboard.up:
        if player.y > 0:
            player.y = player.y - speed
    if keyboard.down:
        if player.y < HEIGHT - player.height:
            player.y = player.y + speed

def dan_meteor():
    global NY_METEOR, score

    NY_METEOR = NY_METEOR - 1
    if NY_METEOR > 0:
        return

    NY_METEOR = 40
    for i in range(len(METEORER_H)):
        if METEORER_H[i] == -1:
            METEORER_H[i] = random.randint(1, 6)
            position = random.randint(0, HEIGHT - METEORER[0].height - 1)
            METEORER[i].pos = WIDTH, position
            score = score + 1
            return

def kollisioner():
    global game_over, player

    x = player.x + 5
    y = player.y + 16
    prect = Rect(x, y, 16, 34)
    for i in range(len(METEORER_H)):
        if METEORER_H[i] == -1:
            continue

        m = METEORER[i]
        mrect = Rect(m.x + 4, m.y + 4, m.width - 8, m.height - 8)
        if mrect.colliderect(prect):
            player.anchor = ('center', 'center')
            game_over = True
            return

pgzrun.go()