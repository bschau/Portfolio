function load() {
	const elems = document.getElementsByClassName('funko-pop-image');
	for (var i = 0; i < elems.length; i++) {
		elems[i].addEventListener('click', function() {
			const img = this.src.replace('thumbs/', 'imgs/');
			const geometry = this.getAttribute('data-geometry').split(",");
			this.width = geometry[0];
			this.height = geometry[1];
			this.src = img;
		});
	}
}
