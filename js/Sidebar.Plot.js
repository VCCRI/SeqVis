

Sidebar.Plot = function ( editor ) {

	var container = new UI.Panel();
	var signals = editor.signals;
	// var inputData = null;
	container.add( new UI.Break(),new UI.Break());

	var viewType = {
		'allPositionFreq':'All position',
		'firstPositionFreq':'1st codon position',
		'secondPositionFreq':'2nd codon position',
		'thirdPositionFreq':'3rd codon position'
	};
	var viewOptionRow = new UI.Row();
	var viewOptions = new UI.Select().setOptions( viewType ).setWidth( '120px' ).setValue('allPositionFreq').setFontSize( '12px' );
	viewOptionRow.add( new UI.Text( 'Choose view' ).setWidth( '120px' ) );
	viewOptionRow.add(viewOptions);
	container.add( viewOptionRow );

	//PLOT TYPE
	var plotTypes = {
		'A,C,G,T':'ACGT',

		'AT,G,C':'WGC (pool A and T)',
		'AG,C,T':'RTC (pool A and G)',
		'AC,G,T':'KTG (pool A and C)',
		'A,TG,C':'MAC (pool T and G)',
		'A,CT,G':'YAG (pool T and C)',
		'A,T,CG':'SAT (pool C and G)',

		'AT,GC':'WS (AT vs GC)',
		'AG,CT':'RY (AG vs CT)',
		'AC,GT':'KM (AC vs GT)',
		'ATG,C':'DC (ATG vs C)',
		'AGC,T':'VT (AGC vs T)',
		'ACT,G':'HG (ACT vs G)',
		'GCT,A':'BA (GCT vs A)'
	};
		// if(codingType == 0) fprintf(outFile,"ACGT (A|C|G|T)\n");
		// if(codingType == 1) fprintf(outFile,"CTR (C|T|AG)\n");
		// if(codingType == 2) fprintf(outFile,"AGY (A|G|CT)\n");
		// if(codingType == 3) fprintf(outFile,"ATS (A|T|CG)\n");
		// if(codingType == 4) fprintf(outFile,"CGW (C|G|AT)\n");
		// if(codingType == 5) fprintf(outFile,"ACK (A|C|GT)\n");
		// if(codingType == 6) fprintf(outFile,"GTM (G|T|AC)\n");
		// if(codingType == 7) fprintf(outFile,"KM (GT|AC)\n");
		// if(codingType == 8) fprintf(outFile,"SW (GC|AT)\n");
		// if(codingType == 9) fprintf(outFile,"RY (AG|CT)\n");
		// if(codingType == 10) fprintf(outFile,"AB (A|CGT)\n");
		// if(codingType == 11) fprintf(outFile,"CD (C|AGT)\n");
		// if(codingType == 12) fprintf(outFile,"GH (G|ACT)\n");
		// if(codingType == 13) fprintf(outFile,"TV (T|ACG)\n");

		


	var plotOptionsRow = new UI.Row();
	var plotOptions = new UI.Select().setOptions( plotTypes ).setWidth( '120px' ).setValue('A,C,G,T').setFontSize( '12px' );
	plotOptionsRow.add( new UI.Text( 'Choose plots' ).setWidth( '120px' ) );
	plotOptionsRow.add(plotOptions);
	container.add( plotOptionsRow );


	//ADD PlOT
	var addPlotButton = new UI.Button( 'Add plot' );
	addPlotButton.onClick( function () {
		if (editor.inputData == null) return; 
		var graphTypeIndex = plotOptions.getValue();
		// var axesName = Config.axesNames[dim].axesName;
	
		var positionInfo = viewOptions.getValue();
		
		var newCoordinates = getCoordinates(editor.inputData, positionInfo, graphTypeIndex);
		signals.addScene.dispatch(viewType[positionInfo],newCoordinates,graphTypeIndex);

	} );
	container.add(addPlotButton);

	container.add( new UI.Break(),new UI.Break());



	signals.dataPrepared.add(function( ){
		console.log('dataPrepared');
		var singleCoordinates = getCoordinates(editor.inputData,'allPositionFreq' ,'A,C,G,T');

		signals.addScene.dispatch(viewType.allPositionFreq, singleCoordinates,'A,C,G,T');

		var firstPosCoordinates = getCoordinates(editor.inputData,'firstPositionFreq', 'A,C,G,T');
		signals.addScene.dispatch(viewType.firstPositionFreq,firstPosCoordinates,'A,C,G,T');

		var secondPosCoordinates = getCoordinates(editor.inputData,'secondPositionFreq', 'A,C,G,T');
		signals.addScene.dispatch(viewType.secondPositionFreq,secondPosCoordinates,'A,C,G,T');

		var thirdPosCoordinates = getCoordinates(editor.inputData,'thirdPositionFreq', 'A,C,G,T');
		signals.addScene.dispatch(viewType.thirdPositionFreq,thirdPosCoordinates,'A,C,G,T');
		
		
	});

	
	if(fileData != '') signals.fileLoaded.dispatch(fileData,fileType);
	fileData = '';
	fileType = '';
	
	return container;

};