<?php
	require_once('cache.inc.php');

function MaintenanceDelete()
{
	if (isset($_POST['deleteUid'])==true && is_array($_POST['deleteUid']) && count($_POST['deleteUid'])>0) {
		if (($mBox=MBOpen())!==false) {
			while (list($k, $v)=each($_POST['deleteUid']))
				@imap_delete($mBox, $v);

			@imap_expunge($mBox);
			MBClose();
		}
	}
}

?>
