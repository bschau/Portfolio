class Storage {
	constructor() {
		const localStorageSupported = typeof window['localStorage'] != "undefined" && window['localStorage'] != null;
		if (localStorageSupported === false) {
			throw new Error("Local Storage is not supported in your browser.");
		}
	}

	set(key, item) {
		localStorage.setItem(key, item);
	}

	get(key) {
		return localStorage.getItem(key);
	}

	exists(key) {
		return (!!localStorage.getItem(key));
	}

	remove(key) {
		localStorage.removeItem(key);
	}
}
