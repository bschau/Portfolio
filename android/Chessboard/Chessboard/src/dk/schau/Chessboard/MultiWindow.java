package dk.schau.Chessboard;

import java.util.ArrayList;
import java.util.List;

import wei.mark.standout.StandOutWindow;
import wei.mark.standout.constants.StandOutFlags;
import wei.mark.standout.ui.Window;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.FrameLayout;

public class MultiWindow extends StandOutWindow {
	private static final int CmdSetBoard = 0xcafe;
	
	@Override
	public String getAppName() {
		return "Chessboard";
	}

	@Override
	public int getAppIcon() {
		return R.drawable.window_icon;
	}

	@Override
	public String getTitle(int id) {
		return getAppName() + " #" + id;
	}

	@Override
	public void createAndAttachView(int id, FrameLayout frame) {
		LayoutInflater inflater = (LayoutInflater) getSystemService(LAYOUT_INFLATER_SERVICE);
		inflater.inflate(R.layout.body, frame, true);
	}

	@Override
	public StandOutLayoutParams getParams(int id, Window window) {
		return new StandOutLayoutParams(id, 270, 270, StandOutLayoutParams.AUTO_POSITION, StandOutLayoutParams.AUTO_POSITION, 100, 100);
	}

	@Override
	public int getFlags(int id) {
		return StandOutFlags.FLAG_DECORATION_SYSTEM
				| StandOutFlags.FLAG_BODY_MOVE_ENABLE
				| StandOutFlags.FLAG_WINDOW_HIDE_ENABLE
				| StandOutFlags.FLAG_WINDOW_PINCH_RESIZE_ENABLE
				;
	}

	@Override
	public String getPersistentNotificationTitle(int id) {
		return getAppName() + " Running";
 	}

	@Override
	public String getPersistentNotificationMessage(int id) {
		return "Tap to add a new " + getAppName() + " ...";
	}

	@Override
	public Intent getPersistentNotificationIntent(int id) {
		return StandOutWindow.getShowIntent(this, getClass(), getUniqueId());
	}

	@Override
	public int getHiddenIcon() {
		//return android.R.drawable.ic_menu_info_details;
		return R.id.window_icon;
	}

	@Override
	public String getHiddenNotificationTitle(int id) {
		return getAppName() + " Hidden";
	}

	@Override
	public String getHiddenNotificationMessage(int id) {
		return "Click to restore #" + id;
	}

	@Override
	public Intent getHiddenNotificationIntent(int id) {
		return StandOutWindow.getShowIntent(this, getClass(), id);
	}

	@Override
	public Animation getShowAnimation(int id) {
		if (isExistingId(id)) {
			return AnimationUtils.loadAnimation(this, android.R.anim.slide_in_left);
		}
		
		return super.getShowAnimation(id);
	}

	@Override
	public Animation getHideAnimation(int id) {
		return AnimationUtils.loadAnimation(this, android.R.anim.slide_out_right);
	}

	@Override
	public List<DropDownListItem> getDropDownItems(int id) {
		final int windowId = id;
		final Context context = this;
		
		List<DropDownListItem> items = new ArrayList<DropDownListItem>();
		items.add(new DropDownListItem(R.drawable.nomenuicon,
			"Clone", new Runnable() {
			@Override
			public void run() {
				Window window = getWindow(windowId);
				ChessboardView chessboardView = (ChessboardView) window.findViewById(R.id.chessboard);
				if (chessboardView != null) {
					MultiWindow childWindow = new MultiWindow();
					int childId = childWindow.getUniqueId();
					StandOutWindow.show(context, MultiWindow.class, childId);
					Bundle bundle = new Bundle();
					bundle.putString("board", ((ChessboardView) getWindow(windowId).findViewById(R.id.chessboard)).getChessboard().toString());
					StandOutWindow.sendData(context, MultiWindow.class, childId, CmdSetBoard, bundle, MultiWindow.class, windowId);
				}
			}
		}));
		items.add(new DropDownListItem(R.drawable.nomenuicon, 
			"Reset board", new Runnable() {
			@Override
			public void run() {
				Window window = getWindow(windowId);
				ChessboardView chessboardView = (ChessboardView) window.findViewById(R.id.chessboard);
				if (chessboardView != null) {
					chessboardView.getChessboard().reset();
					chessboardView.goToPlayMode();
					chessboardView.invalidate();
				}
			}
		}));
		items.add(new DropDownListItem(R.drawable.nomenuicon, 
			"Clear board", new Runnable() {
			@Override
			public void run() {
				Window window = getWindow(windowId);
				ChessboardView chessboardView = (ChessboardView) window.findViewById(R.id.chessboard);
				if (chessboardView != null) {
					chessboardView.getChessboard().clear();
					chessboardView.goToBuilder();
					chessboardView.invalidate();
				}
			}
		}));
		
		return items;
	}

	public boolean onTouchBody(int id, Window window, View view, MotionEvent event) {
		if (event.getAction() != MotionEvent.ACTION_DOWN) {
			return true;
		}
		
		ChessboardView chessboardView = (ChessboardView) getWindow(id).findViewById(R.id.chessboard);
		Chessboard chessboard = chessboardView.getChessboard();

		if (event.getY() < ChessboardView.ButtonBarSize) {
			chessboardView.buttonBarClick(event.getX(), event.getY());
			return true;
		}
		
		Mode mode = chessboardView.getMode();
        Move move = chessboardView.getMove();
		if (mode.isInPieceSelector()) {
			chessboardView.pieceSelectorClick(event.getX(), event.getY());
			return true;
		}
		
		if (mode.isInBlackPawnPromotion() || mode.isInWhitePawnPromotion()) {
			chessboardView.pawnPromotionClick(event.getX(), event.getY());
			return true;
		}
		
		float cellSizeWidth = ChessboardView.CellSize * chessboardView.getScaleWidth();
		float cellSizeHeight = ChessboardView.CellSize * chessboardView.getScaleHeight();
		
		int x = (int) (event.getX() / cellSizeWidth);
		int y = (int) ((event.getY() - ChessboardView.ButtonBarSize) / cellSizeHeight);
	
		if (x < 0 || x > 7 || y < 0 || y > 7) {
			return true;
		}

        int dstMove = move.convert(x, y);
		
		if (mode.isInEraseMode()) {
			chessboard.erase(dstMove);
			chessboardView.invalidate();
			return true;
		}
		
		if (mode.isInSetPiece()) {
			chessboard.setPiece(dstMove, chessboardView.getPieceSelector().getSelectedPiece());
			chessboardView.invalidate();
			return true;
		}

		if (move.isFirstMove()) {
			if (chessboard.isEmptySquare(dstMove)) {
				return true;
			}
			
			if ((chessboard.isBlackPly() && !chessboard.isBlackPiece(dstMove)) ||
				(chessboard.isWhitePly() && !chessboard.isWhitePiece(dstMove))) {
				move.reset();
				return true;
			}
		}
		
		move.add(x, y);
		if (move.isValidMove()) {
			if (!chessboard.isValidMove(move.getFrom(), move.getTo())) {
				move.reset();
				return true;
			}

            if (specialMove(chessboard, move)) {
                move.reset();
                chessboardView.invalidate();
                return true;
            }

			int dst = move.getTo();
			chessboard.move(move.getFrom(), dst);
			if (chessboard.pawnPromotionWhite(dst)) {
				chessboardView.goToWhitePawnPromotion(dst);
			} else if (chessboard.pawnPromotionBlack(dst)) {
				chessboardView.goToBlackPawnPromotion(dst);
			}
			move.reset();
		}
		
		chessboardView.invalidate();
		return true;
	}

    private boolean specialMove(Chessboard chessboard, Move move) {
        if (chessboard.tryWhiteCastling(move.getFrom(), move.getTo())) {
            return true;
        }

        if (chessboard.tryBlackCastling(move.getFrom(), move.getTo())) {
            return true;
        }

        if (chessboard.tryWhiteEnpassant(move.getFrom(), move.getTo())) {
            return true;
        }

        if (chessboard.tryBlackEnpassant(move.getFrom(), move.getTo())) {
            return true;
        }

        return false;
    }

	public void onReceiveData(int id, int requestCode, Bundle data, Class<? extends StandOutWindow> fromCls, int fromId) {
		if (requestCode == CmdSetBoard && data != null) {
			ChessboardView chessboardView = (ChessboardView) getWindow(id).findViewById(R.id.chessboard);
			Chessboard chessboard = chessboardView.getChessboard();
			String board = data.getString("board");
			chessboard.fromString(board);
			chessboardView.getMove().reset();
			chessboardView.goToPlayMode();
			chessboardView.invalidate();
		}
	}
	
	public boolean onFocusChange(int id, Window window, boolean focus) {
		if (focus) {
			bringToFront(id);
		}
		return false;
	}
}