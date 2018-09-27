
var Menubar = function ( editor ) {
	var signals = editor.signals;
	var container = new UI.Panel();
	container.setId( 'menubar' );

	container.add( new Menubar.File( editor ) );
	// container.add( new Menubar.Edit( editor ) );
	// container.add( new Menubar.Add( editor ) );
	// container.add( new Menubar.Play( editor ) );
	// // container.add( new Menubar.View( editor ) );
	// container.add( new Menubar.Examples( editor ) );
	container.add( new Menubar.Help( editor ) );

	// container.add( new Menubar.Status( editor ) );


	// var sidebarControl = new UI.Checkbox();
	// sidebarControl.setId('sideToggle');

	var sidebarControl = document.createElement('input');
	sidebarControl.setAttribute('type','checkbox');
	sidebarControl.setAttribute('id','sideToggle');
	document.body.appendChild( sidebarControl );


	// var wrap = new UI.Button( ).setRight();
	// wrap.setId('wrap');
	// wrap.setStyle('padding','0px');

	// var label = new UI.Label('=');
	// label.setAtr('for','sideToggle');
	// label.setId('sideMenuControl');
	// wrap.add(label);



	// container.add (wrap );



	return container;

};