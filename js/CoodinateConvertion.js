
    
var ROOTTHREE = 1.7321;
var ROOTSIX = 2.4495;

// group ('A,C,G,T'  'AC,GT')
function getCoordinates(  seqInfo, position, dim ) {

    var axesNames = Config.graphTypes[dim].axesNames;
    groups = dim.split(',');	
          
    if (groups.length == 2) {
          console.log(1);
        return oneDimCoordinates(seqInfo, position, groups, axesNames);
    }
    else if (groups.length == 3) {
                console.log(3);
        return twoDimCoordinates(seqInfo, position, groups, axesNames);
    }
    else if (groups.length == 4) {
        console.log(4);
        return threeDimCoordinates(seqInfo,position);
    }
    
// var y =3√2t+3√6f.

}

function oneDimCoordinates(seqInfo, position, groups, axesNames)  {
    
    var data = {};
    
    var firstVertex = groups[0].split('');

    var firstAxes = axesNames[0];
    var secondAxes = axesNames[1];
        
    for( let [name,seq] of Object.entries(seqInfo) )  {
        
        var x = 0;
        var point = seq[position];

        for ( let axes of firstVertex) { 
            x += point[axes];
        }
        data[name] = {};
        data[name].position = new THREE.Vector3((1-x)*Config.scalar,0,0);

        data[name].frequence = {};
        data[name].frequence[firstAxes] = x;
        data[name].frequence[secondAxes] = 1-x;


        
    }
    
    return data;
    
    
}
// t l r
function twoDimCoordinates(seqInfo, position, groups, axesNames)  {
    
    var data = {};

    
    var firstVertex = groups[0].split('');
    var secondVertex = groups[1].split('');
    var thirdVertex = groups[2].split('');

    var firstAxes = axesNames[0];
    var secondAxes = axesNames[1];
    var thirdAxes = axesNames[2];

    for( let [name,seq] of Object.entries(seqInfo) ) {
        
        var t = 0;
        var l = 0;
        var r = 0;
        var point = seq[position];

        for ( let axes of firstVertex) { t += point[axes]; }
        for ( let axes of secondVertex) { l += point[axes]; }
        for ( let axes of thirdVertex) { r += point[axes]; }

        var x = (r + 1 - l)/2
        var y = ROOTTHREE*t/2;
        

        data[name] = {};
        data[name].position = new THREE.Vector3(x*Config.scalar,y*Config.scalar,0);

        data[name].frequence = {};
        data[name].frequence[firstAxes] = t;
        data[name].frequence[secondAxes] = l;
        data[name].frequence[thirdAxes] = r;
        
        
    }
          
    return data;
    
    
}
//A  C  G  T

//t  l  r  f
function threeDimCoordinates(seqInfo,position)  {
    
    var data = {};

    for( let [name,seq] of Object.entries(seqInfo) )   {
        var point = seq[position];
        var x = (point.G + 1 - point.C)/2;
        var y = ROOTSIX*point.A/3;
        var z = ROOTTHREE*point.T/2 + ROOTTHREE*point.A/6;
        data[name] = {frequence:point, position: new THREE.Vector3(x*Config.scalar,y*Config.scalar,z*Config.scalar)};
        
    }
    
    return data;
    
}
    

    