# Install:
#	apt install libssl-dev
#	File::HomeDir, LWP::Simple, HTTP::Request, MIME::Lite
#	Bundle::LWP, LWP::Protocol::https
use strict;
use File::HomeDir;
use LWP::Simple;
use LWP::Protocol::https;
use HTTP::Request;
use MIME::Lite;
use POSIX qw(strftime);

my $explosm_url = 'https://explosm.net';

sub get_html {
	my ($title, $link) = @_;

	return <<"EOF";
<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
    <title>$title</title>
</head>
<body>
<p><img src="${link}" alt="Explosm" width="900" /></p>
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

sub fetch {
	my ($url) = @_;

	my $ua = LWP::UserAgent->new;
	my $request = HTTP::Request->new(GET => $url);
	$request->header("User-Agent" => "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:55.0) Gecko/20100101 Firefox/55.0");
	my $res = $ua->request($request);
	if (!$res->is_success) {
		print($res->decoded_content);
		return "";
	}

	return $res->decoded_content;
}

sub get_explosm_link {
	my $origin = 'div class="MainComic__ComicImage';
	my $token = 'img src="';
	my $page = fetch($explosm_url);
	my $pos = index($page, $origin);
	if ($pos < 0) {
		exit;
	}

	$pos = index($page, $token, $pos);
	if ($pos < 0) {
		return;
	}

	my $start_pos = $pos + length($token);
	my $end_pos = index($page, '"', $start_pos);
	return substr($page, $start_pos, $end_pos - $start_pos);
}

my $link = get_explosm_link();
my $title = strftime "Explosm %F %T", localtime;
my $html = get_html($title, $link);
send_email($title, $html);
