var Config = {
    sceneSize:{
        fullScreen:'.list-item {width:98%;height:97%}',
        //two:'.list-item {width:48%;}',
        two:'.list-item {width:47%;height:46%}',
        three:'.list-item {width:32%;height:46%}',
        four:'.list-item {width:24.5%;height:46%}',
    },
    host:'http://localhost:3000',
    colors:{
        SCENEDARK:new THREE.Color(0x030303),
         //SCENEDARK:new THREE.Color(0x363636),
     
        SCENELIGHT: new THREE.Color(0xffffff),
        DATARED: new THREE.Color(0xff3721),
        BLACK: new THREE.Color(0x000000),
        SELECTED: new THREE.Color(0x00ff00),
        
    },
    initPointSize: 8,
    scalar:300,
    lineWidth: 0.007,
    graphTypes : {

		'A,C,G,T':{ codingType: 0, axesNames:['A','C','G','T']},
		'AT,G,C':{ codingType: 4, axesNames:['W','G','C']},
		'AG,C,T':{ codingType: 1, axesNames:['R','T','C']},
		'AC,G,T':{ codingType: 6, axesNames:['K','T','G']},
		'A,TG,C':{ codingType: 5, axesNames:['A','M','C']},
		'A,CT,G':{ codingType: 2, axesNames:['A','Y','G']},
		'A,T,CG':{ codingType: 3, axesNames:['A','T','S']},
		'AT,GC':{ codingType: 8, axesNames:['W','S']},
		'AG,CT':{ codingType: 9, axesNames:['R','Y']},
		'AC,GT':{ codingType: 7, axesNames:['K','M']},
		'ATG,C':{ codingType: 11, axesNames:['D','C']},
		'AGC,T':{ codingType: 13, axesNames:['V','T']},
		'ACT,G':{ codingType: 12, axesNames:['H','G']},
		'GCT,A':{ codingType: 10, axesNames:['B','A']}

	},
    positions : {
		'All position':0,
		'1st codon position':1,
		'2nd codon position':2,
		'3rd codon position':3
	}
    

    // colorScheme:{
    //     writeBlack:{},
    //     colored:{}
    // }
};