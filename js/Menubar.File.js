
Menubar.File = function ( editor ) {
	var signals = editor.signals;
	var NUMBER_PRECISION = 6;

	function parseNumber( key, value ) {

		return typeof value === 'number' ? parseFloat( value.toFixed( NUMBER_PRECISION ) ) : value;

	}


	var container = new UI.Panel();
	container.setClass( 'menu' );

	var title = new UI.Panel();
	title.setClass( 'title' );
	title.setTextContent( 'Menu' );
	container.add( title );

	var options = new UI.Panel();
	options.setClass( 'options' );
	container.add( options );


	// UPLOAD FILE
	var form = document.createElement( 'form' );
	form.style.display = 'none';
	document.body.appendChild( form );
	var fileInput = document.createElement( 'input' );
	fileInput.type = 'file';
	fileInput.accept=".nex,.phy,.fasta";
	fileInput.addEventListener( 'change', function ( event ) {

		signals.fileLoaded.dispatch(fileInput.files[ 0 ],'');
		// console.log
		fileInput.value = null;
		form.reset();
		console.log(fileInput.files);

	} );
	form.appendChild(fileInput);
	var upload = new UI.Row();
	upload.setClass( 'option' );
	upload.setTextContent( 'Upload' );
	upload.onClick( function () {

		fileInput.click();


	} );
	options.add( upload );


	
	return container;



};

