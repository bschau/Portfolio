.flash name="ichains.swf" bbox=216x388 version=8 fps=25
.png s0 "frame1.png"
.png s1 "frame2.png"
.png s2 "frame3.png"
.png s3 "frame4.png"
.png s4 "frame5.png"
.png b "skin.png"
.put b x=0 y=0 alpha=100%
.put s0 x=16 y=65 alpha=0%
.frame 20
.change s0 alpha=100%
.frame 70
.change s0 alpha=100%
.frame 90
.change s0 alpha=0%
.put s1 x=16 y=65 alpha=0%
.del s0
.frame 110
.change s1 alpha=100%
.frame 160
.change s1 alpha=100%
.frame 180
.change s1 alpha=0%
.put s2 x=16 y=65 alpha=0%
.del s1
.frame 200
.change s2 alpha=100%
.frame 250
.change s2 alpha=100%
.frame 270
.change s2 alpha=0%
.put s3 x=16 y=65 alpha=0%
.del s2
.frame 290
.change s3 alpha=100%
.frame 340
.change s3 alpha=100%
.frame 360
.change s3 alpha=0%
.put s4 x=16 y=65 alpha=0%
.del s3
.frame 380
.change s4 alpha=100%
.frame 430
.change s4 alpha=100%
.frame 450
.change s4 alpha=0%
.put s0 x=16 y=65 alpha=0%
.del s4
.action:
	gotoFrame(0);
	Play();
.end
.end
