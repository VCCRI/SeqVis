
Sidebar.Settings = function ( editor ) {


	var container = new UI.Panel();
	var signals = editor.signals;

	container.add( new UI.Break(),new UI.Break());
	// class

	var options = {
		'0': 'colored',
		'1': 'black and white'
	};

	var themeRow = new UI.Row();
	var theme = new UI.Select().setWidth( '150px' );
	theme.setOptions( options ).setValue('0');

	theme.onChange( function () {

		signals.changeColorScheme.dispatch(theme.getValue());

	} );

	themeRow.add( new UI.Text( 'Theme' ).setWidth( '100px' ) );
	themeRow.add( theme );

	container.add( themeRow );
	
	
	var pointSizeRow = new UI.Row();
	var pointSizeSlider = new UI.Slider(1,10,10);

	pointSizeRow.add(new UI.Text( 'Set point size' ).setWidth( '100px' ) );
	pointSizeRow.add( pointSizeSlider );
	
	container.add(pointSizeRow);
	
	signals.editorCleared.add(function(){
		theme.setValue('0');
		editor.colorScheme = 0;
	});
	
	pointSizeSlider.dom.oninput = function() {
		
		signals.pointSizeChanged.dispatch(this.value/500);

	};

	return container;

};