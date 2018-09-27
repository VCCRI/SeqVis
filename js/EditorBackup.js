
var ROOTTHREE = 1.7321;
var ROOTSIX = 2.4495;

var Editor = function (  ) {


	this.fullScreenMode = 0;
	this.canvas = document.createElement('canvas');
	this.canvas.setAttribute('id', 'c');
	document.body.appendChild(this.canvas);

	styleEl = document.createElement('style');
	document.head.appendChild(styleEl);
	this.styleSheet = styleEl.sheet;
	this.styleSheet.insertRule(Config.sceneSize.three,0);

	this.viewport = document.createElement('div');
	this.viewport.setAttribute('id','content');

	document.body.appendChild(this.viewport);

	this.addNewEdgeMode = 0;
	this.inputData  = null;

	var Signal = signals.Signal;

	this.signals = {


		editorCleared: new Signal(),

		savingStarted: new Signal(),
		savingFinished: new Signal(),
		loadDataUrl: new Signal(),
	

		refreshAvaiableGames: new Signal(),
		windowResize: new Signal(),

		cameraChanged: new Signal(),
		objectSelected: new Signal(),
		objectAdded: new Signal(),
		objectChanged: new Signal(),

		refreshSidebarObjectProperties: new Signal(),

		addScene: new Signal(),
		setSceneSize: new Signal(),
		deleteScene: new Signal(),	
		fullScreenMode: new Signal(),
		renderRequired: new Signal(),
		fileLoaded: new Signal(),
		dataPrepared: new Signal(),

		hideChild: new Signal()
	};


	this.scenes = [];

	this.fullScreenScene = null;

};

Editor.prototype = {

	deleteScene:function(listItem,uuid){
	
		var index = 0;
		console.log(this.scenes.length);
		for( var scene of this.scenes){
			if(scene.uuid != uuid) index += 1;
			else break;
		}
		this.scenes.splice(index, 1);
		this.viewport.removeChild(listItem);
		this.signals.renderRequired.dispatch();
	
		// this.scenes;
	},

	hideOtherScenes:function(scene){
		

		for ( var currScene of this.scenes ) {

			if (currScene.uuid != scene.uuid ) {
				// console.log(scene)
				currScene.userData.element.parentElement.style.display = 'none';
			}
			this.setSceneSize(Config.sceneSize.fullScreen);
			this.signals.renderRequired.dispatch();
			
		}

	},

	displayAllScenes:function() {
		for ( var currScene of this.scenes ) {
			currScene.userData.element.parentElement.style.display = '';
		}
		this.signals.renderRequired.dispatch();
	},

	createSceneContainer:function(sceneName,scene){

		scope = this;
		var listItem = document.createElement('div');
		listItem.setAttribute('class','list-item');

		var sceneContainer = document.createElement('div');
		sceneContainer.setAttribute('class','scene');

		var footRow = document.createElement('div');
		footRow.setAttribute('class','foot');
		var description = document.createElement('div');
		description.setAttribute('class','description');
		description.innerText = sceneName;
		footRow.appendChild(description);
		

		var toggleAxesButton = document.createElement('button');
		toggleAxesButton.setAttribute('class','optionButton');
		toggleAxesButton.style['background-image'] = 'url(/image/axes.png)';
		footRow.appendChild(toggleAxesButton).onclick = function() {
			scope.signals.hideChild.dispatch(scene, 'axes');
		};;

		var toggleWireframeButton = document.createElement('button');
		toggleWireframeButton.setAttribute('class','optionButton');
		toggleWireframeButton.style['background-image'] = 'url(/image/triangle.png)';
		footRow.appendChild(toggleWireframeButton);
		toggleWireframeButton.onclick = function() {
			scope.signals.hideChild.dispatch(scene, 'wireframe');
		};


		var toggleLabelButton = document.createElement('button');
		toggleLabelButton.setAttribute('class','optionButton');
		toggleLabelButton.style['background-image'] = 'url(/image/acgt.png)';
		footRow.appendChild(toggleLabelButton);

		var zoominButton = document.createElement('button');
		zoominButton.setAttribute('class','optionButton');
		// zoominButton.innerText ='full screen';
		zoominButton.style['background-image'] = 'url(/image/resize.png)';
		
		zoominButton.onclick = function(){
			if ( scope.fullScreenMode == 0 ) {
				scope.fullScreenMode = 1;
				scope.signals.fullScreenMode.dispatch(true, scene);
			}
			else {
				scope.fullScreenMode = 0;
				scope.signals.fullScreenMode.dispatch(false, null);
			}
				
		};
		footRow.appendChild(zoominButton);

		var closeButton = document.createElement('button');
		closeButton.setAttribute('class','optionButton');
		// closeButton.innerText =String.fromCodePoint(0x2716);
		closeButton.style['background-image'] = 'url(/image/cross.png)';
		closeButton.onclick = function(){
			//TODO signal based or not
			scope.deleteScene(listItem,scene.uuid);
		};

		footRow.appendChild(closeButton);

		listItem.appendChild(sceneContainer);
		listItem.appendChild(footRow);

		return listItem;

	},

	drawGraph : function( data, dim ) {
		
		var graphGroup = new THREE.Group();
		if(dim.length == 2) graphGroup.position.set(-0.5, 0, 0);
		if(dim.length == 3) graphGroup.position.set(-0.5, -ROOTTHREE/6, 0);
		if(dim.length == 4) graphGroup.position.set(-0.5, -ROOTTHREE/9, -ROOTTHREE/6);

		graphGroup.add(this.drawWireframe( dim.length-1 ));
		graphGroup.add(this.drawAxes( dim ));
		graphGroup.add(this.drawData( data ));
		return graphGroup;	
		
	}, 
	

	drawWireframe : function ( dim ) {
		var group = new THREE.Group();
		group.position.set(0,0,0);
		group.name = 'wireframe';
		var geometry = new THREE.BufferGeometry();
		var material = new THREE.LineBasicMaterial({ linewidth:5, color: 0xffffff, vertexColors: THREE.VertexColors });
		var vertices;
		var colors;

		// DRAW GRAPH SHAPES
		if(dim == 1) {
		
			vertices = new Float32Array( [
				//l-r
				0, 0, 0,
				1, 0, 0
			] );
			
			colors = new Float32Array( [
				1, 0, 0,
				0, 0, 1
			] );	
			
		}
		
		else if(dim == 2) {

			vertices = new Float32Array( [
				//t-l
				0.5, ROOTTHREE/2, 0,
				0, 0, 0,
				//t-r
				0.5, ROOTTHREE/2, 0,
				1, 0, 0,
				//l-r
				0, 0, 0,
				1, 0, 0
			] );
			
			colors = new Float32Array( [
				0, 1, 0, 
				1, 0, 0,
				
				0, 1, 0, 
				0, 0, 1,				
				
				1, 0, 0,
				0, 0, 1
			] );
	

		}
		
		else if(dim == 3) {
		
			vertices = new Float32Array( [
				//t-l
				0.5, ROOTSIX/3, ROOTTHREE/6,
				0, 0, 0,
				//t-r
				0.5, ROOTSIX/3, ROOTTHREE/6,
				1, 0, 0,
				//t-f
				0.5, ROOTSIX/3, ROOTTHREE/6,
				0.5, 0, ROOTTHREE/2,
				//l-r
				0, 0, 0,
				1, 0, 0,
				//l-f
				0, 0, 0,
				0.5, 0, ROOTTHREE/2,
				//r-f
				1, 0, 0,
				0.5, 0, ROOTTHREE/2
			

			] );
			
			colors = new Float32Array( [
				0, 1, 0, 
				1, 0, 0,
				
				0, 1, 0, 
				0, 0, 1,
				
				0, 1, 0, 		
				0.5, 0.5, 0,
				
				1, 0, 0,
				0, 0, 1,
				
				1, 0, 0,
				0.5, 0.5, 0,
				
				0, 0, 1,
				0.5, 0.5, 0
			] );
			
		}
		
		geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
		geometry.addAttribute( 'color', new THREE.BufferAttribute( colors, 3 ) );
	
		group.add( new THREE.LineSegments( geometry, material ));
		return group;
	},
	calcuateT: function(start,center){
		var vec = new THREE.Vector3().subVectors(start,center).normalize().multiplyScalar(0.1);
		var res = center.add(vec);
		console.log(res);
		return res;


	},
	calcuateLabel: function(start,center){
		var vec = new THREE.Vector3().subVectors(start,center).normalize().multiplyScalar(0.1);
		var res = start.add(vec);
		console.log(res);
		return res;

	},
	drawAxes: function ( dim ) {

		var group = new THREE.Group();
		group.position.set(0,0,0);
		group.name = 'axes';

		if (dim.length <= 2)  return group;
		var geometry = new THREE.BufferGeometry();
		var material = new THREE.LineBasicMaterial({ linewidth:5, color: 0xffffff, vertexColors: THREE.VertexColors });
		var vertices;
		var colors;
		this.calcuateLabel(new THREE.Vector3(0, 0, 0),new THREE.Vector3(0.5, ROOTTHREE/6, 0));
		this.calcuateLabel(new THREE.Vector3(1, 0, 0),new THREE.Vector3(0.5, ROOTTHREE/6, 0));
		this.calcuateLabel(new THREE.Vector3(0, 0, 0),new THREE.Vector3(0.5, ROOTTHREE/6, 0));

		if(dim.length == 3  ){
			vertices = new Float32Array( [
				
				0.5, ROOTTHREE/6, 0,
				0, 0, 0,

				0.5, ROOTTHREE/6, 0,
				1.0, 0, 0,
			
				0.5, ROOTTHREE/6, 0,
				0.5, ROOTTHREE/2, 0,

			] );

			colors = new Float32Array( [
				1, 0, 0, 
				1, 0, 0, 

				0, 0, 1,
				0, 0, 1,

				0, 1, 0,					
				0, 1, 0
			] );
	


		}

		else if( dim.length == 4 ){

			vertices = new Float32Array( [
				
				0.5, ROOTSIX/9, ROOTTHREE/6,
				-0, 0, 0,

				0.5, ROOTSIX/9, ROOTTHREE/6,
				1, 0, 0,
			
				0.5, ROOTSIX/9, ROOTTHREE/6,
				0.5, ROOTSIX/3, ROOTTHREE/6,

				0.5, ROOTSIX/9, ROOTTHREE/6,
				0.5, 0, ROOTTHREE/2

			] );

			colors = new Float32Array( [
				1, 0, 0, 
				1, 0, 0, 

				0, 0, 1,
				0, 0, 1,

				0, 1, 0,					
				0, 1, 0,

				0.5, 0.5, 0,					
				0.5, 0.5, 0
			] );


		}

		geometry.addAttribute( 'position', new THREE.BufferAttribute( vertices, 3 ) );
		geometry.addAttribute( 'color', new THREE.BufferAttribute( colors, 3 ) );
	
		group.add( new THREE.LineSegments( geometry, material ));
		return group;


	},

	drawAxesLabels: function( dim, axesName){


	},
	drawData: function( data ){
	
		var group = new THREE.Group();
		group.name  = 'data';
		group.position.set(0, 0, 0);
		// var texture = new THREE.TextureLoader().load('./image/ball.png');
		for (let [name,point] of Object.entries(data)){
			var spriteMaterial = new THREE.SpriteMaterial( {map:Config.dataTexture, color: 0xff0000, alphaTest: 0.5, depthTest: true} );
			var sprite = new THREE.Sprite( spriteMaterial );
			sprite.scale.set(0.01, 0.01, 0.01);
			sprite.position.copy(point);
			sprite.name = name;
			group.add( sprite );
		}
		return group;		
	},



	addScene: function( data, dim ){
		var scope = this;
		var geometries = [
			new THREE.ConeBufferGeometry( 2, 3, 3 ),

		];

		var i = scope.scenes.length;
		var scene = new THREE.Scene();
		// scene.name = i;
		// CREATE PERSPECTIVE CAMERA
		var camera = new THREE.PerspectiveCamera( 50, 1);
		camera.position.z = 2;
		scene.userData.camera = camera;

		// ADD LIGHT
		scene.add( new THREE.HemisphereLight( 0xaaaaaa, 0x444444 ) );
		scene.add( new THREE.HemisphereLight( 0xaaaaaa, 0x444444 ) );
		var light = new THREE.DirectionalLight( 0xffffff, 0.5 );
		light.position.set( 1, 1, 1 );
		scene.add( light );
		
		//ADD AXIS AND DATA
		scene.add( scope.drawGraph( data, dim ) );
		scope.scenes.push( scene );

		// CREATE ELEMENT IN HTML
		var element = this.createSceneContainer(dim.join(),scene);
		scene.userData.element = element.querySelector( ".scene" );
		scope.viewport.appendChild( element );

		// ADD ORBIT CONTROLS
		var controls = new THREE.OrbitControls( scene.userData.camera, scene.userData.element );
		controls.minDistance = 0.5;
		controls.maxDistance = 2;
		// controls.enablePan = false;
		// controls.enableZoom = false;
		scene.userData.controls = controls;
		controls.addEventListener('change', function(){
			scope.signals.renderRequired.dispatch();
		});

		scope.signals.renderRequired.dispatch();
	},

	setSceneSize: function(rules){
		this.styleSheet.deleteRule(0);
		this.styleSheet.insertRule(rules, 0);
		this.signals.renderRequired.dispatch();
	}

};

