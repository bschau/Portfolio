/**
 * Unicode to base64 ascii.
 */
function utoa(raw) {
	return btoa(unescape(encodeURIComponent(raw)));
}

/**
 * Base64 ascii to Unicode.
 */
function atou(raw) {
	return decodeURIComponent(escape(atob(raw)));
}
