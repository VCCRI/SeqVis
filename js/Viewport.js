var Viewport = function ( editor ) {

	var signals = editor.signals;
	var canvas = editor.canvas;
	var animationLoopId;
	var fullScreenScene = editor.fullScreenScene;
 	var container  = this.container = editor.viewport;
	container.onscroll = function (e) {
		editor.signals.renderRequired.dispatch();
	};
	var scenes = editor.scenes;

	var svgRenderer = new THREE.SVGRenderer();
 	
	svgRenderer.setClearColor( 0x000000 );

	var renderer =  new THREE.WebGLRenderer( { canvas: canvas, antialias: false } );
	renderer.setClearColor( 0xffffff, 1 );
	renderer.setPixelRatio( window.devicePixelRatio );
	renderer.autoClear = false;
	renderer.autoUpdateScene = false;

	var sceneHelpers = editor.sceneHelpers;

	//SIGNALS
	signals.addScene.add(function (sceneName, data,dim, axesNames) {
		editor.addScene(sceneName, data,dim, axesNames);
	});
	
	signals.pointSizeChanged.add(function(size){
		editor.changePointSize(size);
	});

	signals.sceneResize.add(function(){

		sceneResize();
		renderAll();

	});

	signals.windowResize.add( function () {

		sceneResize();
		var width = canvas.clientWidth;
		var height = canvas.clientHeight;
		if ( canvas.width !== width || canvas.height !== height ) {
			renderer.setSize( width, height, false );
		}
		renderAll();

	} );

	signals.renderRequired.add( function () {

		renderAll();

	} );

	signals.animationRequired.add( function () {
		editor.animationMode = 1;
		editor.createAnimations();
		animate();

	} );
	signals.stopAnimations.add( function () {
	// editor.startAnimations();
		editor.animationMode = 0;
		stopAnimate();
	} );
	
	
	signals.changeColorScheme.add( function ( type ) {
			
		editor.colorScheme = type;

		for ( var scene of scenes ) {
			if(type == 0 ) scene.background =  Config.colors.SCENEDARK;
			else scene.background =  Config.colors.SCENELIGHT;
			
			var wireframe = scene.children[1].children[0].children[0];
			//editor.changeBufferGeometryColorScheme(wireframe,type);
				
			editor.changeLineGeometryColorScheme(wireframe,type);
			
			var axes = scene.children[1].children[0].children[1].children[0];
			editor.changeLineGeometryColorScheme(axes,type);
			
			var labels = scene.children[1].children[0].children[1].children[1];
			editor.changeSpriteColorScheme(labels,type);
			
			renderAll();
		}
		
	} );



	signals.selectionModeChanged.add( function (type) {

		editor.selectionMode = type;
	});
	

	signals.objectAdded.add( function ( object ) {

		object.traverse( function ( child ) {

			objects.push( child );

		} );

	} );

	signals.screenShot.add( function() {
		
		// var w = window.open('', '');
		renderAll();

		renderer.domElement.toBlob(function(blob){
			var a = document.createElement('a');
			var url = URL.createObjectURL(blob);
			a.href = url;
			a.download = 'scenes.png';
			a.click();
		}, 'image/png', 1.0);

		// var imgData = renderer.domElement.toDataURL();
		// var img = new Image();
		// img.src = imgData;
		// w.document.body.appendChild(img);  

	} );

	signals.takeSvgImage.add( function(scene) {

		//clone scene
		var svgScene = new THREE.Scene();
		svgScene.background = Config.colors.SCENELIGHT;
		svgScene.add( new THREE.AmbientLight( 0x404040 ) );
		svgScene.add(editor.drawGraph(scene, 'svg', scene.userData.graphType.axesNames));
	
		var XMLS = new XMLSerializer();
		svgRenderer.setSize( scene.userData.element.clientWidth*5, scene.userData.element.clientHeight*5 );
		// svgRenderer.setSize( canvas.width, canvas.height, false );

		svgRenderer.clear();
		svgRenderer.setQuality('high');

		svgRenderer.render( svgScene, scene.userData.camera );
		var svgData = XMLS.serializeToString(svgRenderer.domElement);
		save(new Blob([svgData], {type:"image/svg+xml;charset=utf-8"}),scene.userData.sceneName+'.svg');
	} );


	signals.hideChild.add( function(parent, childName) {

		var child;
		if(childName == 'wireframe') child = parent.children[1].children[0].children[0];
		
		else if(childName == 'axes') child = parent.children[1].children[0].children[1].children[0];
		
		else if(childName == 'labels') child = parent.children[1].children[0].children[1].children[1];
		
		if( child.visible == true) child.visible = false;
		else child.visible = true;
		renderAll();
	
	} );

	function sceneResize() {

		scenes.forEach( function( scene ) {
			var camera = scene.userData.camera;
			var dom = scene.userData.element;
			camera.aspect = dom.offsetWidth / dom.offsetHeight;
			camera.updateProjectionMatrix();
	
		});

	}


	function stopAnimate() {

		cancelAnimationFrame( animationLoopId );

	}


	function animate(timestamp) {
	
		animationLoopId = requestAnimationFrame( animate );
		TWEEN.update(timestamp);
		renderAll();
	}

	function renderAll() {

		renderer.setClearColor( 0xffffff );
		renderer.setScissorTest( false );
		renderer.clear();
		renderer.setClearColor( 0xe0e0e0 );
		renderer.setScissorTest( true );
		scenes.forEach( function( scene ) {

			render(scene);
		});
		

	}
	
	
	function render(scene){
		var element = scene.userData.element;
		if(element.style.display == 'none') return;
		// get its position relative to the page's viewport
		var rect = element.getBoundingClientRect();
		// check if it's offscreen. If so skip it
		if ( rect.bottom < 0 || rect.top  > renderer.domElement.clientHeight ||
		rect.right  < 0 || rect.left > renderer.domElement.clientWidth ) {
		return;  // it's off screen
		}
		// set the viewport
		var width  = rect.right - rect.left;
		var height = rect.bottom - rect.top;
		var left   = rect.left;
		var top    = rect.top;
		renderer.setViewport( left, top, width, height );
		renderer.setScissor( left, top, width, height );
		var camera = scene.userData.camera;
		//camera.aspect = width / height; // not changing in this example
		//camera.updateProjectionMatrix();
		// scene.userData.orbitControls.update();
		renderer.render( scene, camera );
		
		
		
	}

};


var link = document.createElement( 'a' );
link.style.display = 'none';
document.body.appendChild( link ); 

function save( blob, filename ) {

	link.href = URL.createObjectURL( blob );
	link.download = filename || 'scene.svg';
	link.click();

	// URL.revokeObjectURL( url ); breaks Firefox...

}