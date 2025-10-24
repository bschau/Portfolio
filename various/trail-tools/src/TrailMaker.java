package com.schau.TrailTools;

/**
 * TrailMaker.
 */
public class TrailMaker {
	public static void main(String[] args) {
		Options opt = new Options();
		Trail ti = null;
		String processedFileName = "processed.png";
		int cBridge = 0x00ff00, cHidden = 0x0000ff, cProcessed = 0xffff00ff, cStart = 0xff0000, cTrail = 0;

		try {
			String s;

			opt.Parse(args, "P:b:h:np:s:t:");
			if ((s = opt.Get("P")) != null) {
				processedFileName = s;
			}

			if ((s = opt.Get("b")) != null) {
				cBridge = Integer.parseInt(s);
			}

			if ((s = opt.Get("h")) != null) {
				cHidden = Integer.parseInt(s);
			}

			if ((s = opt.Get("n")) != null) {
				CoordsConv.setNullBased();
			}

			if ((s = opt.Get("p")) != null) {
				cProcessed = Integer.parseInt(s) | 0xff000000;
			}

			if ((s = opt.Get("s")) != null) {
				cStart = Integer.parseInt(s);
			}

			if ((s = opt.Get("t")) != null) {
				cTrail = Integer.parseInt(s);
			}
		} catch (Exception ex) {
			System.err.println("*** " + ex.getMessage());
			System.exit(1);
		}

		try {
			String img;

			switch (opt.GetRestArgsCount()) {
				case 1:
					img = opt.GetRestArg(0);
					Emit.setEmitToStdOut();
					break;
				case 2:
					img = opt.GetRestArg(0);
					Emit.setEmitToFile(opt.GetRestArg(1));
					break;
				default:
					throw new Exception("Version 1.0\nUsage: TrailMaker [OPTIONS] gif-with-trail [output]");
			}

			ti = new Trail(cTrail, cBridge, cHidden, cProcessed);
			ti.load(img);
			ti.trail(cStart);
			Emit.commit();
			System.out.println(Emit.count + " coordinates created");
			System.out.println("| " + ti.bridges + " bridge" + ((ti.bridges == 1) ? " " : "s ") + "seen");
			ti.writeCanvas(processedFileName);
		} catch (Exception ex) {
			System.err.println("*** " + ex.getMessage());
			System.exit(1);
		} finally {
			Emit.close();
		}

		System.exit(0);
	}
}
