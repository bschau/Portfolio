# Install:
#	apt install libssl-dev
#	File::HomeDir, LWP::Simple, HTTP::Request, MIME::Lite, JSON
#	Bundle::LWP, LWP::Protocol::https
use strict;
use File::HomeDir;
use LWP::Simple;
use LWP::Protocol::https;
use HTTP::Request;
use MIME::Lite;
use JSON;
use POSIX qw(strftime);

sub get_token {
	my $tokenFile = File::HomeDir->my_home . "/.lichesst";
	open my $file, '<', $tokenFile;
	my $token = <$file>;
	close $file;
	$token =~ s/^\s+|\s+$//g;
	return $token;
}

sub get_lichess_json {
	my ($token) = @_;
	my $url = "https://lichess.org/api/account/playing";
	my $ua = LWP::UserAgent->new;
	my $request = HTTP::Request->new(GET => $url);
	$request->header("User-Agent" => "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:55.0) Gecko/20100101 Firefox/55.0");
	$request->header("Authorization" => "Bearer ${token}");
	my $res = $ua->request($request);
	if (!$res->is_success) {
		exit;
	}

	return decode_json($res->decoded_content);
}

sub get_games {
	my ($title, @nowPlaying) = @_;

	my $block = "";
	foreach my $game (@nowPlaying) {
		next if (!$game->{'isMyTurn'});
		my $opponent = $game->{'opponent'};
		my $name = $opponent->{'username'};
		my $gameId = $game->{"gameId"};
		$block = "${block}<li><i>${name}</i>: <a href=\"https://lichess.org/${gameId}\">Go to game</a></li>";
	}

	return $block;
}

sub get_html {
	my ($title, $blocks) = @_;

	return <<"EOF";
<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
    <title>$title</title>
</head>
<body>
<p>My turn in game vs.:</p>
<ul>
$blocks
</ul>
</body>
</html>
EOF
}

sub send_email {
	my ($title, $html) = @_;

	MIME::Lite->send('smtp', 'localhost', Timeout => 60, Port => 25);
	my $msg = MIME::Lite->new(
		"From" => 'bs@leah.schau.dk',
		"To" => 'brian@schau.dk',
		"Subject" => $title,
		"Data" => $html
	);
	$msg->attr("Content-Type" => "text/html");
	$msg->send();
}

my $token = get_token();
my $text = get_lichess_json($token);
my @nowPlaying = @{$text->{'nowPlaying'}};
if (!@nowPlaying) {
	exit;
}

my $title = strftime "Lichess %F %T", localtime;
my $blocks = get_games($title, @nowPlaying);
if (length($blocks) == 0) {
	exit;
}

my $html = get_html($title, $blocks);

send_email($title, $html);
