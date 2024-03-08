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

my $comics = "";
my $goc_date = strftime "%Y/%m/%d", localtime;

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
$blocks
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
		return "";
	}

	return $res->decoded_content;
}

sub generic {
	my ($origin, $token, $title, $url) = @_;

	my $page = fetch($url);
	my $pos = index($page, $origin);
	if ($pos < 0) {
		return;
	}

	$pos = index($page, $token, $pos);
	if ($pos < 0) {
		return;
	}

	my $start_pos = $pos + length($token);
	my $end_pos = index($page, '"', $start_pos);
	my $link = substr($page, $start_pos, $end_pos - $start_pos);
	my $entry = << "EOF";
<h1><a href="${url}">${title}</a></h1>
<p><img src="${link}" alt="${title}" width="900" /></p>
EOF

	$comics = "${comics}${entry}";
}

sub creators {
	my ($title, $slug) = @_;

	my $url = "https://www.creators.com/read/${slug}";
	generic('class="fancybox"', 'img src="', $title, $url);
}

sub kingdom {
	my ($title, $slug) = @_;

	my $url = "https://comicskingdom.com/${slug}/";
	generic('img id="theComicImage"', 'src="', $title, $url);
}

sub go_comics {
	my ($title, $slug) = @_;

	my $url = "https://www.gocomics.com/${slug}/${goc_date}";
	generic('item-comic-image', 'src="', $title, $url)
}

creators('Andy Capp', 'andy-capp');
creators('B.C.', 'bc');
kingdom('Beetle Bailey', 'beetle-bailey');
go_comics('Betty', 'betty');
go_comics('Broom Hilda', 'broomhilda');
go_comics('Calvin and Hobbes', 'calvinandhobbes');
kingdom('Crock', 'crock');

my $date = strftime "%Y-%m-%d", localtime;
my $url = "https://dilbert.com/strip/${date}";
generic('img-responsive img-comic', 'src="', 'Dilbert', $url);

generic('div class="MainComic__ComicImage', 'img src="', 'Explosm', 'http://explosm.net');
go_comics('Garfield', 'garfield');
kingdom('Hagar the Horrible', 'hagar-the-horrible');
go_comics('Pearls before Swine', 'pearlsbeforeswine');
kingdom('Sam and Silo', 'sam-and-silo');
kingdom('Zits', 'zits');

my $title = strftime "Comics %F %T", localtime;
my $html = get_html($title, $comics);
send_email($title, $html);
