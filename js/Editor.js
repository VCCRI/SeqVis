
var ROOTTHREE = 1.7321;
var ROOTSIX = 2.4495;

var Editor = function (  ) {


	this.fullScreenMode = 0;
	this.lockMode = 1;
	this.colorScheme = 0;
	this.animationMode = 0;
	this.canvas = document.createElement('canvas');
	this.canvas.setAttribute('id', 'c');
	document.body.appendChild(this.canvas);

	styleEl = document.createElement('style');
	document.head.appendChild(styleEl);
	this.styleSheet = styleEl.sheet;
	this.styleSheet.insertRule(Config.sceneSize.two,0);

	this.viewport = document.createElement('div');
	this.viewport.setAttribute('id','content');
	

	document.body.appendChild(this.viewport);

	this.addNewEdgeMode = 0;
	this.inputData  = {};

	var Signal = signals.Signal;

	this.signals = {


		screenShot : new Signal(),
		editorCleared: new Signal(),
		changeColorScheme : new Signal(),

		savingStarted: new Signal(),
		savingFinished: new Signal(),
		loadDataUrl: new Signal(),

		refreshAvaiableGames: new Signal(),
		windowResize: new Signal(),
		sceneResize: new Signal(),

		cameraChanged: new Signal(),
		objectSelected: new Signal(),
		objectAdded: new Signal(),
		objectChanged: new Signal(),
		pointSizeChanged: new Signal(),
		refreshSidebarObjectProperties: new Signal(),
		matchedPairsTest: new Signal(),
		addScene: new Signal(),
		setSceneSize: new Signal(),
		deleteScene: new Signal(),	
		fullScreenMode: new Signal(),
		animationRequired: new Signal(),
		stopAnimations: new Signal(),
		renderRequired: new Signal(),
		fileLoaded: new Signal(),
		dataPrepared: new Signal(),

		hideChild: new Signal(),
		selectionModeChanged: new Signal(),
		takeSvgImage : new Signal()
	};


	this.scenes = [];
	this.selected = new Set();
	
	this.fullScreenScene = null;
	this.searchPatten = '';

};

Editor.prototype = {

	select: function ( intersect ) {

		//remove select
		this.changeDataColors(Config.colors.DATARED);
		this.selected = new Set();
		this.searchPatten = '';

		this.selected.add(intersect.index);
		
		this.changeDataColors( Config.colors.SELECTED );

		this.signals.objectSelected.dispatch( this.selected );
		this.signals.renderRequired.dispatch();
	},

	selectByName : function( name ) {

		name = name.toUpperCase();
		if ( this.searchPatten == name || this.name == '') return;

		//remove select
		this.changeDataColors(Config.colors.DATARED);
		this.selected = new Set();

		var keyList = Object.keys(this.inputData);

		for ( var i = 0 ; i < keyList.length; i+= 1 ) {

			if ( keyList[i].toUpperCase().includes(name)){
				this.selected.add(i);
			}
		}
		
		this.searchPatten = name;

		this.changeDataColors( Config.colors.SELECTED );
		if(this.selected.length != 0) this.signals.objectSelected.dispatch( this.selected  );
		this.signals.renderRequired.dispatch();
	


	},
	changePointSize: function( size ) {
		for( let scene of this.scenes) {
			
			var pointsMaterial = scene.children[1].children[0].children[2].material;
			pointsMaterial.size = size;
			pointsMaterial.needsUpdate = true;
	
		}
		this.signals.renderRequired.dispatch();

	},

	changeDataColors: function( color ) {

		var indices = Array.from(this.selected);

		for ( let scene of this.scenes ) {

			var colorAttribute = scene.children[1].children[0].children[2].geometry.attributes.color;
			var colorArray = colorAttribute.array;
			for (let index of indices) {

				colorArray[3*index] = color.r;
				colorArray[3*index+1] = color.g;
				colorArray[3*index+2] = color.b;

			
			}
			colorAttribute.needsUpdate = true;
		}
	},


	
	resetViewport:function(){

	
		while (this.scenes.length > 0){
					this.deleteScene(this.scenes[0].userData.element.parentNode,this.scenes[0].uuid);
		}

		this.selected = new Set();
		this.fullScreenScene = null;
		this.fullScreenMode = 0;
		this.setSceneSize(Config.sceneSize.two);
		this.signals.renderRequired.dispatch();
	},

	deleteScene:function(listItem,uuid){
	
		var index = 0;
	
		for( var scene of this.scenes){
			if(scene.uuid != uuid) index += 1;
			else break;
		}
		TWEEN.remove(this.scenes[index].userData.animation);
		this.scenes.splice(index, 1);
		this.viewport.removeChild(listItem);
		this.signals.renderRequired.dispatch();
	
		// this.scenes;
	},

	hideOtherScenes:function(scene){
		

		for ( var currScene of this.scenes ) {

			if (currScene.uuid != scene.uuid ) {
			
				currScene.userData.element.parentElement.style.display = 'none';
			}
			this.setSceneSize(Config.sceneSize.fullScreen);
			// this.signals.renderRequired.dispatch();
			
		}

	},

	displayAllScenes:function() {
		for ( var currScene of this.scenes ) {
			currScene.userData.element.parentElement.style.display = '';
		}
		// this.signals.renderRequired.dispatch();
	},

	createSceneContainer:function(sceneName,scene){

		scope = this;
		var listItem = document.createElement('div');
		listItem.setAttribute('class','list-item');

		var sceneContainer = document.createElement('div');
		sceneContainer.setAttribute('class','scene');

		sceneContainer.addEventListener( 'dblclick', function(){
			if ( scope.fullScreenMode == 0 ) {
				scope.fullScreenMode = 1;
				scope.signals.fullScreenMode.dispatch(true, scene);
			}
			else {
				scope.fullScreenMode = 0;
				scope.signals.fullScreenMode.dispatch(false, null);
			}
		}, false );

		var footRow = document.createElement('div');
		footRow.setAttribute('class','foot');
		var description = document.createElement('div');
		description.setAttribute('class','description');
		description.innerText = sceneName;
		footRow.appendChild(description);
		

		var toggleAxesButton = document.createElement('button');
		toggleAxesButton.setAttribute('class','optionButton');
		toggleAxesButton.style['background-image'] = 'url(./image/axes.png)';
		footRow.appendChild(toggleAxesButton).onclick = function() {
			scope.signals.hideChild.dispatch(scene, 'axes');
		};

		var toggleWireframeButton = document.createElement('button');
		toggleWireframeButton.setAttribute('class','optionButton');
		toggleWireframeButton.style['background-image'] = 'url(./image/triangle.png)';
		footRow.appendChild(toggleWireframeButton);
		toggleWireframeButton.onclick = function() {
			scope.signals.hideChild.dispatch(scene, 'wireframe');
		};


		var toggleLabelButton = document.createElement('button');
		toggleLabelButton.setAttribute('class','optionButton');
		toggleLabelButton.style['background-image'] = 'url(./image/acgt.png)';
		toggleLabelButton.onclick = function() {
			scope.signals.hideChild.dispatch(scene, 'labels');
		};
		footRow.appendChild(toggleLabelButton);

		var svgButton = document.createElement('button');
		svgButton.setAttribute('class','optionButton');
		svgButton.style['background-image'] = 'url(./image/icon_svg.png)';
		
		svgButton.onclick = function(){
			scope.signals.takeSvgImage.dispatch(scene);
		};
		footRow.appendChild(svgButton);

		var mptButton = document.createElement('button');
		mptButton.setAttribute('class','optionButton');
		mptButton.style['background-image'] = 'url(./image/mpt.png)';
		
		mptButton.onclick = function(){
			console.log(Config.positions[sceneName]);
			scope.signals.matchedPairsTest.dispatch(scene.userData.graphType.codingType,scene.userData.sceneName);
		};
		footRow.appendChild(mptButton);

		var closeButton = document.createElement('button');
		closeButton.setAttribute('class','optionButton');
		closeButton.style['background-image'] = 'url(./image/cross.png)';
		closeButton.onclick = function(){
			//TODO signal based or not
			scope.deleteScene(listItem,scene.uuid);
		};

		footRow.appendChild(closeButton);

		listItem.appendChild(sceneContainer);
		listItem.appendChild(footRow);

		return listItem;

	},

	drawGraph : function( scene, renderType, axesNames, data  ) {
		
		var dimNo = axesNames.length-1;
		var graphContainer = new THREE.Group();
		var graphGroup = new THREE.Group();
		graphContainer.add(graphGroup);

		if(dimNo == 1) graphGroup.position.set(-0.5*Config.scalar, 0, 0);
		if(dimNo == 2) graphGroup.position.set(-0.5*Config.scalar, -ROOTTHREE/6*Config.scalar, 0);
		if(dimNo == 3) graphGroup.position.set(-0.5*Config.scalar, -ROOTTHREE/9*Config.scalar, -ROOTTHREE/6*Config.scalar);
	

		var points;
		if(renderType == 'render'){
			graphGroup.add(this.drawWireframe( dimNo, renderType,true ));
			graphGroup.add(this.drawAxes( dimNo, axesNames, renderType, true, true ));
			points = this.drawData( data );
			scene.userData.objects = points;
		}else{
			graphGroup.add(this.drawWireframe( dimNo, renderType,scene.children[1].children[0].children[0].visible ));
			console.log(scene.children[1].children[0].children[1].children[0]);
			graphGroup.add(this.drawAxes( dimNo, axesNames, renderType, scene.children[1].children[0].children[1].children[0].visible, scene.children[1].children[0].children[1].children[1].visible,  scene.userData.camera.quaternion ));
			
			material = scene.children[1].children[0].children[2].material.clone();
			geometry = scene.children[1].children[0].children[2].geometry.clone();
			points = new THREE.Points( geometry, material );
			points.material.color = Config.colors.BLACK;
			points.material.needsUpdate = true;
		}
		graphGroup.add(points);

	

	
		return graphContainer;	
		
	}, 


	drawLineSegments2 : function (geoBColor,geoCColor, linewidth, vertices){

		var geometry = new THREE.LineSegmentsGeometry();
		geometry.colored = geoCColor;
		geometry.bcolored = geoBColor;
		geometry.setPositions(vertices);
		if(this.colorScheme == 0) geometry.setColors( geoCColor );
		else geometry.setColors( geoBColor );

		var material = new THREE.LineMaterial( {
			color: 0xffffff,
			linewidth: linewidth, 
			vertexColors: THREE.VertexColors,
			dashed: false
		} );
	
		return new THREE.LineSegments2( geometry, material );
	},

	drawLineSegments : function (linewidth, vertices){

		var geometry = new THREE.BufferGeometry();
		// geometry.colored = geoCColor;
		// geometry.bcolored = geoBColor;
		geometry.addAttribute( 'position', new THREE.BufferAttribute( new Float32Array(vertices), 3 ) );
		// geometry.addAttribute( 'color', new THREE.BufferAttribute( new Float32Array(geoBColor), 3 ) );
		var material = new THREE.LineBasicMaterial({ linewidth:5, color: 0x00000 });

		return new THREE.LineSegments( geometry, material );
	},

	drawWireframe : function ( dimNo, type,drawWireframe ) {
		var group = new THREE.Group();
		group.position.set(0,0,0);
		group.name = 'wireframe';
		group.scale.set(Config.scalar,Config.scalar,Config.scalar);

		var vertices;
		var coloredColors;
		var blackColors;

		// DRAW GRAPH SHAPES
		if(dimNo == 1) {
		
			vertices =[
				//l-r
				0, 0, 0,
				1, 0, 0
			];
			
			coloredColors =  [
				1, 0, 0,
				0, 0, 1
			];
			
			blackColors = [
				0, 0, 0,
				0, 0, 0
			];
			
		}
		
		else if(dimNo == 2) {

			vertices =[
				//t-l
				0.5, ROOTTHREE/2, 0,
				0, 0, 0,
				//t-r
				0.5, ROOTTHREE/2, 0,
				1, 0, 0,
				//l-r
				0, 0, 0,
				1, 0, 0
			];
			
			coloredColors = [
				0, 1, 0, 
				1, 0, 0,
				
				0, 1, 0, 
				0, 0, 1,				
				
				1, 0, 0,
				0, 0, 1
			] ;
	
			blackColors = [
				0, 0, 0,
				0, 0, 0,
				
				0, 0, 0,
				0, 0, 0,
				
				0, 0, 0,
				0, 0, 0
			];

		}
		
		else if(dimNo == 3) {
		
			vertices =  [
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
			];
			
			coloredColors = [
				0, 1, 0, 
				1, 0, 0,
				
				0, 1, 0, 
				0, 0, 1,
				
				0, 1, 0, 		
				1, 1, 0,
				
				1, 0, 0,
				0, 0, 1,
				
				1, 0, 0,
				1, 1, 0,
				
				0, 0, 1,
				1, 1, 0
			];
			
			blackColors = [
				0, 0, 0,
				0, 0, 0,

				0, 0, 0,
				0, 0, 0,
				
				0, 0, 0,
				0, 0, 0,
				
				0, 0, 0,
				0, 0, 0,
				
				0, 0, 0,
				0, 0, 0,
				
				0, 0, 0,
				0, 0, 0
			];			
		}
		if(type == 'render') group.add(this.drawLineSegments2(blackColors,coloredColors, Config.lineWidth, vertices));
		else if(drawWireframe) group.add(this.drawLineSegments(Config.lineWidth, vertices));
		return group;
	},

	calcuateT: function(start,center){
		var vec = new THREE.Vector3().subVectors(center,start).normalize().multiplyScalar(0.1);
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

	drawAxes: function ( dimNo, axesNames, renderType, drawAxes, drawLabels, quaternion ) {

		var scope = this;
		var group = new THREE.Group();
		group.position.set(0,0,0);
		
		group.name = 'axesAndLabel';
		group.scale.set(Config.scalar,Config.scalar,Config.scalar);

		var axesGroup = new THREE.Group();
		axesGroup.position.set(0,0,0);
		axesGroup.visible = false;
		
		var labelsGroup = new THREE.Group();
		labelsGroup.position.set(0,0,0);

		group.add(axesGroup);
		// group.add(labelsGroup);
	
		if(renderType == 'render'){
			scope.drawAxesLabels(axesNames,function(axeslabelgroup){

				group.add(axeslabelgroup);
				scope.signals.renderRequired.dispatch();

			});
		}

		else if(drawLabels) {
			group.add(scope.drawSVGAxesLabels(axesNames,quaternion));

		}

		

		if (dimNo <= 1)  return group;

		var vertices;
		var coloredColors;
		var blackColors;

		if(dimNo == 2  ){
			vertices = [
				
				0.5866, 0.3387, 0,
				0, 0, 0,

				0.4134, 0.3387, 0,
				1.0, 0, 0,
			
				0.5, 0.1887, 0,
				0.5, ROOTTHREE/2, 0,

			] ;

			coloredColors = [
				1, 0, 0, 
				1, 0, 0, 

				0, 0, 1,
				0, 0, 1,

				0, 1, 0,					
				0, 1, 0
			];
	

			blackColors = [
				0, 0, 0, 
				0, 0, 0, 

				0, 0, 0,
				0, 0, 0,

				0, 0, 0,					
				0, 0, 0
			];
		}

		else if( dimNo == 3 ){

			vertices =  [
				
				0.5783, 0.3148, 0.3339,
				0, 0, 0,

				0.4217, 0.3148, 0.3339,
				1, 0, 0,
			
				0.5, 0.1722, 0.2887,
				0.5, ROOTSIX/3, ROOTTHREE/6,

				0.5, 0.3148, 0.1982,
				0.5, 0, ROOTTHREE/2

			] ;

			coloredColors = [
				1, 0, 0, 
				1, 0, 0, 

				0, 0, 1,
				0, 0, 1,

				0, 1, 0,					
				0, 1, 0,

				1, 1, 0,					
				1, 1, 0
			];

			blackColors = [
				0, 0, 0, 
				0, 0, 0, 

				0, 0, 0,
				0, 0, 0,

				0, 0, 0,
				0, 0, 0,

				0, 0, 0,					
				0, 0, 0
			];
		}
			

		// this.calcuateLabel(new THREE.Vector3(0, 0, 0),new THREE.Vector3(0.5, ROOTSIX/9, ROOTTHREE/6));
		// this.calcuateLabel(new THREE.Vector3(1, 0, 0),new THREE.Vector3(0.5, ROOTSIX/9, ROOTTHREE/6));
		// this.calcuateLabel(new THREE.Vector3(0.5, ROOTSIX/3, ROOTTHREE/6),new THREE.Vector3(0.5, ROOTSIX/9, ROOTTHREE/6));
		// this.calcuateLabel(new THREE.Vector3(0.5, 0, ROOTTHREE/2),new THREE.Vector3(0.5, ROOTSIX/9, ROOTTHREE/6));

		if(renderType == 'render') axesGroup.add(this.drawLineSegments2(blackColors,coloredColors, Config.lineWidth, vertices));
		else if(drawAxes) {axesGroup.add(this.drawLineSegments(Config.lineWidth, vertices));axesGroup.visible = true;}

		return group;


	},

	createAnimations: function(){
		TWEEN.removeAll();
		for ( var scene of this.scenes){
			this.createAnimation(scene.children[1]);

		}

	},
	createAnimation: function(container){
		var tween = new TWEEN.Tween(container.rotation.clone())
		.to(new THREE.Euler(0,container.rotation.y+6.28, 0), 7000)
		.onUpdate(function() {
			
			container.rotation.copy(new THREE.Euler(this.x,this.y,this.z) );
			container.updateMatrixWorld( true );
		}).repeat(Infinity);
		if(this.animationMode == 1) tween.start();
	},

	startAnimations: function(){
		for(let scene of this.scenes){
	
			scene.userData.animation.start();
		}

	},
	drawSVGAxesLabels :function(axesNames,quaternion){

		var scope = this;
		var group = new THREE.Group();
		group.position.set(0,0,0);
		var coloredColors;
		var len = axesNames.length;
		var coordinates;
		if( len == 2 ) {
			coordinates = [
				new THREE.Vector3(0,0,0),
				new THREE.Vector3(1,0,0)
			];
		}
		else if(len == 3) {
			coordinates = [
				new THREE.Vector3( 0.5, 0.9661, 0 ),
				new THREE.Vector3( -0.0866, -0.05, 0 ),
				new THREE.Vector3( 1.0866, -0.05, 0 )
			];
		}
		else if (len == 4) {
			coordinates = [	
				new THREE.Vector3 ( 0.5, 0.9165, 0.2887),
				new THREE.Vector3 ( -0.0783,  -0.0426,  -0.0452),
				new THREE.Vector3 ( 1.0783, -0.0426, -0.0452),
				new THREE.Vector3 ( 0.5, -0.0426, 0.9565)
			];
		}



		var matLite = new THREE.MeshBasicMaterial( {
			color: 0x000000,
			side: THREE.DoubleSide
		} );
		
		for ( var i = 0;i<len;i+=1){

			var xMid, text;
			var textGroup = new THREE.Group();
			textGroup.position.copy(coordinates[i]);
			textGroup.quaternion.copy( quaternion );
			
			var shapes = Config.font.generateShapes( axesNames[i],0.125);
			var geometry = new THREE.ShapeBufferGeometry( shapes );
			geometry.computeBoundingBox();
			xMid = - 0.5 * ( geometry.boundingBox.max.x - geometry.boundingBox.min.x );
			var p = coordinates[i];
			geometry.translate( xMid,0,0 );
			text = new THREE.Mesh( geometry, matLite );
			textGroup.add(text);
			group.add( textGroup );


		}
		return group;

	},

	drawAxesLabels: function(axesNames,callback){
		
		var scope = this;
		var group = new THREE.Group();
		group.position.set(0,0,0);
		var coloredColors;
		var len = axesNames.length;
		var coordinates;
		if( len == 2 ) {
			coordinates = [
				new THREE.Vector3(0,0,0),
				new THREE.Vector3(1,0,0)
			];
			coloredColors = [0xff0000, 0x0000ff];
		
		}
		else if(len == 3) {
			coordinates = [
				new THREE.Vector3( 0.5, 0.9661, 0 ),
				new THREE.Vector3( -0.0866, -0.05, 0 ),
				new THREE.Vector3( 1.0866, -0.05, 0 )
			];
			coloredColors  = [0x00ff00, 0xff0000, 0x0000ff];
		}
		else if (len == 4) {
			coordinates = [	
				new THREE.Vector3 ( 0.5, 0.9165, 0.2887),
				new THREE.Vector3 ( -0.0783,  -0.0426,  -0.0452),
				new THREE.Vector3 ( 1.0783, -0.0426, -0.0452),
				new THREE.Vector3 ( 0.5, -0.0426, 0.9565)
			];
			coloredColors  = [0x00ff00, 0xff0000, 0x0000ff, 0xffff00];
		}
		var counter = 0;

		var loader = new THREE.TextureLoader();

		function drawLabels(axesName, coordinate, color){
			loader.load(
			
				'./image/'+axesName+'.png',

				function ( texture ) {
				
					// if(len!=3) return;
					if( scope.colorScheme == '0' ) var sprite = scope.drawSprite(coordinate, 0.2, texture, new THREE.Color(color), 'label');
					
					else var sprite = scope.drawSprite(coordinate, 0.2, texture, Config.colors.BLACK, 'label');
					
					sprite.material.colored = new THREE.Color(color);
					group.add( sprite);
					counter += 1;
					if(counter == len) callback(group);
			});
		}

		for ( var i = 0;i<len;i+=1){

			drawLabels(axesNames[i],coordinates[i],coloredColors[i]);

		}
		
		return group;
	},
	
	changeBufferGeometryColorScheme : function ( parent, type ){
		
		for(let child of parent.children){
			var colorAttribute = child.geometry.attributes.color;
			var colored = child.geometry.colored;
			var colorArray = colorAttribute.array;
			if( type == '0' ){		
				
				for( var index = 0; index < colorArray.length; index += 1 ) {
					
					colorArray[index] = colored[index];
				}
			}
			else{
				
				for( var index = 0; index < colorArray.length; index += 1 ) {
					
					colorArray[index] = 0;
				}
			}
			colorAttribute.needsUpdate = true;
			
			
		}

	},
	changeLineGeometryColorScheme : function ( parent, type ){
	
		
		for(let child of parent.children){
		
			// var colored = child.geometry.colored;
		
			if( type == '0' ){		
				child.geometry.setColors(child.geometry.colored);
	
			}
			else{
				child.geometry.setColors(child.geometry.bcolored);
	
			}
		
			
		}

	},
	
	changeSpriteColorScheme: function ( parent, type ){
	
		for(let child of parent.children){
			
			if(type == '1') child.material.color = Config.colors.BLACK;
			else child.material.color = child.material.colored;
			child.material.needsUpdate = true;
		}
		

	},
	drawData: function( data ){

		var group = new THREE.Group();
		// group.name  = 'data';
		group.position.set(0, 0, 0);
		var positions = [];
		var colors = [];

		var color = Config.colors.DATARED;
		var geometry = new THREE.BufferGeometry();

		for (let [name,pointInfo] of Object.entries(data)){
		
			positions.push( pointInfo.position.x, pointInfo.position.y, pointInfo.position.z );
			colors.push( color.r, color.g, color.b );

		}
		geometry.addAttribute( 'position', new THREE.Float32BufferAttribute( positions, 3 ));
		geometry.addAttribute( 'color', new THREE.Float32BufferAttribute( colors, 3 ) );
		var material = new THREE.PointsMaterial( { size: Config.initPointSize, vertexColors: THREE.VertexColors, alphaTest: 0.5, depthTest: true,map:Config.dataTexture } );
		var points = new THREE.Points( geometry, material );
		points.name = 'data';
		// group.add( points );

		return points;		
	},

	drawSprite: function(position,scale,texture,color,name, freq){

		var spriteMaterial = new THREE.SpriteMaterial( {map:texture, color: color, alphaTest: 0.5, depthTest: true} );
		var sprite = new THREE.Sprite( spriteMaterial );
		sprite.scale.set(scale, scale, scale);
		sprite.position.copy(position);
		sprite.name = name;
		return sprite;

	},


	addScene: function(sceneName, data, dim ){

		var graphType = Config.graphTypes[dim];

		// var increamental;
		// if(sceneName == 'All position') increamental = 1;
		// else if(sceneName == '1st codon position')
		// else if(sceneName == '2nd codon position')
		// else 
		var scope = this;
		var scene = new THREE.Scene();
		scene.userData.graphType = graphType;
		// CREATE ELEMENT IN HTML
		scene.userData.sceneName = sceneName;
		var element = this.createSceneContainer(sceneName,scene);
		scene.userData.element = element.querySelector( ".scene" );
		scope.viewport.appendChild( element );
		var dom = scene.userData.element;
		
		// ADD LIGHT
		var light = new THREE.AmbientLight( 0x404040 ); // soft white light
		scene.add( light );
		if(scope.colorScheme == 0)	scene.background = Config.colors.SCENEDARK;
		else scene.background = Config.colors.SCENELIGHT;
		scope.scenes.push( scene );

		// CREATE PERSPECTIVE CAMERA
		var camera = new THREE.PerspectiveCamera( 50, 1);
		camera.position.x = 0;
		camera.position.z = 550;
		scene.userData.camera = camera;
		camera.aspect = dom.offsetWidth / dom.offsetHeight;
		camera.updateProjectionMatrix();

		// ADD ORBIT CONTROLS
		var orbitControls = new THREE.OrbitControls( scene.userData.camera, scene.userData.element );
		orbitControls.minDistance = 0.1;
		orbitControls.maxDistance = 700;
		orbitControls.enablePan = false;
		orbitControls.autoRotate = false;
		// orbitControls.enableZoom = false;
		orbitControls.addEventListener('change', function(){
			
			if (scope.lockMode == 1 ){

				scope.scenes.forEach(function (otherScene) {

					if(scene != otherScene) {
						var otherCamera  = otherScene.userData.camera;
						var camera = scene.userData.camera;
						otherCamera.position.copy(camera.position);
						otherCamera.scale.copy(camera.scale);
						otherCamera.rotation.copy(camera.rotation);
					}
					
				});

			}
			scope.signals.renderRequired.dispatch();
		});
		scene.userData.orbitControls = orbitControls;
		scene.userData.graphType = graphType;
		//ADD AXIS AND DATA
		var container = scope.drawGraph(scene,'render', graphType.axesNames, data );
		scene.add( container );

		//ADD OBJECT SELECTION CONTROLS
		scope.objectPicking(scene);

		//CREATE TWEEN ANIMATION
		scene.userData.animation = scope.createAnimation(container);
		scope.signals.renderRequired.dispatch();

	},

	objectPicking:function(scene){
		var scope = this;
		var raycaster = scene.userData.raycaster = new THREE.Raycaster();
		var mouse = scene.userData.mouse = new THREE.Vector2();
		var objects = scene.userData.objects;
		var camera = scene.userData.camera;
		var container = scene.userData.element;
		
		// events

		function getIntersects( point, objects ) {

			mouse.set( ( point.x * 2 ) - 1, - ( point.y * 2 ) + 1 );

			raycaster.setFromCamera( mouse, camera );

			return raycaster.intersectObject( objects );

		}

		var onDownPosition = new THREE.Vector2();
		var onUpPosition = new THREE.Vector2();
		var onDoubleClickPosition = new THREE.Vector2();

		function getMousePosition( dom, x, y ) {

			var rect = dom.getBoundingClientRect();
			return [ ( x - rect.left ) / rect.width, ( y - rect.top ) / rect.height ];

		}


		function handleClick() {
			
			if ( onDownPosition.distanceTo( onUpPosition ) === 0 ) {
			
				var intersects = getIntersects( onUpPosition, objects );

				if ( intersects.length > 0 ) {

					scope.select( intersects[ 0 ] );
	

				} 

			}

		}

		function onMouseDown( event ) {

			event.preventDefault();
		
			var array = getMousePosition( container, event.clientX, event.clientY );
			onDownPosition.fromArray( array );

			document.addEventListener( 'mouseup', onMouseUp, false );

		}

		function onMouseUp( event ) {

			var array = getMousePosition( container, event.clientX, event.clientY );
			onUpPosition.fromArray( array );

			handleClick();

			document.removeEventListener( 'mouseup', onMouseUp, false );

		}

		function onTouchStart( event ) {

			var touch = event.changedTouches[ 0 ];

			var array = getMousePosition( container, touch.clientX, touch.clientY );
			onDownPosition.fromArray( array );

			document.addEventListener( 'touchend', onTouchEnd, false );

		}

		function onTouchEnd( event ) {

			var touch = event.changedTouches[ 0 ];

			var array = getMousePosition( container, touch.clientX, touch.clientY );
			onUpPosition.fromArray( array );

			handleClick();

			document.removeEventListener( 'touchend', onTouchEnd, false );

		}

		container.addEventListener( 'mousedown', onMouseDown, false );
		container.addEventListener( 'touchstart', onTouchStart, false );

	},

	setSceneSize: function(rules){
		this.styleSheet.deleteRule(0);
		this.styleSheet.insertRule(rules, 0);
		this.signals.sceneResize.dispatch();
	},

	clear :function(){

		this.inputData = {};
		this.searchPatten = '';
	
		this.animationMode = 0;

		this.addNewEdgeMode = 0;

		this.resetViewport();
		this.signals.editorCleared.dispatch();


	},

};

