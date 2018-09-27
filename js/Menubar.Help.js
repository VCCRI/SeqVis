Menubar.Help = function ( editor ) {

	var container = new UI.Panel();
	container.setClass( 'menu' );

	var title = new UI.Panel();
	title.setClass( 'title' );
	title.setTextContent( 'Help' );
	container.add( title );

	var options = new UI.Panel();
	options.setClass( 'options' );
	container.add( options );


	// About

	var option = new UI.Row();
	option.setClass( 'option' );
	option.setTextContent( 'About' );
	option.onClick( function () {


	} );
	options.add( option );

	return container;

};