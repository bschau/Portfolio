<?php
class wMailQueue {
	private $base = false;
	private $queue = "/mailqueue";
	public function getQueue() { return $this->queue; }

	private $subject = "";
	public function setSubject($v) { $this->subject = $v; }
	public function getSubject() { return $this->subject; }

	private $from = "<>";
	public function setFrom($v) { $this->from = $v; }
	public function getFrom() { return $this->from; }

	private $content = "text/html; charset=ISO-8859-1";
	public function setContent($v) { $this->content = $v; }
	public function getContent() { return $this->content; }

	private $mimeVersion = "MIME-Version: 1.0";
	public function setMimeVersion($v) { $this->mimeVersion = $v; }
	public function getMimeVersion() { return $this->mimeVersion; }

	private $params = "";
	public function setParams($v) { $this->params = $v; }
	public function getParams() { return $this->params; }

	private $mailBodyHeader = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\r\n<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"@@CONTENT@@\" />\r\n<title>@@TITLE@@</title>\r\n</head>\r\n<body>\r\n";
	public function setMailBodyHeader($v) { $this->mailBodyHeader = $v; }
	public function getMailBodyHeader() { return $this->mailBodyHeader; }

	private $mailBodyFooter = "\r\n</body>\r\n</html>\r\n";
	public function setMailBodyFooter($v) { $this->mailBodyFooter = $v; }
	public function getMailBodyFooter() { return $this->mailBodyFooter; }

	private $mailBody = "";
	public function setMailBody($v) { $this->mailBody= $v; }
	public function getMailBody() { return $this->mailBody; }

	private $recipients = false;
	public function addRecipient($v) {
		if ($this->recipients === false) {
			$this->recipients = array();
		}

		$this->recipients[$v] = $v;
	}
	public function getRecipients() { return $this->recipients; }

	function __construct() {
		$this->base = strtr(microtime(), array("." => "-", " " => "-"));

		$mq = get_option('mailqueue');
		$this->queue = $mq[queue];
	}

	function enqueue() {
		$this->enqueueMessageFile();
		$this->enqueueRecipientsFile();
		$this->enqueueControlFile();
	}

	function getMessageFilePath() { return $this->getQueue() . "/m" . $this->base; }

	function enqueueWithMessageFile() {
		$this->enqueueRecipientsFile();
		$this->enqueueControlFile();
	}

	public function expandMailBody() {
		if ($this->content === false) {
			$this->content = "";
		}

		if ($this->subject === false) {
			$this->subject = "";
		}

		$search = array('@@CONTENT@@', '@@TITLE@@');
		$replace = array(htmlentities($this->content), htmlentities($this->subject));

		$this->mailBodyHeader = str_replace($search, $replace, $this->mailBodyHeader);
		$this->mailBodyFooter = str_replace($search, $replace, $this->mailBodyFooter);
		$this->mailBody = str_replace($search, $replace, $this->mailBody);
	}

	private function enqueueMessageFile() {
		$this->expandMailBody();
		$fh = fopen($this->getQueue() . "/m" . $this->base, "wb+");
		fwrite($fh, $this->mailBodyHeader);
		fwrite($fh, $this->mailBody);
		fwrite($fh, $this->mailBodyFooter);
		fclose($fh);
	}

	private function enqueueRecipientsFile() {
		$fh = fopen($this->getQueue() . "/t" . $this->base, "wb+");
		fwrite($fh, serialize($this->recipients));
		fclose($fh);
	}

	private function enqueueControlFile() {
		$hdr = "From: " . $this->from . "\r\n";
		if (strlen(trim($this->mimeVersion)) > 0) {
			$hdr .= $this->mimeVersion . "\r\n";
		}

		if (strlen(trim($this->content)) > 0) {
			$hdr .= "Content-Type: " . $this->content . "\r\n";
		}

		$msg = array();

		$msg["subject"] = $this->subject;
		$msg["header"] = $hdr;
		$msg["params"] = $this->params;

		$fh = fopen($this->getQueue() . "/_c" . $this->base, "wb+");
		fwrite($fh, serialize($msg));
		fclose($fh);

		rename($this->getQueue() . "/_c" . $this->base, $this->getQueue() . "/c" . $this->base);
	}
}
?>
