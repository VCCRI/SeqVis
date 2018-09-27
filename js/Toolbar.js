
var Toolbar = function ( editor ) {

	var signals = editor.signals;
	
	var container = new UI.Panel();
	container.setId( 'toolbar' );

	var buttons = new UI.Panel();
	container.add( buttons );


	// three scenes in a row
	var two = new UI.Button(  ).setWidth('40px').setHeight('24px');
	var currSelected = two;
	var currRules = Config.sceneSize.two;
	two.dom.classList.add( 'selected' );
	two.dom.style['background-image'] = 'url(./image/icon_twoView.png)';
	two.onClick( function () {
    	updateSceneSizeUI(2);
	} );
		buttons.add( two );

	// three scenes in a row
	var three = new UI.Button( ).setWidth('40px').setHeight('24px');
	// var currSelected = three;
	// var currRules = Config.sceneSize.three;
	three.dom.style['background-image'] = 'url(./image/icon_threeView.png)';
	three.onClick( function () {
    	updateSceneSizeUI(3);
	} );
	buttons.add( three );

	var four = new UI.Button( ).setWidth('40px').setHeight('24px');
	four.dom.style['background-image'] = 'url(./image/icon_fourView.png)';
	four.onClick( function () {
		updateSceneSizeUI(4);
	} );
	buttons.add( four );

	signals.editorCleared.add(function(){
		two.dom.classList.add( 'selected' );
		three.dom.classList.remove( 'selected' );
		four.dom.classList.remove( 'selected' );
	});

	function updateSceneSizeUI ( type ) {

		
		two.dom.classList.remove( 'selected' );
		three.dom.classList.remove( 'selected' );
		four.dom.classList.remove( 'selected' );

		switch ( type ) {

			case 2: 
				rules = Config.sceneSize.two;
				currSelected = two;
				break;
			case 3: 
				rules = Config.sceneSize.three;
				currSelected = three; 
				break;
			case 4: 
				rules = Config.sceneSize.four;
				currSelected = four; 
				break;

		}

		currSelected.dom.classList.add( 'selected' );
		currRules = rules;
		editor.displayAllScenes();
		editor.setSceneSize(rules);
	
		editor.fullScreenMode = 0;
		// var currButton = three;
		// var preButton = four;
		// var rules = Config.sceneSize.three;
		// if( type == 1 ) {
		// 	currButton = four;
		// 	preButton = three;
		// 	rules = Config.sceneSize.four;
		// }
	
		// if (currButton.dom.classList.contains( 'selected' )) {
			
		// 	return;
	
		// }
		// else {
		// 	editor.setSceneSize(rules);
		// 	editor.displayAllScenes();
		// 	editor.fullScreenMode = 0;
		// 	currButton.dom.classList.add( 'selected' );
		// 	currSelected = currButton;
		// 	currRules = rules;
		// 	preButton.dom.classList.remove( 'selected' );
		// }

	}



	var playButton = new UI.Button(  ).setMarginLeft('20px').setWidth('36px').setHeight('24px');
	playButton.dom.style['background-image'] = 'url(./image/icon_play.png)';
	playButton.onClick( function () {
		if (playButton.dom.classList.contains( 'selected' )) {
			playButton.dom.classList.remove( 'selected' );
			signals.stopAnimations.dispatch( );
			playButton.dom.style['background-image'] = 'url(./image/icon_play.png)';

		}
		else{
			playButton.dom.classList.add( 'selected' );
			signals.animationRequired.dispatch( );
			playButton.dom.style['background-image'] = 'url(./image/icon_pause.png)';
		}
		// signals.selectionModeChanged.dispatch( 0 );
	} );
	buttons.add( playButton );
	
	
	

	
	var lockButton = new UI.Button(  ).setWidth('36px').setHeight('24px');
	lockButton.dom.classList.add( 'selected' );
	lockButton.dom.style['background-image'] = 'url(./image/icon_lock.png)';

	lockButton.onClick( function () {
		if(editor.lockMode == 0) {
			editor.lockMode = 1;
			lockButton.dom.classList.add( 'selected' );
			lockButton.dom.style['background-image'] = 'url(./image/icon_lock.png)';
			
		}
		else {
			editor.lockMode = 0;
			lockButton.dom.classList.remove( 'selected' );
			lockButton.dom.style['background-image'] = 'url(./image/icon_unlock.png)';
			
		}
	} );
	
	buttons.add( lockButton );

	var screenShotButton = new UI.Button(  ).setWidth('36px').setHeight('24px');
	screenShotButton.dom.style['background-image'] = 'url(./image/icon_camera.png)';
	screenShotButton.onClick( function () {
		
		signals.screenShot.dispatch( );
	} );
	
	buttons.add( screenShotButton );

	// var singleSelection = new UI.Button( 'Single Selection' );
	// singleSelection.onClick( function () {
    //     signals.selectionModeChanged.dispatch( 1 );
	// } );
	// buttons.add( singleSelection );


	// signals.selectionModeChanged.add( function( type ){

	// 	crossSelection.dom.classList.remove( 'selected' );
	// 	singleSelection.dom.classList.remove( 'selected' );
	

	// 	switch ( type ) {

	// 		case 0: crossSelection.dom.classList.add( 'selected' ); break;
	// 		case 1: singleSelection.dom.classList.add( 'selected' ); break;

	// 	}
	// });
	signals.editorCleared.add(function(){
		editor.lockMode = 1;
		lockButton.dom.classList.add( 'selected' );
	});
	signals.fullScreenMode.add( function(bool, scene){

		if( bool == true ) {
			editor.hideOtherScenes(scene);
			currSelected.dom.classList.remove( 'selected' );
			signals.renderRequired.dispatch();
		}
		else {
			editor.displayAllScenes();
			editor.setSceneSize( currRules );
			currSelected.dom.classList.add( 'selected' );
		
		}

	});

	return container;

};


