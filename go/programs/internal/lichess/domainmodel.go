package lichess

// LichessOpponent is an opponent
type LichessOpponent struct {
	Username string
}

// LichessGame is a single game
type LichessGame struct {
	FullID   string
	Opponent LichessOpponent
	IsMyTurn bool
}

// LichessResponse is the overall response from the Lichess API
type LichessResponse struct {
	NowPlaying []LichessGame
}
