<?php
function QuotedPrintableEncode($input, $lineMax=76, $spaceConv=true)
{
	$hex=array('0','1','2','3','4','5','6','7','8','9', 'A','B','C','D','E','F');
	$lines=preg_split("/(?:\r\n|\r|\n)/", $input);
	$eol="\r\n";
	$escape="=";
	$output="";

	while (list(,$line)=each($lines)) {
		$line=rtrim($line);
		$lineLen=strlen($line);
		$newLine="";
		for ($i=0; $i<$lineLen; $i++) {
			$c=substr($line, $i, 1);
			$dec=ord($c);
			if (($i==0) && ($dec==46))
				$c="=2E";

			if ($dec==32) {
				if ($i==($lineLen-1))
					$c="=20";
				else if ($spaceConv)
					$c="=20";
			} elseif (($dec==61) || ($dec<32) || ($dec>126)) {
                               $h2=floor($dec/16);
                               $h1=floor($dec%16);
                               $c=$escape.$hex["$h2"].$hex["$h1"];
			}

			if ((strlen($newLine)+strlen($c))>=$lineMax) {
				$output.= $newLine.$escape.$eol;
				$newLine="";
				if ($dec==46)
					$c="=2E";
			}
			$newLine.=$c;
		}
		$output.=$newLine.$eol;
	}
	return trim($output);
}
?>
